/*
--- Leakduino ---
https://github.com/cicero44bc/leakduino

Arduino Based Leak Detector and Auto Shut-off, built using;

* 5V Arduino
* 2 Bank Relay - Example - http://www.ebay.co.uk/itm/5V-2-CHANNEL-BOARD-RELAY-MODULE-SHIELD-FOR-ARDUINO-ARM-PIC-AVR-DSP-ELECTRONIC-UK-/271450520198
* Soil Hygrometer / Moisture meter - Example - http://www.ebay.co.uk/itm/Soil-Hygrometer-Humidity-Detection-Module-Moisture-Testing-Sensor-for-Arduino-PI-/350961143799

Description:

* Allows a PDU to be shut off when water is detected in any area the probes are located.
* Shuts power off and fires alarm whenever probes are wet.
* 3 beeps indicate when system is primed.

Operation / Notes :

* Relays are Normally Open with no power - this allows for Arduino Failure and means when system is first plugged in, no circuits have power.
* The switch at the top of the schematic needs to be held closed for 3 seconds to prime the system / turn the relays & sockets on.
* Two relays are used in series to allow for a maximum of 1 relay to fail in the on position.
* 4 relays can be used; 2 sets of 2 relays in series, in parallel (so two of the two relays described above in parallel) to
  allow for a maximum of one relay to fail in the off position.

Uses the Bounce2 Library for button debouncing - https://github.com/thomasfredericks/Bounce2

cicero44bc 15/07/2015
*/
#include "Bounce2.h"

// Bounce Related
Bounce setButtonDebouncer = Bounce();
int buttonValue;
unsigned long buttonDownStartMillis = 1;

#define hydroSensor0 A0
#define buttonPin 6
#define relay0 7
#define relay1 8
#define speakerPin 9
boolean systemIsSet = false;

void setup()
{

	Serial.begin(115200);

  // Setup 'Set' button debouncer
	pinMode(buttonPin,INPUT); // Setup the button
	pinMode(hydroSensor0,INPUT); // Setup the button
	pinMode(relay0,OUTPUT);
	pinMode(relay1,OUTPUT);
	digitalWrite(relay0,HIGH);
	digitalWrite(relay1,HIGH);
	pinMode(speakerPin,OUTPUT);
	digitalWrite(buttonPin,HIGH);  // Activate internal pull-up
	// digitalWrite(hydroSensor0,HIGH);  // Activate internal pull-up
	setButtonDebouncer.attach(buttonPin);
	setButtonDebouncer.interval(5);
}

void loop()
{
	// Check hydroSensor/s
	Serial.println("hydroSensor0 value: " + String(analogRead(hydroSensor0)));
	Serial.println("systemIsSet value: " + String(systemIsSet));
	//delay(500);
	if(analogRead(hydroSensor0)>30) // Adjust sensitivity / trigger value here.
	{
	// Check System Status and act accordingly;
	switch (systemIsSet)
	{
		case false: 
			systemIsNotSetLoop();
			break;
		case true:
			systemIsSetLoop();
			break;
		default:;
	}
	}
	else
	{
		systemAlarm();
	}
}

void systemIsSetLoop()
{
	Serial.println("System is Set");
}

void systemIsNotSetLoop()
{
	boolean buttonStateChanged = setButtonDebouncer.update();
	buttonValue = setButtonDebouncer.read();
	if(buttonStateChanged)
	{
		if(buttonValue==HIGH){
			//button is not being pushed
			//Serial.println("Button not pushed");
			//Serial.println("buttonValue HIGH" + String(buttonValue));
			buttonDownStartMillis = 0;
		} else
		{
			//button is being pushed
			//Serial.println("Button pushed");
			//Serial.println("buttonValue LOW" + String(buttonValue));
		}
	}
	if(!buttonStateChanged && buttonValue==LOW)
	{
		processButtonDown();
	}
}

void processButtonDown()
{

	if(buttonDownStartMillis==0)
	{
		buttonDownStartMillis = millis();
		Serial.println("This should be returning. Here's millis(): " + String(millis()));
		return;
	}

	if(millis()>3000 && buttonDownStartMillis<(millis()-3000))
	{
		Serial.println("if(millis()>3000 && buttonDownStartMillis<(millis()-3000)) hit");
		setSystem();
		buttonDownStartMillis = -1; //Prevent beeping on next loop while button is still held down.
	}

}

void setSystem()
{
	digitalWrite(relay0,LOW);
	digitalWrite(relay1,LOW);
	systemIsSet = true;
	makeIsSetBeep();
}

void systemAlarm()
{
	digitalWrite(relay0,HIGH);
	digitalWrite(relay1,HIGH);
	systemIsSet = false;
	makeAlarmBeep();
}

void makeIsSetBeep()
{
	  beep(speakerPin,3520,500);	//A
	  delay(200);
	  beep(speakerPin,3520,500);	//A
      delay(200);
	  beep(speakerPin,3520,500);	//A
}

void makeAlarmBeep()
{
	// SOS

	beep(speakerPin,3520,150);	//A
	delay(200);
	beep(speakerPin,3520,150);	//A
    delay(200);
	beep(speakerPin,3520,150);	//A
	delay(400);
	beep(speakerPin,3520,600);	//A
	delay(200);
	beep(speakerPin,3520,600);	//A
    delay(200);
	beep(speakerPin,3520,600);	//A
	delay(400);
	beep(speakerPin,3520,150);	//A
	delay(200);
	beep(speakerPin,3520,150);	//A
    delay(200);
	beep(speakerPin,3520,200);	//A
	delay(400);
}

void beep(int speakerDPin, int frequencyInHertz, long timeInMilliseconds)     // Sub for PWM?
{	 
          int x;	 
          long delayAmount = (long)(1000000/frequencyInHertz);
          long loopTime = (long)((timeInMilliseconds*1000)/(delayAmount*2));
          for (x=0;x<loopTime;x++)	 
          {	 
              digitalWrite(speakerDPin,HIGH);
              delayMicroseconds(delayAmount);
              digitalWrite(speakerDPin,LOW);
              delayMicroseconds(delayAmount);
          }	 
}
