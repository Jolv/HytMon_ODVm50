/* ***********************************************
*
* Hytmon is a method for monitoring status at the cottage
* when it is closed for the winter.  An alarm is sent
* immediately if the electricity fails. Other situations
* are signaled in the "morning report" sent every morning
* at a specific time of the day 16/10-15
*--------------
* SMS stopped 17/12 approx 100 sms after start.
* I think it is because  the sms counter was not reset
* when reset btn was pushed. Included setting counter
* to 0 in in cmdq.initiate in setup()
*********************************************** */
#include <SoftwareSerial.h>
#include "StrongDeBounce.h"
#include "SmsAndCmdDefFile.h"
#define spnt Serial.print
#define spntln Serial.println
#define dp spnt
#define dpl spntln
#define PRNT_FLG true
#define NOPRNT_FLG false

char car; //temporary character
String  ioBuffGPRS="";
const int pinTmprFrz=1; // T1  Pin 1 og 2 bør byttes <<<
const int pinTmprCottage=0; // T2  Pin 1 og 2 bør byttes <<<
const int pinLedPwrDown=2; //L1 
const int pinLedTeprLow=3; //L2
const int pinLedMusTrapped=4;//L3
const int pinTestSw=5; // S1
const int pin6=6; // Not in use
// pins 7,8 for Software-serial, see gprsSerial below
const int pinPwrGprs=9, pinSmsQueLED=13; 
// pin 10 has hw problems
const int pinPwrMains=11; // Contact D1 
//const int pinMusTrp1=11; // Not in use
const int pinMusTrp1=12; // Contact D2
const int loopLED=13; // Contact D2
// Important: 'postfix' _ds indicates
// a counter in deca sec ie. a 10 sec unit
// read separate document for timing synchronization
const int synchClk_ds=2520; //Prm const:0700*60*6=2520, 16:30=
const int moRepoClk_ds=2880;    //Prm const:0800*60*6=2880
const int synchTolerance_ds=180;//Prm:  30 min*6=180
const int fullCycle_ds=8640;    //Prm const:24hr*60*6
int nextSynchTime_ds; 
int nextMoRepoTime_ds;
int gprsTime_ds=0;
int loopTime_ds=0;	 //deca (10) seconds counter
	// 'loopTime_ds' will drift due to use of delay(), millis()
	// also drift, and numbers gets impractically large
	// loopTime is synchronized with real time (GprsTime) every morning
const int MaxNumberOfSMS = 90;
int cnSmsMsgPerDay=0; // Counts number of attempts to send SMS pr 24hr
					  // reset in daily time synch. sequence
const int cnSmsMsgPerDayLim=8; //Prm: 
bool synchEnableFlg=true; // controls only one synching pr 24hr
bool moRepoEnableFlg=false;	// controls only one morning report pr 24hr		
byte hr,min,sec; // holds Gprs values of hr, min & sec.
// Declare serial for communication with GPRS
SoftwareSerial gprsSerial(7,8);
// #define gprsSerial Serial
// Definitions/instantiations for handling sensors
bool detectorDummyFlg; // temporary flag
bool musCaughtFlg, mainsPwrDownFlg; // Reports current detection status
bool musCaughtReportFlg, mainsPwrDownReportFlg; //used for morning repost
//- - - Temperature variables and parameters
bool inCottageTemprFlg, inCottageTemprLowReportFlg;
bool inFrzrTemprFlg,inFrzrTemprHighReportFlg;
bool tstMsgSentFlg=false; // Ensures only one SMS is sent when test btn is pressed
int inCottageTemprVal=0;
int inCottageTemprMin=2000; // minimum value tempr. per 24hr.Init to high value
int inFrzTemprVal = 0; // Fereezer temperature variable
int inFrzTemprValMax = 0; // Fereezer temperature variable max value
int const temprValZero = 280; //Prm const: 280 = 0 dgrC  
int const inCottageTemprValueLimLow=315; //Prm:EQ. 300=ca 4 dgrC, defines alarm condition
int const inFrzTemprValueLimHigh = 200; //Upper limit for freezer temperature ~-15gdr

smsIdxQ cmdQ(pinSmsQueLED); // Construct queue of commands (cmd)

StrongDeBounce mainsPwr(2,cmd_MainPwrOut,cmd_MainPwrIn,&cmdQ); //<<< Construct mainsPwr monitor
//StrongDeBounce mainsPwr(360,cmd_MainPwrOut,cmd_MainPwrIn,&cmdQ); //Prm: Construct mainsPwr monitor
StrongDeBounce temprInsideCottage(4,cmd_TemprLow,cmd_TemprHigh,&cmdQ);	//<<<
//StrongDeBounce temprInsideCottage(90,cmd_TemprLow,cmd_TemprHigh,&cmdQ);	//Prm: 90ds=900sec=15min
StrongDeBounce temprFrz(4, cmd_TemprFrzHigh, cmd_TemprFrzLow, &cmdQ); //Prm: 90ds=900sec=15min
//StrongDeBounce temprFrz(90, cmd_TemprFrzHigh, cmd_TemprFrzLow, &cmdQ); //Prm: short for dbug
StrongDeBounce musTrap1(3, cmd_MusCaught, cmd_NoMusCaught, &cmdQ); //Prm: Construct mousetrap

