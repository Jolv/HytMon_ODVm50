/* 
* smsIdxQ.cpp
*
* Created: 26.01.14 20:16:51
* Author: Jorolv
*
* Usage sequence:
* #include "smsIdxQ.h"
* Before setup(): Declare an instance:'xQ'
*                 Remember monitoring LED 
* inside setup(): 'xQ'.initiate()
* in loop(): 'xQ'.push(n), 'xQ'.pop(), etc.
*/

#include "smsIdxQ.h"

smsIdxQ::smsIdxQ(int pmonitQueLedPin)
{
monitQueLedPin=pmonitQueLedPin;
qlen=0; 
numberOfSMS=0; }

void smsIdxQ::initiate()
{
pinMode(monitQueLedPin,OUTPUT);	
digitalWrite(monitQueLedPin,LOW);
numberOfSMS = 0;
delay(100);
}

boolean smsIdxQ::isempty(){
	if (qlen<=0){
	digitalWrite(monitQueLedPin,LOW);	 
	return true;
	}else
	return false;
} //bool empty


void smsIdxQ::push(int inStg)
{
	numberOfSMS++; // count total number of SMS generated
	digitalWrite(monitQueLedPin,HIGH);
	
	if (qlen<2)	{
		qlen+=1;
		qSms[qlen]=inStg;
	}
} //push

int smsIdxQ::pop(){
	qSms[0]=qSms[1];
	qSms[1]=qSms[2];
	qSms[2]=qSms[3];
	if (qlen>0) {
		qlen-=1;
	return qSms[0];
	}
	else {
		digitalWrite(monitQueLedPin,LOW);
		return 0;
	}
}// pop
int smsIdxQ::qlength(){
	return qlen;
} //bool empty
