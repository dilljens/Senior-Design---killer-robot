/* ProfRepos.h
 *
 * Licence: See "LICENSE.txt"
 *
 * Author: James Jensen (Mechtro LLC)
 *         tiagojen@gmail.com
 *
 */

#ifndef _PROF_REPOS_H_
#define _PROF_REPOS_H_

#include "Profile.h"

class ProfRepos :public Profile{
  public:
    // low level motion instructions
    ProfileInstr motionInstr[3];
    uint32_t numUsedInstr;
    // Units:
    //   distance: revs
    //   time: sec

    // a trapazoidal profile that covers a totals dist, in 3 segments.
    // A - acc upto maxVel (not exceeding limitVel)
    // B - const vel
    // C - acc back down to zero.
    // Seg B can be zero if acc never reaches limit

    // full set of params to describe motion, just as an aid (could remove some)
    float dist;
    float distAcc;
    float distRun;
    float dt;
    float dtAcc;
    float dtRun;
    float accMag;
    float vel;
    uint8_t isTriangle;

    void dump(){
      prBuf.print("TrapCalc: ");
      if( isTriangle ){
        prBuf.println("Tri");
        prBuf.print(" dist(");
        prBuf.print(dist);
        prBuf.print(") = 2*distAcc(");
        prBuf.print(distAcc);
        prBuf.println(")");
        prBuf.print(" dt(");
        prBuf.print(dt);
        prBuf.print(") = 2*dtAcc(");
        prBuf.print(dtAcc);
        prBuf.println(")");
      }else{
        prBuf.println("Trap");
        prBuf.print(" dist(");
        prBuf.print(dist*1000);
        prBuf.print(") = 2*distAcc(");
        prBuf.print(distAcc*1000);
        prBuf.print(")+distRun(");
        prBuf.print(distRun*1000);
        prBuf.println(")");
        prBuf.print(" dt(");
        prBuf.print(dt*1000);
        prBuf.print(") = 2*dtAcc(");
        prBuf.print(dtAcc*1000);
        prBuf.print(")+dtRun(");
        prBuf.print(dtRun*1000);
        prBuf.println(")");
      }
      prBuf.print(" vel(");
      prBuf.print(vel*1000);
      prBuf.print(") acc(");
      prBuf.print(accMag*1000);
      prBuf.println(")");
      prBuf.print("  ");
      motionInstr[0].dump();
      prBuf.print("  ");
      motionInstr[1].dump();
      if( ! isTriangle ){
        prBuf.print("  ");
        motionInstr[2].dump();
      }
    }

    // Special case, repos, supply dist and duration, calc the rest.
    // will create a repos with acc so that acc ramp is fixed part of time
    // of whole move. I.e. same shape move.
    // uints: rev, sec
    void initWithDistTime(float _dist, float _dt){
      dist = _dist + MICRO_STEP_NUDGE_REVS_F; // add nudge for rounding
      dt = _dt;
      int32_t velSign = 1;
      float distMag = dist;
      if( dist < 0 ){
        velSign = -1;
        distMag = - dist;
      }
      float accTimeRatio = .15f; // amount of time used for acc on each end, must be less than .5

      dtAcc = dt*accTimeRatio;
      dtRun = dt-2*dtAcc;
      vel = distMag/(dt*(1-accTimeRatio));
      accMag = vel/dtAcc;

      // fill in common vars used for dump
      distAcc = accMag*dtAcc*dtAcc/2.0f;
      //distRun = distMag - 2*distAcc;
      distRun = dtRun * vel;
      prBuf.print(" XX distErr: ");
      prBuf.println(2*distAcc+distRun - distMag);
      isTriangle = false;
      numUsedInstr = 3;

      motionInstr[0].accBabySteps = (uint64_t)(accMag * BABY_STEPS_PER_REV / TICS_PER_SEC / TICS_PER_SEC); //Accel
      motionInstr[0].endVelBabySteps = (int64_t)(vel * BABY_STEPS_PER_REV / TICS_PER_SEC);
      motionInstr[0].dtTics = (uint64_t)(dtAcc * TICS_PER_SEC);
      motionInstr[0].velSign = velSign; // this profile only extends

      motionInstr[1].accBabySteps = 0;
      motionInstr[1].endVelBabySteps = (int64_t)(vel * BABY_STEPS_PER_REV / TICS_PER_SEC);
      motionInstr[1].dtTics = (uint64_t)(dtRun * TICS_PER_SEC);
      motionInstr[1].velSign = velSign;

      motionInstr[2].accBabySteps = - (uint64_t)(accMag * BABY_STEPS_PER_REV / TICS_PER_SEC / TICS_PER_SEC); //Accel
      motionInstr[2].endVelBabySteps = 0;
      motionInstr[2].dtTics = (uint64_t)(dtAcc * TICS_PER_SEC);
      motionInstr[2].velSign = velSign;

      prBuf.print(" stp:");
      prBuf.print(dist*MICRO_STEPS_PER_REV);
      prBuf.println(); //XX
    }

