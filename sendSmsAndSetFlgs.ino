void sendSmsAndSetFlgs(int cmdNo)
{	//For debugging
    //gprsSerial.flush();
	//int buffSize=gprsSerial.available();
	//Serial.print("buffSize="); Serial.println(buffSize);
	// consider to restart GPRS-module 
	// it might be 24 hr since it was used
	delay(300);
	gprsSerial.print("AT+CMGF=1\r");    //Because we want to send the SMS in text mode
	delay(300);
	gprsSerial.println("AT + CMGS = \"+4790020237\"");//send sms message, remember a country code
	                                 // the \ is 'escape' character
	delay(300);
	//Print variable txt here, depending on smsIndex : 
	if (cmdNo==cmd_MoReport)
		{ 
		//Serial.println(F("MoRep skal sendes!"));
		gprsSerial.println(cmd_MoReportSMS_Sms);//the content of the message
		delay(100);
		sendSmsReportContent();
		resetReportedValues();
		//delay(2000);
		}// if(...cmd_MoReport

	if (cmdNo == cmd_TestBotPushed)
	{
		//Serial.println(F("TestBotPushed SMS skal sendes!"));
		gprsSerial.println(cmd_TestBotPushed_Sms);//the content of the message
		delay(100);
		sendSmsReportContent();
		//resetReportedValues(); // To see how values develope
		//delay(2000);
	}// if(...cmd_TestBotPushed

	if (cmdNo==cmd_MainPwrOut) //cmd_MainPwrOut_Sms
		{
		mainsPwrDownReportFlg=true; // For reporting in MorningReport
		Serial.println(F("Hyttun:Pwr_utfall--M13 "));
		gprsSerial.println(cmd_MainPwrOut_Sms);//the content of the message
		}
	if (cmdNo==cmd_MainPwrIn)
		{
		//mainsPwrDownReportFlg=false; // reset in morning report
		Serial.println(F("Hyttun:Pwr_tilbake--M14 "));
		gprsSerial.println(cmd_MainPwrIn_Sms);//the content of the message
		}
	delay(300);
	gprsSerial.println((char)26);//the ASCII code of the ctrl+z is 26
	delay(2000);
	//buffSize=gprsSerial.available();
	//Serial.print("buffSize="); Serial.println(buffSize);
	//delay(300);
	gprsSerial.flush();
} //sendSmsAndSetFlgs(int cmdNo)

void sendSmsReportContent()
{
	bool tempBit;
	gprsSerial.print(F(" PwrAlm:")); // 8 char
	gprsSerial.print(mainsPwrDownReportFlg); // 1 char
	gprsSerial.print(F(" ")); gprsSerial.print(mainsPwrDownFlg); // 2 char 
	tempBit = mainsPwrDownFlg || mainsPwrDownReportFlg;
	gprsSerial.print(F(" ")); gprsSerial.println(tempBit); // 2 char 

	gprsSerial.print(F(" TmprCotAlm:")); gprsSerial.println(inCottageTemprLowReportFlg); // 13 char
	gprsSerial.print(F(" TmpCot:")); gprsSerial.print(inCottageTemprVal - temprValZero); // 9-11 char
	gprsSerial.print(F("mn")); gprsSerial.println(inCottageTemprMin - temprValZero); // 5 char inCottageTemprMin-temprValZero

	gprsSerial.print(F(" FrzAlm:")); gprsSerial.println(inFrzrTemprHighReportFlg);   //  9 char
	gprsSerial.print(F(" TmprFrz:")); gprsSerial.print(inFrzTemprVal - temprValZero); //frzTemprVaø-temprValZero // 11char
	gprsSerial.print(F("mx")); gprsSerial.println(inFrzTemprValMax - temprValZero); //frzTemprVaø-temprValZero // 5 char

	gprsSerial.print(F(" Mus:")); gprsSerial.print(musCaughtReportFlg); // 6 char
	gprsSerial.print(F(" ")); gprsSerial.print(musCaughtFlg); // 2 char 
	tempBit = musCaughtFlg || musCaughtReportFlg;
	gprsSerial.print(F(" ")); gprsSerial.println(tempBit); // 2 char 
	// Total ca 78
}

void resetReportedValues()
{
	inCottageTemprLowReportFlg = false; //
	inCottageTemprMin = 2000; //Reset minimum value of temperature
	inFrzrTemprHighReportFlg = false; //
	inFrzTemprValMax = -100; //Reset max. temperature freezer
	mainsPwrDownReportFlg = false;
	musCaughtReportFlg = false;
}