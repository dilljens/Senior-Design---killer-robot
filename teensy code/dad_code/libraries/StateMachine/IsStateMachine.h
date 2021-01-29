/*
 *  A library to define StateMachine objects to aid in parallel algoritms
 *
 * Licence: See "LICENSE.txt"
 *
 * Author: James Jensen (Mechtro LLC)
 *         tiagojen@gmail.com
 *
 */

#ifndef _IS_STATE_MACHINE_H_
#define _IS_STATE_MACHINE_H_

#include "Arduino.h"
#include "IsElapsedTime.h"

#define IS_STATE_MACHINE_LIB_VER 0x00010001ul

/*========================================================

 *========================================================*/
class IsStateMachine{
  protected:
    const char * pLabel; // overwrite with useful names

  public:
    IsStateMachine(){
        pLabel = "?";
    }

    // this evaluates the state of the machine and my do stuff
    virtual void eval(){
        // base class does nothing
    }
};

/*========================================================
 * Base class for state machines that advance through integer
 * state, and keep track of errors.
 *========================================================*/
class IsSeq: public IsStateMachine{

  protected:
    uint32_t errorTimeStampMs;
    IsSeq* pChildWithError; //used to chain errors

    // some misc fields to use if needed
    uint16_t state;
    uint16_t lastState;
    uint16_t errorFromState; // state that declared error

    // error helper if wanted NOTE: uses 1000 as hardCoded error
    // state, override this function if other behavior desired
    void err(const char * pErr){
        errorFromState = state;
        state = 1000;
        pError = pErr;
    }
    // call in eval loop if desired to see state transitions
    void evalStateDebug(){
        if( state != lastState ){ //XX
            Serial.print("--- Switch ");
            Serial.print(pLabel);
            Serial.print(" from ");
            Serial.print(lastState);
            Serial.print(" to ");
            Serial.println(state);
            lastState = state;
        }
    }

  public:
    const char * pError;

    IsSeq(){
        pError = NULL;
        errorTimeStampMs = 0;
        pChildWithError = NULL; //used to chain errors
        state = 0;
        lastState = 0;
        errorFromState = 0;
        pLabel = "?";
    }

};

#endif // _IS_STATE_MACHINE_H_
