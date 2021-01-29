/* ProfStop.h
 *
 * Licence: See "LICENSE.txt"
 *
 * Author: James Jensen (Mechtro LLC)
 *         tiagojen@gmail.com
 *
 */

#ifndef _PROF_STOP_H_
#define _PROF_STOP_H_

#include "Profile.h"

class ProfStop :public Profile{
  public:
    // low level motion instructions
    ProfileInstr motionInstr[1];
    // Units:
    //   distance: revs
    //   time: sec

    // full set of params to describe motion, just as an aid (could remove some)
    float startVel;
    float accMag;
    float dist;
    float dt;

    void dump(){
      prBuf.print("StopCalc: ");
      prBuf.print(" dist(");
      prBuf.print(dist);
      prBuf.print(") dt(");
      prBuf.println(dt);

      prBuf.print("  ");
      motionInstr[0].dump();
    }

    // create simple reposition case, assumes starting/end vel zero,
    // Note that sign of distance describes the move
    // uints: mrev, mrev/sec, mrev/(sec*sec)
    void init(float _startVel, float _accMag){
      startVel = _startVel;
      float velSign = 1;
      if( startVel < 0 ){
        velSign = -1;
      }
      accMag = abs(_accMag);
      float velMag = abs(startVel);
      dt = velMag/accMag;
      dist = velMag * dt / 2.0f;

      motionInstr[0].accBabySteps = -(uint64_t)(accMag * BABY_STEPS_PER_REV / TICS_PER_SEC / TICS_PER_SEC); //Accel
      motionInstr[0].endVelBabySteps = 0;
      motionInstr[0].dtTics = (uint64_t)(dt * TICS_PER_SEC);
      motionInstr[0].velSign = velSign;

      prBuf.print("XX Stop steps:");
      prBuf.print(dist*MICRO_STEPS_PER_REV);
      prBuf.println(); //XX
    }

    // implement profile interface:
    virtual ProfileInstr * getFirstInstr(){
      return motionInstr;
    }
    virtual uint32_t getNumInstr(){
      return 1;
    }

};

#endif // _PROF_STOP_H_
