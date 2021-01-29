/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  This example code is in the public domain.
 */
#include "Arduino.h"
#include "IsParse.h"
// Pin 13 has an LED connected on most Arduino boards.
// Pin 11 has the LED on Teensy 2.0
// Pin 6  has the LED on Teensy++ 2.0
// Pin 13 has the LED on Teensy 3.0
// give it a name:
int led = 13;
IntervalTimer myTimer;
uint32_t isrCnt = 0;

void stepperIsr(){
    isrCnt ++;
}
// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
  Serial.begin(9600);
  myTimer.begin(stepperIsr, 50); //# is period of Isr in us
}

uint32_t lastTimestamp = 0;
uint32_t usRemainder = 0;

uint64_t tmrMsA = 0;
uint64_t tmrUsB = 0;
uint64_t tmrMsBlink = 0;
uint8_t inputBuf[256];
uint32_t inputCnt;
uint32_t blinkOnTime = 1000;

// the loop routine runs over and over again forever:
void loop() {
    // calculate time delta
    uint32_t timestamp = micros();
    uint32_t deltaUs = timestamp - lastTimestamp;
    lastTimestamp = timestamp;
    usRemainder += deltaUs;

    uint32_t deltaMs = usRemainder/1000;
    usRemainder = usRemainder%1000;

    // update timers
    tmrMsA += deltaMs;
    tmrUsB += deltaUs;
    tmrMsBlink += deltaMs;
    
    // ---------------------------------
    if(tmrMsA>2000){ //use -= if you want more accurate timing
        tmrMsA -= 2000;
        Serial.print("isrCnt = ");
        Serial.println(isrCnt);
        
    }
    if(tmrMsBlink>blinkOnTime){
        tmrMsBlink = 0;
        uint8_t ledState = digitalReadFast(13);
        if(ledState){
            digitalWriteFast(13, 0);
            // Serial.println("off");
        }else{
            digitalWriteFast(13, 1);            
            // Serial.println("on");
        }
    }

    // ---------------------------------
    while(Serial.available()){
        char c = Serial.read();
        // Serial.print("got a ");
        // Serial.println((uint8_t)c);

        if(c==10||inputCnt>=254){
            inputBuf[inputCnt] = 0;
            Serial.print("got '");
            Serial.print((char*)inputBuf);
            Serial.println("'");
            inputCnt = 0;

            // while loop for parsing commands
            while(true){ //context to break out of
                // test function
                bool isOk =  1;
                uint16_t caret = 0;
                matchWord(&isOk, (char*)inputBuf, &caret, "led");
                skipSpaces(&isOk, (char*)inputBuf, &caret);
                uint8_t ledArg = 0;
                parseUint8(&isOk, (char*)inputBuf, &caret, &ledArg);
                if(isOk){
                    Serial.print("xxx ");
                    Serial.println(ledArg);
                    break;
                }

                // changes blinking frequency of led
                isOk = 1;
                caret = 0;
                matchWord(&isOk, (char*)inputBuf, &caret, "blinkOn");
                skipSpaces(&isOk, (char*)inputBuf, &caret);
                parseUint32(&isOk, (char*)inputBuf, &caret, &blinkOnTime);
                if(isOk){
                    Serial.print("changed blink time to ");
                    Serial.println(blinkOnTime);
                    break;
                }
                
                // prints menu
                isOk = 1;
                caret = 0;
                matchWord(&isOk, (char*)inputBuf, &caret, "?");
                if(isOk){
                    Serial.println("-------------------");
                    Serial.println("Menu:");
                    Serial.println("    ? - print this menu");
                    Serial.println("    led [uint8] - print xxx + ");
                    Serial.println("    blinkOn [uint32] - ms btw blink time toggling");
                    Serial.println("    ");
                    Serial.println("-------------------");

                    break;
                }

                //add a set velocity and run at velocity cmds
                break;
            } //end while
        } else{
            inputBuf[inputCnt] = c;
            inputCnt ++;
        }


    }
} // end loop