    // create simple reposition case, assumes starting/end vel zero,
    // Note that sign of distance describes the move
    // uints: rev, rev/sec, rev/(sec*sec)
    void initWithDistVelAcc(float _dist, float _maxVelMag, float _accMag){
      dist = _dist + MICRO_STEP_NUDGE_REVS_F; // add nudge for rounding
      float maxVelMag = abs(_maxVelMag); // 'mag' is short for magnitude
      accMag = abs(_accMag);
      float sign = 1;
      float distMag = dist;
      if( dist < 0 ){
        sign = -1;
        distMag = - dist;
      }
      // prevent div/0 issues by clamping to min value
      maxVelMag = max(maxVelMag, .01f);
      accMag = max(accMag , .01f);
      prBuf.print("XX dist:");
      prBuf.print(dist);
      prBuf.print(" distMag:");
      prBuf.print(distMag);
      prBuf.print(" velMag:");
      prBuf.print(maxVelMag);
      prBuf.print(" accMag:");
      prBuf.print(accMag);
      // find acc time/dist, decide if triangle or trapazoid vel graph
      dtAcc = maxVelMag/accMag;
      float distAccMag = accMag*dtAcc*dtAcc/2.0f;
      float distRunMag = distMag - 2*distAccMag; // const vel dist, assuming is trapazoid
      distAcc = distAccMag * sign;
      prBuf.print(" runMag:");
      prBuf.print(distRunMag);
      prBuf.print(" dtAcc:");
      prBuf.print(dtAcc);

      // fill in three motionInstr
      //motionInstr[0].assumedPreAccVel = 0;
      if( distRunMag > 0 ){ // Trap
        isTriangle = false;
        numUsedInstr = 3;
        dtRun = distRunMag/maxVelMag;
        dt = 2*dtAcc + dtRun;
        distRun = distRunMag * sign;
        vel = maxVelMag * sign;

        // convert acc from rev/(sec*sec) to babyStep/(tic * tic)
        motionInstr[0].accBabySteps = (uint64_t)(accMag * BABY_STEPS_PER_REV / TICS_PER_SEC / TICS_PER_SEC); //Accel
        motionInstr[0].endVelBabySteps = (int64_t)(vel * BABY_STEPS_PER_REV / TICS_PER_SEC);
        motionInstr[0].dtTics = (uint64_t)(dtAcc * TICS_PER_SEC);

        motionInstr[1].accBabySteps = 0;
        motionInstr[1].endVelBabySteps = motionInstr[0].endVelBabySteps;
        motionInstr[1].dtTics = (uint64_t)(dtRun * TICS_PER_SEC);

        motionInstr[2].accBabySteps = -motionInstr[0].accBabySteps; // decel
        motionInstr[2].endVelBabySteps = 0;
        motionInstr[2].dtTics = motionInstr[0].dtTics;

      }else{ // triangle
        isTriangle = true;
        numUsedInstr = 2;
        dtAcc = (float)sqrt(distMag/accMag);
        dt = 2*dtAcc;
        prBuf.print(" triDtAcc:");
        prBuf.print(dtAcc);
        vel = dtAcc * accMag * sign;

        motionInstr[0].accBabySteps = (uint64_t)(accMag * BABY_STEPS_PER_REV / TICS_PER_SEC / TICS_PER_SEC); // accel
        motionInstr[0].endVelBabySteps = (int64_t)(vel * BABY_STEPS_PER_REV / TICS_PER_SEC);
        motionInstr[0].dtTics = (uint64_t)(dtAcc * TICS_PER_SEC);

        motionInstr[1].accBabySteps = -motionInstr[0].accBabySteps; // decel
        motionInstr[1].endVelBabySteps = 0;
        motionInstr[1].dtTics = motionInstr[0].dtTics;
      }
      // store direction
      motionInstr[0].velSign = (vel >= 0) ? 1 : -1 ;
      motionInstr[1].velSign = motionInstr[0].velSign;
      motionInstr[2].velSign = motionInstr[0].velSign;

      prBuf.print(" stp:");
      prBuf.print(dist*MICRO_STEPS_PER_REV);
      prBuf.println(); //XX

    }

    // implement profile interface:
    virtual ProfileInstr * getFirstInstr(){
      return motionInstr;
    }
    virtual uint32_t getNumInstr(){
      return numUsedInstr;
    }

};

#endif // _PROF_REPOS_H_