// The following controls debug printing
long loopCt=0;			//<<< only for debugging
int loopCtPrtCt=1;		//<<< only for debugging
//=================================================
void setup()
{
	Serial.begin(115200);
	//<<<<<<<<<<<<<Version<>>>>>>>>>>>>>>>>>>>>>>>>>>
	Serial.println("Program version : HytMon_ODVm50");
	//<<<<<<<<<<<<<Version<>>>>>>>>>>>>>>>>>>>>>>>>>>>
	cnSmsMsgPerDay = 0; // Ensure effect of reset button
	gprsSerial.begin(19200);
	pinMode(pinPwrGprs,INPUT); // Turns GPRS module on/Off
	pinMode(pinTestSw,INPUT);  // Pushbotton on PCB
	//digitalWrite(pinPwrGprs,LOW);
	pinMode(pinPwrMains,INPUT); // input from Mains Sensor (Relay)
	//digitalWrite(pinPwrMains,LOW);
	pinMode(pinLedPwrDown,OUTPUT);
	pinMode(pinLedTeprLow,OUTPUT);
	pinMode(pinLedMusTrapped,OUTPUT);
	pinMode(loopLED,OUTPUT);
	// For test
	cmdQ.initiate(); //Initiate queue of commands
	long unsigned intervTmLU;
	testAndActivateGPRSmodule(ioBuffGPRS); // test if good response from GPRS
	// prgram could be stopped here? Why proceed if GPRS module is dead?
	gprsTime_ds= getGprsTimeIn_ds();
	printGprsTimer();
	prtCt("1. gprsTime_ds",gprsTime_ds);
	prtCt("synchClk_ds:",synchClk_ds);
	loopTime_ds=gprsTime_ds; // Synchronize in setup
			// later they will drift apart because of delay() etc.
	if (gprsTime_ds-synchClk_ds >0) // 
	{   // It is too late to synch. today, do it tomorrow
		nextSynchTime_ds=fullCycle_ds-gprsTime_ds+synchClk_ds+loopTime_ds;
	} else {
		// synching can be done later today
		nextSynchTime_ds=synchClk_ds;
	}
	nextMoRepoTime_ds=nextSynchTime_ds+10;
	//nextSynchTime_ds=loopTime_ds+12; //<<< §§§ test
	//bp
	prtCt("nextSynchTime_ds",nextSynchTime_ds);
	Serial.println(F("Setup finished"));
	//cmdQ.push(cmd_MoReport);
}
//=================================================
void loop()
{   //- - - Read test test button
	if(digitalRead(pinTestSw))
		{
		Serial.println("Test button pressed");
		if (!tstMsgSentFlg)
			{//send msg - Note: No debouncing done
				cmdQ.push(cmd_TestBotPushed);
				blink3LED();
				tstMsgSentFlg=true;
			}
		}else{
			tstMsgSentFlg=false;
		}
	//- - - Test inside Cottage temperature
	inCottageTemprVal=analogRead(pinTmprCottage);
	if (inCottageTemprVal<inCottageTemprMin) // record minimum temperature
	{	inCottageTemprMin=inCottageTemprVal; 	}
	// show sensor value in LED before debounce
	inCottageTemprFlg=(inCottageTemprVal<inCottageTemprValueLimLow);
	digitalWrite(pinLedTeprLow,inCottageTemprFlg); 
	Serial.print("inCottageTemprVal="); Serial.print(inCottageTemprVal);
	Serial.print(" inCottageTemprFlg=");Serial.println(inCottageTemprFlg);
	detectorDummyFlg=temprInsideCottage.detector(inCottageTemprFlg,loopTime_ds);// flags set by 'processCmds' fn
	//- - - Test Freezer temperature
	inFrzTemprVal = analogRead(pinTmprFrz); //pinTmprFrz
	if (inFrzTemprVal>inFrzTemprValMax) // record max freezer temperature
	{ //Save highest tempr value observed since last report, where reset
		inFrzTemprValMax=inFrzTemprVal;
	}
	// show sensor value in LED before debounce
	inFrzrTemprFlg = (inFrzTemprVal>inFrzTemprValueLimHigh);
	digitalWrite(pinLedTeprLow, (inFrzrTemprFlg || inCottageTemprFlg));
	Serial.print("inFrzTemprVal="); Serial.print(inFrzTemprVal); Serial.print(" frzTemprFlg="); Serial.println(inFrzrTemprFlg);
	detectorDummyFlg = temprFrz.detector(inFrzrTemprFlg, loopTime_ds); // Debounce and initiate event
	//--- Test mouse trap
	bool sensorValMusTrap=!digitalRead(pinMusTrp1); // Read mouse trap sensor
	digitalWrite(pinLedMusTrapped,sensorValMusTrap);   // show sensor value before debounce
	Serial.print("sensorValMusTrap=");Serial.println(sensorValMusTrap);
	musCaughtFlg=musTrap1.detector(sensorValMusTrap,loopTime_ds);		// Debounce musTrap input
	//--- Test mainsPwr
	bool sensorValMainsPwr= !digitalRead(pinPwrMains);	// Read mainsPwr input
	Serial.print("sensorValMainsPwr=");Serial.println(sensorValMainsPwr);
	digitalWrite(pinLedPwrDown,sensorValMainsPwr); // show sensor value before debounce
	mainsPwrDownFlg=mainsPwr.detector(sensorValMainsPwr,loopTime_ds);  // Debounce  mainsPwr input
			
if ((loopTime_ds-nextSynchTime_ds)>0 &&
	(nextSynchTime_ds+synchTolerance_ds-loopTime_ds>0) &&
	synchEnableFlg )
	{
	synchEnableFlg=false; moRepoEnableFlg=true;
    readGprsClk(hr,min,sec,NOPRNT_FLG); // reads correct present time from GPRS
	gprsTime_ds=getGprsTimeIn_ds();
	nextSynchTime_ds=fullCycle_ds-gprsTime_ds+synchClk_ds+loopTime_ds;
	//nextSynchTime_ds=loopTime_ds+21; //<<< §§ For testing resnd moRep in 210sec
	nextMoRepoTime_ds=moRepoClk_ds-gprsTime_ds+loopTime_ds;
	//nextMoRepoTime_ds=gprsTime_ds+synchTolerance_ds+5; //<<< dbg minimum
	prtCt("---Synching time; loopCt:",loopCt);
	prtCt("nextSynchTime_ds",nextSynchTime_ds);
	prtCt("nextMoRepoTime_ds=",nextMoRepoTime_ds);
	cnSmsMsgPerDay=0; // Reset to allow new attempts this day
	}
if ((loopTime_ds-nextMoRepoTime_ds)>0 && moRepoEnableFlg)
	{
	synchEnableFlg=true; moRepoEnableFlg=false;

	prtCt("---Morning report, loopCt:",loopCt);	
	Serial.println();
	cmdQ.push(cmd_MoReport);
	}

    processCmds();

if (loopTime_ds> 520000 || cmdQ.numberOfSMS>90)  //corresponds to approx 60 days
	{
	Serial.print(F("Monitoring ended, number of SMS sent="));
	Serial.println(cmdQ.numberOfSMS);
	toglePwr(); // as a marker
	while(1) { //loop forever
	blinkAll(100); // and Blink
	blinkAll(1000);
	}
	}
//delay(10000); // loopTime_ds == 10 sek
//delay(9463);
digitalWrite(loopLED,LOW);
delay(3000);
digitalWrite(loopLED,HIGH);
delay(3000);
digitalWrite(loopLED,LOW);
delay(9463-6000);
digitalWrite(loopLED,HIGH);
loopTime_ds++;

loopCt++;
if (loopCtPrtCt>1)
	{loopCtPrtCt=0;
	Serial.print("End loop,");
	prtSec();
	gprsTime_ds= getGprsTimeIn_ds();
	Serial.print("loopTime_ds="); Serial.print(loopTime_ds);
	Serial.print(" clc_ds=");
	Serial.print(gprsTime_ds);
	prtCt(" loopCt=",loopCt);
	 //Serial.println(); //<<<
	}
	loopCtPrtCt++;
	//gprsTime_ds= getGprsTimeIn_ds();
	//prtCt("EndLoop gprsTime_ds",gprsTime_ds);
	//Serial.print("loopTime_ds="); Serial.print(loopTime_ds);
Serial.println("- - - Loop slutt - - -");
//Serial.println();
} //loop
//=================================================
void prtCt(String hvor, long ct)
{	Serial.print(" ");
	Serial.print(hvor); Serial.print(": ");
	Serial.println((ct));
}
void prtSec()
{	Serial.print(" kmills()=");
	Serial.print((long)(millis()/1000));
	Serial.print(" ");
}
//=================================================
void blink3LED()
	{
	for (int j=0;j<3;j++)blinkAll(100);				
	}

void blinkAll(int waitTm)
{
			delay(waitTm);
			digitalWrite(pinLedPwrDown,HIGH);
			delay(waitTm);
			digitalWrite(pinLedPwrDown,LOW);
			delay(waitTm);
			digitalWrite(pinLedTeprLow,HIGH);
			delay(waitTm);
			digitalWrite(pinLedTeprLow,LOW);
			delay(waitTm);
			digitalWrite(pinLedMusTrapped,HIGH);
			delay(waitTm);
			digitalWrite(pinLedMusTrapped,LOW);
}

