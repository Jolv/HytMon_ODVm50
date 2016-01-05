/* 
* SmsDefFile.h
*
* Created: 11.01.2015 22:24:06
* Author: admin
*/

#ifndef SMS_DEF_FILE_H_
#define SMS_DEF_FILE_H_

#define cmd_Error 0
#define cmd_Error_Sms F("Hyttun: Software error - M0 ")

#define cmd_MoReport 1
#define cmd_MoReportSMS_Sms F("Hyttun: Morgen rapport: ")

#define cmd_TestBotPushed 2
#define cmd_TestBotPushed_Sms F("Hyttun: TestBotPushed rapport: ")

// Mains power SMS
#define cmd_MainPwrOut 13
#define cmd_MainPwrOut_Sms F("Hyttun:Strom_utfall - M13 ")
#define cmd_MainPwrIn 14
#define cmd_MainPwrIn_Sms F("Hyttun:Strom_tilbake - M14 ")
// The cmd number is set > 100 in no SMS shall be sent
// Mouse trap SMS  NOT in use Info in morning Report (moReport)
#define cmd_MusCaught 111
#define cmd_MusCaught_Sms F("Hyttun: Mus i fella! - M111 ")
#define cmd_NoMusCaught 112  // not in use
#define cmd_NoMusCaught_Sms F("Hyttun: NA - M112 ")

// Temperature SMS  NOT in use Info in morning Report
#define cmd_TemprLow 115
#define cmd_TemprLow_Sms F("Hyttun:Temperatur - msg5 ")
#define cmd_TemprHigh 116
#define cmd_TemprHigh_Sms F("Hyttun:Temperatur - msg5 ")

// Temperature Freezer SMS  NOT in use Info in morning Report
#define cmd_TemprFrzHigh 117
#define cmd_TemprLow_Sms F("Hyttun:Temperatur - msg5 ")
#define cmd_TemprFrzLow 118
#define cmd_TemprHigh_Sms F("Hyttun:Temperatur - msg5 ")

#endif /* SMS_DEF_FILE_H_ */