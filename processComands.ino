void processCmds()
{
// It is probably a rollover timing problem here
// since long unsigned is not used (reason for mousetrap problem??)
bool monitFlag=false;
int cmdNo;  

if(!cmdQ.isempty())
	{  // an cmd to be processed in queue
	cmdNo = cmdQ.pop(); // Retrieve a cmd from queue
	Serial.print("cmdNo=");Serial.println(cmdNo);
	if (cmdNo>cmd_Error && cmdNo<100) //Cmd range for sending SMS
		{	
			//prevent trying to send more than 8 sms pr day
			if(cnSmsMsgPerDay<cnSmsMsgPerDayLim)
			{
			cnSmsMsgPerDay++; // Always incr. counter even if no SMS sent
			// to prevent network blocking in an error situation.
			if (testAndActivateGPRSmodule(ioBuffGPRS))
				{ // GPRS module is active and connected to network
					sendSmsAndSetFlgs(cmdNo);
				}
				// if not GPRS is active the SMS will bee lost.
				// SMS could be put in new "lostSMSq" queue for another attempt
				// to send, but control would be complex, wait and see if necessary
			}//f(cnSmsMsgPerDay
			
		} // processing of cmds sending Sms finished
		else 
		{	// The cmd-number imply no SmS sending
			// instead flags must be set for correct signaling
			// in MoReport 	
			Serial.println(F("Cmds with no SMS processed"));
			// process command from tempr reading.
			if (cmdNo==cmd_TemprLow)
			{ // Cottage is too cold, set flag for moMsg
			inCottageTemprLowReportFlg=true; 
			Serial.println("Cottage is too cold, set flag for moMsg");
			} 
			if(cmdNo==cmd_TemprHigh)
			{ // if temprLowFlag is set high, is will only be
			  // reset when mo msg has been sent.
			  Serial.println("Cottage is warm again");
			  //Reset in resetReportedValues()
			  // temprLowReportFlg=false; //
			 }
			// --- process command from Freezer reading.
			if (cmdNo == cmd_TemprFrzHigh)
			{ // if tempr Freezer is set High, is will only be
				// reset when mornin/testbot msg has been sent.
				Serial.println("Freezer is to warm ");
				inFrzrTemprHighReportFlg = true; //
			}
			if (cmdNo == cmd_TemprFrzLow)
			{ // if tempr Freezer is set Low, is will only be
				// reset when mornin/testbot msg has been sent.
				Serial.println("Freezer is cold enough ");
				//Reset in resetReportedValues()
				//inFrzrTemprHighReportFlg = false; //
			}
			// --- process command from mouse Trap reading.
			if (cmdNo==cmd_MusCaught) 
				{
				  Serial.println(F("Hyttun:Mouse Caught "));
				  musCaughtReportFlg=true;
				 }
			if (cmdNo==cmd_NoMusCaught) //cmd_MainPwrOut_Sms
				{
				 Serial.println(F("Hyttun:MouseTrap Disarmed"));
				 //Reset in resetReportedValues()
				 //musCaughtReportFlg=false;
				}
		}// Cmd range for SMS
	}// if(!cmdQ.isempty
}// end fn
//===================================================================================
void debug_processCmds()
{
bool monitFlag=false;
bool activelySendingSmsFlg=false; // used to keep execute in while loop until all sms queue is empty
long int sendSmsTmeOut;
long int tmeLoopEntry=millis();
int sendSmsTxtIdx;  
int sendSmsSts=0;
if(!cmdQ.isempty())
	{ // an SMS i  queue
	sendSmsTxtIdx = cmdQ.pop();
	if (sendSmsTxtIdx>cmd_Error) //It is not a dummy SMS
		{
		Serial.print("Send SMS #");Serial.println(sendSmsTxtIdx);

		}else{ //Indx==0 it is a dummy
		Serial.println("Dummy SMS requested");
		}//if (cmdNo>cmd_Error
	}

}
//===================================================================================