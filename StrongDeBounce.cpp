/*
* StrongDeBounce.cpp
*
* Created: 05.01.2015 07:11:57
* Author: admin
*/
#include "StrongDeBounce.h"

// default constructor
StrongDeBounce::StrongDeBounce(long ptmeMax,
	int psmsIdxAlarmON, int psmsIdxAlarmOff
	,smsIdxQ *fPtrSmsQ
	)
{
	tmeMax_ds=ptmeMax;
	smsIdxAlarmON=psmsIdxAlarmON;
	smsIdxAlarmOff=psmsIdxAlarmOff;
	ptrSmsQ=fPtrSmsQ;
	sts=Noff;
	oldSts=sts;
	} //StrongDeBounce

bool StrongDeBounce::detector(bool sensorInput,int ds)
{	
	switch (sts)
	{
	case Noff:
		if(sensorInput)  // Input triggered
		{
			tmeout_ds=ds+tmeMax_ds;
			sts=NtoT;		// Start timing the debounce
			fnOutput=false;
		}
		break;
	case NtoT:				
			// This is a transition state we are waiting
			// for input to become stable, if not return previous
		if (!sensorInput)
		{
			sts=Noff;
			fnOutput=false;
		} else{
			if((ds-tmeout_ds)>0) // state is stable
			{
				sts=Ton;
				fnOutput=true;
				ptrSmsQ->push(smsIdxAlarmON); //<<<<<<<<
				Serial.println("StrongBounce - alarmON");
				//>>> put an sms in queue smsIda . . .
				break;
			}
		}
		break;
		
	case Ton:
		if (!sensorInput) // Input has changed o
		{
			tmeout_ds=ds+tmeMax_ds;
			sts=TtoN;
			fnOutput=true;
		}
		break;
		
	case TtoN:
		if (sensorInput) // Input was not stable
		{
			sts=Ton;	// go back to previous state
			fnOutput=true;
	}else
		{				// Test that input is stable off
			if ((ds-tmeout_ds)>0)
			{
				sts=Noff;
				fnOutput=false;
				
				ptrSmsQ->push(smsIdxAlarmOff); //<<<<<<<<
				Serial.println("StrongBounce - alarmOFF");
				//<< send smsidx =0
			}
		}
		break;
	}
	if (sts!=oldSts)
	{
		//Serial.println();
		Serial.print("In Debounce, ds="); Serial.print(ds);
		Serial.print(" "); Serial.print("bounce sts=");
		Serial.println(sts);
		oldSts=sts;
	}
return fnOutput;
}

