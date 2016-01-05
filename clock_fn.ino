

//==================================================
int readFromGPRS(String &pStrg,int cnMaxChar){
	// reads a response of maximum cnMaxChar from GPRS
	// returns the response in pStrg, and number of characters (n)
	// if nothing received in 5000 ms, reading is stopped n=0
	// if n=cnMaxChar is received the input buffer is flushed
	//Serial.println("goodAtRep ENTERED");
	int cnTimeOut=5; // Count time out loops
	int cnChar=0; // Count char read
	pStrg=""; // Empty buffer string
	while(cnTimeOut>0) //Timeout loop
	{
		while(gprsSerial.available() && cnChar<cnMaxChar )
		{
			car=gprsSerial.read();
					//dg Serial.print(car,HEX); Serial.print(" ");
			cnChar +=1;
			pStrg.concat(car);
			delay(1);
		}//while(gprsSerial
					// Serial.println();
					// Serial.print("    After while cnCar= ");
					// Serial.print(cnChar);
					// Serial.print("   pStrg= ");
					// Serial.println(pStrg);		
		if(cnChar==cnMaxChar)gprsSerial.flush(); // make sure no garbage is left in input buffer
		if(cnChar>0)break;
		cnTimeOut-=1;
					// Serial.print("cnTimeOut: ");Serial.println(cnTimeOut);
		delay(1000);
	}//while(cnTimeOut>0)
					// Serial.print("Return from Fn ");
					// Serial.print("cnCar= ");
					// Serial.println(cnChar);
	return cnChar;
}// readFromGPRS(String
//===================================================================
int get2ClockDigits(String& strg,int j)
{
	int n=strg.charAt(j)-'0'; // Search for zero char 
	n=n*10+strg.charAt(j+1)-'0';
	if(n>=0 && n<60) return n;
	return -1;
}//get2ClockDigits

//===============================================================
bool testAndActivateGPRSmodule(String &buffGPRS)
{	// AT cmd sent and return of OK tested
	// if OK not received GPRS power-ON
	// is toggeled for 10 sec
	bool OKflg=false;
	int cnWhileLoop=5;
	do //do while loop
	{
		Serial.println("testAndActivateGPRSmodule: Enter new AT-command");
		gprsSerial.println("AT");
		delay(300);
		int AtRespLenth=readFromGPRS(buffGPRS,50);
		Serial.print(" char, ");	Serial.print(AtRespLenth);
		Serial.print(" tmpOr  ");
		//dg Serial.println(buffGPRS);
		if (AtRespLenth>0)
		{
						//d Serial.print(" AT-string received: ");
						//d Serial.print(AtRespLenth); Serial.print("chars, ");
						//d Serial.println(buffGPRS);
						//d Serial.println("- - - - - ");
			int ioBuffIdx=buffGPRS.lastIndexOf('O');
						//d Serial.print("jn=");Serial.print(ioBuffIdx);
			char jm=buffGPRS.charAt(ioBuffIdx+1);
						//d Serial.println("jm=");Serial.print(jm);
			if(ioBuffIdx>0 && buffGPRS.charAt(ioBuffIdx+1)=='K')
			{
				Serial.println(F(" OK received from GPRS"));
				OKflg=true;					
			}
			delay(2000);
				
		} else {
			// Length of response == 0
			// No response from GPRS shield, try toggling power
			Serial.println(" No AT-string received: ");
			Serial.println(buffGPRS);
			Serial.println("- - - - - ");
			toglePwr();
			cnWhileLoop -=1;
			delay(3000);
		}
	}while(!OKflg && cnWhileLoop>0);
	return OKflg;
}
//==============================================================
int getGprsTimeIn_ds()
{
readGprsClk(hr,min,sec,false); // reads correct present time from GPRS
					//fakeGprsClk(hr,min,sec,PRNT_FLG); // reads faked present time from GPRS
					// hr=7, min=2, seconds=40  easy to make, useful for debug
int rett= ( ( (int)(hr*60) + (int)(min) )*6  + (int)(sec/10) );
return rett;
}
//==============================================================
void readGprsClk(byte &hr,byte &min,byte &seconds,bool prtTime)
	{
	//Serial.println("Enter read CLOCK command");
	gprsSerial.println("AT+CCLK?"); // AT command to read clock form GPRS
	delay(300);
	int clkStrLen=readFromGPRS(ioBuffGPRS,60);
	//Serial.print(" char, ");	Serial.print(clkStrLen);
	//Serial.print(" tmpOr  ");
	//dg Serial.println(ioBuffGPRS); // 17/1
	if (clkStrLen>0)
	{
		//Serial.print(" AT-string received: ");
		//Serial.print(clkStrLen); Serial.print("chars, ");
		//dg Serial.println(ioBuffGPRS);
        // find index to comma before string of hr, min and seconds
		int idxIoBuff=ioBuffGPRS.indexOf(','); 
						
		//Serial.print("--idxIoBuff=");Serial.print(idxIoBuff);
		//char jm=ioBuffGPRS.charAt(idxIoBuff+1);
		//Serial.println("--jm=");Serial.println(jm);
				
		hr=get2ClockDigits(ioBuffGPRS,(idxIoBuff+1));
		min=get2ClockDigits(ioBuffGPRS,(idxIoBuff+4));
		seconds=get2ClockDigits(ioBuffGPRS,(idxIoBuff+7));
		
		if (prtTime) //used to print time when 
		{
			//Serial.println();
			Serial.print(F("readGprsClk, hr:"));Serial.print(hr);		
			Serial.print(F(" min:"));Serial.print(min);
			Serial.print(F(" sec:"));Serial.println(seconds);
			//Serial.println(F("- - - - - "));
		}
				
	}
	}

//===============================================================
void toglePwr()
{
digitalWrite(pinPwrGprs,LOW);
delay(1000);
Serial.println("Set GPRS pinPWR to HIGH");
digitalWrite(pinPwrGprs,HIGH);
delay(2000);
Serial.println("Set GPRS pinPWR to LOW");
digitalWrite(pinPwrGprs,LOW);
delay(3000);
}
//=================================================================
void toglePwr1()
{
	Serial.println("Power Toggeled");
}
//=================================================================
void printGprsTimer()
{
			//Serial.println();
			Serial.print(F("readGprsClk, hr:"));Serial.print(hr);		
			Serial.print(F(" min:"));Serial.print(min);
			Serial.print(F(" sec:"));Serial.println(sec);
			//Serial.println(F("- - - - - "));
		}
