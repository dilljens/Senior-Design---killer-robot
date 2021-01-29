/*
*  Licence: See "LICENSE.txt"
*
*  Author: James Jensen (Mechtro LLC)
*          tiagojen@gmail.com
*
*
*/

#include "IsElapsedTimeTest.h"

// note if you want to pass normal char arrays, you have to define
// a second copy of this function and replace helper with char
void etAssertTrue(const __FlashStringHelper* testName, bool b){
    if( b ){
        Serial.print(F("[pass] : "));
    }else{
        Serial.print(F("[! FAIL !] : "));
    }
    Serial.println(testName);
}

void testElapsedTime(){
    Serial.println(F("========= Test Elapsed Time Library =========="));

    etAssertTrue(F("Should PASS"), true);
    etAssertTrue(F("Should FAIL"), false);


    IsAdvancer advancer;

    IsElapsedMs32 emsA(&advancer);
    IsElapsedMs32 emsB(&advancer);
    IsElapsedMs32 emsC(&advancer);
    IsElapsedUs32 eusX(&advancer);

    emsA.elapsed = 0; // reset
    emsB.elapsed = 0;
    eusX.elapsed = 0;

    etAssertTrue(F("EmsA is 0"), emsA.elapsed==0);
    etAssertTrue(F("EmsB is 0"), emsB.elapsed==0);
    etAssertTrue(F("EusX is 0"), eusX.elapsed==0);

    Serial.println(F("Advance without nodes linked"));

    etAssertTrue(F("EmsA is 0"), emsA.elapsed==0);
    etAssertTrue(F("EmsB is 0"), emsB.elapsed==0);
    etAssertTrue(F("EusX is 0"), eusX.elapsed==0);

    Serial.println(F("Enable A&X, Advance 100"));
    emsA.enable(true);
    eusX.enable(true);
    advancer.advanceUs(100*1000);

    etAssertTrue(F("EmsA is 100"), emsA.elapsed==100);
    etAssertTrue(F("EmsB is 0"), emsB.elapsed==0);
    etAssertTrue(F("EusX is 100000"), eusX.elapsed==100*1000);

    Serial.println(F("Enable A, Advance to same 100 (shouldn't change)"));
    advancer.advanceUs(100*1000);

    etAssertTrue(F("EmsA is 100"), emsA.elapsed==100);
    etAssertTrue(F("EmsB is 0"), emsB.elapsed==0);
    etAssertTrue(F("EusX is 100000"), eusX.elapsed==100*1000);

    Serial.println(F("Disable A&X, Enable B, Advance to 200"));
    emsA.enable(false);
    etAssertTrue(F("EmsA is disabled"), !emsA.isEnabled());
    eusX.enable(false);
    etAssertTrue(F("EusX is disabled"), !eusX.isEnabled());
    emsB.enable(true);
    etAssertTrue(F("EmsB is enabled"), emsB.isEnabled());
    advancer.advanceUs(200*1000);

    etAssertTrue(F("EmsA is 100"), emsA.elapsed==100);
    etAssertTrue(F("EmsB is 100"), emsB.elapsed==100);

    Serial.println(F("Disable All, Advance to max"));
    emsA.enable(false);
    emsB.enable(false);
    eusX.enable(false);
    advancer.advanceUs(UINT32_MAX);

    etAssertTrue(F("EmsA is 100"), emsA.elapsed==100);
    etAssertTrue(F("EmsB is 100"), emsB.elapsed==100);
    etAssertTrue(F("EusX is 100000"), eusX.elapsed==100*1000);

    Serial.println(F("Enable A&X, advance to 0 (i.e. rollover of 1us shouldn't step ms)"));
    emsA.enable(true);
    eusX.enable(true);
    advancer.advanceUs(0);
    Serial.print("XX emsA ");
    Serial.println(emsA.elapsed);
    Serial.print("XX eusX ");
    Serial.println(eusX.elapsed);
    etAssertTrue(F("EmsA is 100"), emsA.elapsed==100);
    etAssertTrue(F("EmsB is 100"), emsB.elapsed==100);
    etAssertTrue(F("EusX is 100001"), eusX.elapsed==100001);

    // At first reaching Max, remainder should be 295 at MAX,
    // so 705 more to go untill ms incr
    // so at 0, 704 to ms incr

    Serial.println(F("Advance to just before ms incr"));
    advancer.advanceUs(703);
    etAssertTrue(F("EmsA is 100"), emsA.elapsed==100);
    etAssertTrue(F("EmsB is 100"), emsB.elapsed==100);
    etAssertTrue(F("EusX is 100704"), eusX.elapsed==100704);

    Serial.println(F("Advance to expected ms incr"));
    advancer.advanceUs(704); // one more us
    etAssertTrue(F("EmsA is 101"), emsA.elapsed==101);
    etAssertTrue(F("EmsB is 100"), emsB.elapsed==100);
    etAssertTrue(F("EusX is 100705"), eusX.elapsed==100705);

    Serial.println(F("Disable all, reset all to zero, advance to zero"));
    emsA.enable(false);
    emsB.enable(false);
    emsC.enable(false);
    eusX.enable(false);
    emsA.elapsed = 0;
    emsB.elapsed = 0;
    emsC.elapsed = 0;
    eusX.elapsed = 0;
    advancer.advanceUs(0);
    etAssertTrue(F("EmsA is 0"), emsA.elapsed==0);
    etAssertTrue(F("EmsB is 0"), emsB.elapsed==0);
    etAssertTrue(F("EmsC is 0"), emsC.elapsed==0);

    // test the linking to make sure that pulling middle node from list works as expected
    Serial.println(F("Enable ABC then disable B to pull from middle of list"));
    emsA.enable(true);
    emsB.enable(true);
    emsC.enable(true);
    emsB.enable(false);
    advancer.advanceUs(10*1000);

    etAssertTrue(F("EmsA is 10"), emsA.elapsed==10);
    etAssertTrue(F("EmsB is 0"), emsB.elapsed==0);
    etAssertTrue(F("EmsC is 10"), emsC.elapsed==10);

    Serial.println(F("Enable B, advance again"));
    emsB.enable(true);
    advancer.advanceUs(20*1000);
    etAssertTrue(F("EmsA is 20"), emsA.elapsed==20);
    etAssertTrue(F("EmsB is 10"), emsB.elapsed==10);
    etAssertTrue(F("EmsC is 20"), emsC.elapsed==20);

    Serial.println(F("========= END =========="));

}
