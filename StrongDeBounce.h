/* 
* StrongDeBounce.h
*
* Created: 05.01.2015 07:11:58
* Author: admin
*/


#ifndef __STRONGDEBOUNCE_H__
#define __STRONGDEBOUNCE_H__
#include "Arduino.h"
#include "smsIdxQ.h"
//Definition of status values
#define Noff 1 //  Normal Off
#define NtoT 2 // Noff->Triggered
#define Ton 3  // Triggered On
#define TtoN 4  // Ton->Noff
class StrongDeBounce
// triggered

{
//variables
//bool ttt;
//int tt1;
public:
protected:
private:
bool readInput, fnOutput;
int smsIdxAlarmON,smsIdxAlarmOff;
int sts, oldSts;
int tmeout_ds, tmeMax_ds;
smsIdxQ *ptrSmsQ;
//functions
public:
	StrongDeBounce(long ptmeMax,int psmsIdxAlarmON,int psmsIdxAlarmOff
	,smsIdxQ *fPtrSmsQ
	);  //constructor
	bool detector(bool sensorRead, int ds);
protected:
private:
}; //StrongDeBounce

#endif //__STRONGDEBOUNCE_H__
