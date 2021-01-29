/* SvcHome.h
 *
 * Licence: See "LICENSE.txt"
 *
 * Author: James Jensen (Mechtro LLC)
 *         tiagojen@gmail.com
 *
 */


#ifndef _EP_SVC_HOME_H_
#define _EP_SVC_HOME_H_

#include "IsElapsedTime.h"
#include "EpCommon.h"
#include "EpSvc.h"

enum ERR_HOME {
  ERR_HOME_none = 0,
  ERR_HOME_stepper_busy = 10,
  ERR_HOME_no_sensor_retracting_to_home,
  ERR_HOME_no_sensor_extending_to_home
};

class SvcHome: public Svc{
    IsElapsedMs32 delayEms;

    uint32_t state;
    uint8_t errCode = ERR_HOME_none;
    ProfRepos profRepos;

    // debug
    uint32_t oldState;
    uint8_t oldErrCode = ERR_HOME_none;

  public:

  	SvcHome():
      delayEms(advancer)
  	{
  		reset();
      pSvcName = "Home";
  	}


  	void reset(){
  		resetFlags();
      state = 0;
      oldState = 0;
      errCode = ERR_HOME_none;
      oldErrCode = ERR_HOME_none;
  	}

    // Svc Iface ---------------------------------------------------
  	void eval(){
      switch(state){
        case 0: // just starting, do init
          if( hw.snsHome ){// is extended
            profRepos.initWithDistVelAcc(HOME_RETRACT_DIST_REV, HOME_VEL_RPS, HOME_ACC_RPSS);
            stepper1.startProfile(profRepos);
            state = 100;
            break;
          }// else is retracted
          profRepos.initWithDistVelAcc(HOME_EXTEND_DIST_REV, HOME_VEL_RPS, HOME_ACC_RPSS);
          stepper1.startProfile(profRepos);
          state = 200;
          break;

        case 100: // retracting back to home
          // wait for either home sensor, or timeout
          if( ! hw.snsHome){ // got transition
            stepper1.position = 0;
            state = 300;
            break;
          }
          if( ! stepper1.isActive()){
            // got to end of motion, without transition
            errCode = ERR_HOME_no_sensor_retracting_to_home;
            state = 1000;
          }
          break;

        case 200: // extending out to home
          // wait for either home sensor, or timeout
          if( hw.snsHome){ // got transition
            stepper1.position = 0;
            state = 300;
            break;
          }
          if( ! stepper1.isActive()){
            // got to end of motion, without transition
            errCode = ERR_HOME_no_sensor_extending_to_home;
            state = 1000;
          }
          break;

        case 300: // continue homing, stop motor, move to idle
          stepper1.stop(HOME_ACC_RPSS);
          state = 350;
          break;

        case 350: // continue homing, stop motor, move to idle
          if( ! stepper1.isActive() ){
            state = 400;
          }
          break;

        case 400: // continue homing, stop motor, move to idle
          {
            float posRevs = (float)(stepper1.position) / (float)MICRO_STEPS_PER_REV;
            profRepos.initWithDistVelAcc(IDLE_POS_REVS - posRevs, HOME_VEL_RPS, HOME_ACC_RPSS);
            stepper1.startProfile(profRepos);
            state = 500;
          }
          break;

        case 500: // wait for end of final positioning
          if( ! stepper1.isActive() ){
            state = 1000; // done
          }
          break;

        default: // 1000 or any other
          stopped = true; // Stop Svc
          break;
      } // end switch

      // XX debug
      if( oldState != state ){
        oldState = state;
        prBuf.prependMs();
        prBuf.print("Home state -> ");
        prBuf.println(state);
      }
      if( oldErrCode != errCode ){
        oldErrCode = errCode;
        prBuf.prependMs();
        prBuf.print("Home err -> ");
        prBuf.println(errCode);
      }

  	}// end eval()
};

#endif // _EP_SVC_HOME_H_
