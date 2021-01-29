/* Stepper.cpp
 *
 * Licence: See "LICENSE.txt"
 *
 * Author: James Jensen (Mechtro LLC)
 *         tiagojen@gmail.com
 *
 */

#include "Stepper.h"

// !! for this testbed, direction is separate steps, so profiles
// should only declare forward motion, to move back, flip the rev
// bit of the appropriate driver before running the profile.
// if second pin == first, big not supported

Stepper stepper1(SM1_STEP, SM1_DIR);

uint32_t isrCnt;
uint32_t maxIsrDur;
IntervalTimer intervalTimer;

// called from ISR
void intervalTimerFun(){
    uint32_t startUs = micros();
    isrCnt ++;

    // don't use multiple motors here without refactoring Stepper to
    // do the pulse delay here, so don't do multiple
    stepper1._processFromIsr();

    uint32_t endUs = micros();
    if( endUs >= startUs ){ // not rollover
        uint32_t delta = endUs - startUs;
        maxIsrDur = max( delta, maxIsrDur );
    }
}
