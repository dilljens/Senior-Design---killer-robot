/* SvcBreath.h - A service to perform one breath cycle
 *
 * Licence: See "LICENSE.txt"
 *
 * Author: James Jensen (Mechtro LLC)
 *         tiagojen@gmail.com
 *
 */

#ifndef _EP_SVC_BREATH_H_
#define _EP_SVC_BREATH_H_

#include "IsElapsedTime.h"
#include "EpCommon.h"
#include "EpSvc.h"

enum ERR_BREATH {
  ERR_BREATH_none = 0,
  ERR_BREATH_not_starting_in_idle_pos = 50,
};

class SvcBreath: public Svc{
    IsElapsedMs32 delayEms;

    uint32_t state;
    uint8_t errCode = ERR_HOME_none;
    ProfRepos profRepos;
    ProfInhale profInhale;

    uint32_t cycleTimeMs;
    uint32_t cycleCnt;
    float posErrStepsF;
    uint8_t oldSnsHome;

    // debug
    uint32_t oldState;
    uint8_t oldErrCode = ERR_HOME_none;

  public:

    uint8_t continuous; // flag to stop or keep going


  	SvcBreath():
      delayEms(advancer)
  	{
  		reset();
      pSvcName = "Breath";
      continuous = true;
      cycleCnt = 0;
  	}


  	void reset(){
  		resetFlags();
      state = 0;
      oldState = 0;
      errCode = ERR_HOME_none;
      oldErrCode = ERR_HOME_none;
      oldSnsHome = 0;
  	}


    // Svc Iface ---------------------------------------------------
  	void eval(){
      if( oldSnsHome != hw.snsHome ){
        if( oldSnsHome && ! hw.snsHome ){ // retracting
          int32_t oldPos = stepper1.position;
          prBuf.print("XX ====== XX Rezero Home sensor from ");
          prBuf.println(oldPos);
          stepper1.position = 0;
        }
        oldSnsHome = hw.snsHome;
      }
      switch(state){
        case 0: // just starting, do init
        // use 01 suffix to make state numbers a bit easier to differentiate from other svc
        case 101:
        {
          /*
          posErrStepsF = ((float)(stepper1.position)) - ((float)IDLE_POS_MICRO_STEPS);
          prBuf.print("posErrSteps:"); //XX
          prBuf.println(posErrStepsF);
          if( abs(posErrStepsF) > 10.0f ){
            prBuf.print("!! not in idle pos, posErr(steps) = ");
            prBuf.println(posErrStepsF);
            errCode = ERR_BREATH_not_starting_in_idle_pos;
            state = 1001; // done
            break;
          }
          */
          state = 201;
          break;
        }

        case 201: // calc motions, start exhale
        {
          delayEms.elapsed = 0;
          float tv = hw.getTidalVolumeF();
          float ie = hw.get_I_E_RatioF();
          float bpm = hw.getBreathsPerMinF();

          cycleCnt++;
          prBuf.prependMs();
          prBuf.print(" --- Start Br Cycle #");
          prBuf.print(cycleCnt);
          prBuf.print(" tv=");
          prBuf.print(tv);
          prBuf.print(" ie=");
          prBuf.print(ie);
          prBuf.print(" bpm=");
          prBuf.println(bpm);


          float correction =  IDLE_POS_REVS - ((float)stepper1.position)/((float)MICRO_STEPS_PER_REV);
          prBuf.print(" XX pos => ");
          prBuf.print(stepper1.position);
          prBuf.print(" correction ");
          prBuf.println(correction);
          float dist = tv*MAX_EXTEND_FROM_IDLE;
          float cycleTimeSec = SECS_PER_MIN / bpm; // sec/breath
          cycleTimeMs = (uint32_t)(cycleTimeSec*1000);
          float exhaleTime = cycleTimeSec/(ie+1);
          float inhaleTime = cycleTimeSec - exhaleTime;
          // make exhale retract faster then wait
          float exhaleIdleTime = EXHALE_IDLE_RATIO * exhaleTime;
          float exhaleMotionTime = exhaleTime - exhaleIdleTime;
          // calc return trip
          profRepos.initWithDistTime(-dist, exhaleMotionTime);
          // calc and start extension
          profInhale.init(dist+correction, inhaleTime, .5f);
          stepper1.startProfile(profInhale);

          profInhale.dump(); //XX
          profRepos.dump(); //XX

          state = 301;
          break;
        }

        case 301: // wait for end of exhale, start inhale
          if( ! stepper1.isActive() ){
            prBuf.print(" XX endIn, start Ex, pos => ");
            prBuf.println(stepper1.position);
            stepper1.startProfile(profRepos);
            state = 351;
          }
          break;

        case 351: // wait for end of cycle
          if( ! stepper1.isActive() ){
            /*
            prBuf.print(" XX end Ex, pos => ");
            prBuf.println(stepper1.position);
            // adjust out any error
            float correction =  IDLE_POS_REVS - ((float)stepper1.position)/((float)MICRO_STEPS_PER_REV);
            prBuf.print(" XX correction => ");
            prBuf.println(correction);
            profRepos.initWithDistVelAcc(correction, 2, 8000);
            profRepos.dump();
            stepper1.startProfile(profRepos);
            */
            state = 401;
          }
          break;

        case 401: // wait for end of cycle
          if( delayEms.elapsed >= cycleTimeMs && !stepper1.isActive()){
            prBuf.print(" XX endCycle pos => ");
            prBuf.println(stepper1.position);
            if( continuous ){
              state = 201;
            }else{
              state = 1001;
            }
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
        prBuf.print("Breath state -> ");
        prBuf.println(state);
      }
      if( oldErrCode != errCode ){
        oldErrCode = errCode;
        prBuf.prependMs();
        prBuf.print("Breath err -> ");
        prBuf.println(errCode);
      }

  	}// end eval()
};

#endif // _EP_SVC_BREATH_H_
