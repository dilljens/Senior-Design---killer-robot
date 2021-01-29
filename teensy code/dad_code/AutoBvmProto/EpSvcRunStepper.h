/* EpSvcRunStepper.h
 *
 * Licence: See "LICENSE.txt"
 *
 * Author: James Jensen (Mechtro LLC)
 *         tiagojen@gmail.com
 *
 */

#ifndef _EP_SVC_RUN_STEPPER_H_
#define _EP_SVC_RUN_STEPPER_H_

#include "IsElapsedTime.h"
#include "EpSvc.h"

// --------------------------------------------------------------------
// a base class for classes that encapsulate a sequence to execute on
// an agent

class SvcRunStepper: public Svc{
    /*
    IsElapsedMs32 delayEms;

    uint8_t stepperId;
    uint32_t state;
    int32_t dir;
    uint32_t accTimeMs = 0;

    AccToConstVelProfile startProf;
    AccToConstVelProfile stopProf;

    uint8_t stopWasBegun;
    */
  public:

  	SvcRunStepper()//:
      //delayEms(advancer),
      //state(0)
  	{
  		reset();
      pSvcName = "RunStepper";
      //setStepperId(1); // default id
  	}

    void setStepperId(uint8_t _stepperId){
      /*
      stepperId = _stepperId;
      if( stepperId < 1 ){ stepperId = 1; }
      if( stepperId > 3 ){ stepperId = 3; }
      */
    }

    // use setupMove and setupReposition instead of calling this directly
    // as this doens't do the direction part
    void setupProfile(int32_t vel){
      /*
      dir = vel < 0 ? -1 : 1 ;
      startProf.cmdAbsAcc = ACC_TO_RPM_IN_SEC( 350, .5 );
      startProf.startVel = 0;
      startProf.cmdVel = abs(vel);
      startProf.setup();
      //startProf.dump();

      stopProf.cmdAbsAcc = startProf.cmdAbsAcc;
      stopProf.startVel = startProf.cmdVel;
      stopProf.cmdVel = 0;
      stopProf.setup();
      //stopProf.dump();
      accTimeMs = MSEC_PER_SEC * startProf.cmdVel / startProf.cmdAbsAcc / TICS_PER_SEC;
      */
    }

    void beginStop(){
      //stopWasBegun = true;
    }

    // Svc Iface ---------------------------------------------------
    void precheck(){
      //hw.getStepperResource(stepperId).assertReleased(pSvcName);
    }

    void setup(){
      //reset();
    }

  	void reset(){
      /*
  		resetFlags();
      startProf.reset();
  		stopProf.reset();
      state = 0;
      stopWasBegun = false;
      */
  	}

  	void eval(){
      /*
      if( ! started ){
        started = true;
        state = 0;
      }

      if( stopped ) return;
      if( wasAborted() ){
        setStopped();
        return;
      }

      hw.stepper1Pos.eval();

      switch( state ){
        case 0:
          hw.getStepperResource(stepperId).checkout(pSvcName);
          hw.startStepper(stepperId, startProf, dir);
          prBuf.print("Start stepperId[");
          prBuf.print(stepperId);
          prBuf.print("] dir[");
          prBuf.print(dir);
          prBuf.println("]");

          state = 10;
          break;

        case 10:
          if( stopWasBegun ){
            prBuf.println("Stop was begun");
            hw.startStepper(stepperId, stopProf, dir);
            delayEms.elapsed = 0;
            state = 20;
          }
          break;

        case 20:
          if( delayEms.elapsed > accTimeMs ){
            prBuf.println("Run profile done");
            setStopped();
            hw.getStepperResource(stepperId).release();
            state = 30;
          }
          break;

        case 30: // idle
          break;

      }
      */


  	}
};


#endif // _EP_SVC_RUN_STEPPER_H_
