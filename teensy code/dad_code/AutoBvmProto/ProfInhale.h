/* ProfileInhale.h - Inhale Profile
 *
 * Licence: See "LICENSE.txt"
 *
 * Author: James Jensen (Mechtro LLC)
 *         tiagojen@gmail.com
 *
 * This is a special reposition profile that starts fast and decelerates
 * as it extends to help account for the non-linear ramp of volume to depression
 * of a bag.
 *
 * The slope of the decrease is set by the ReductionFactor, which determines
 * the ratio of start and end vel at the max dist.
 *
 * The profile is also defined by distance to travel, time to travel, and accelerations.
 *
 */

#ifndef _PROF_INHALE_H_
#define _PROF_INHALE_H_

#include "Profile.h"

class ProfInhale :public Profile{
  public:
    // low level motion instructions
    ProfileInstr motionInstr[3];
    /*
     * Units:
     *   distance: revs
     *   time: sec
     *
     *  Velocity diagram
     *        velA
     *         /'~,  accB
     *        / |  '~,
     * accA  /  |     '~,  velB
     *      /   |        '~,
     *     /    |          |\  accC
     *    /     |          | \
     *  0/      |          |  \0
     *  ----------------------
     *     tA       tB       tC
     * distance: d is area under curve, (dA + dB + dC)
     *
     * reduction factor  redF = (1 - vB/vA)  at dMax
     *
     * Write big equation, solve for vA with quadratic
     *
     */

    //float dMax; // defines reference state for the reductionFactor
    double redF; // retuction factor for the given dMax

    double aA; // acc
    double aB;
    double aC;
    double vA; // vel
    double vB;
    double dA; // dist
    double dB;
    double dC;
    double d;
    double tA; // time
    double tB;
    double tC;
    double t;

    uint32_t errCode;

    void dump(){
      prBuf.print("InhaleCalc: ");
      if( errCode ){
        prBuf.print(" errCode(");
        prBuf.print(errCode);
        prBuf.println(")");
      }else{
        prBuf.print(" d(");
        prBuf.print((float)d);
        prBuf.print(") redF(");
        prBuf.print((float)redF);
        prBuf.println(")");

        prBuf.print(" aA(");
        prBuf.print((float)aA);
        prBuf.print(") aB(");
        prBuf.print((float)aB);
        prBuf.print(") aC(");
        prBuf.print((float)aC);
        prBuf.print(") vA(");
        prBuf.print((float)vA);
        prBuf.print(") vB(");
        prBuf.print((float)vB);
        prBuf.println(")");

        prBuf.print(" d(");
        prBuf.print((float)d);
        prBuf.print(") dA(");
        prBuf.print((float)dA);
        prBuf.print(") dB(");
        prBuf.print((float)dB);
        prBuf.print(") dC(");
        prBuf.print((float)dC);
        prBuf.println(")");

        prBuf.print(" t(");
        prBuf.print((float)t);
        prBuf.print(") tA(");
        prBuf.print((float)tA);
        prBuf.print(") tB(");
        prBuf.print((float)tB);
        prBuf.print(") tC(");
        prBuf.print((float)tC);
        prBuf.println(")");

        prBuf.print("  ");
        motionInstr[0].dump();
        prBuf.print("  ");
        motionInstr[1].dump();
        prBuf.print("  ");
        motionInstr[2].dump();
      }
    }

    // uints: rev, sec
    void init(float dist, float dt, float reductionFactor){
      // first set all motion segments to default, in case of calc error
      motionInstr[0].clear();
      motionInstr[1].clear();
      motionInstr[2].clear();
      errCode = 0;

      d = abs(dist); // ignore any sign
      if( dist > MAX_EXTEND_FROM_IDLE ){
        errCode = 1; // Exceeded max defined distance
        return;
      }
      t = dt;
      redF = abs(reductionFactor);
      aA = 1000.0f;
      aC = 3000.0f;

      // a couple convenience terms
      double fi = 1.0f - redF;  // inverse factor
      double p = 2 - redF; // arbitrary factor

      // Solve for vA using quadratic
      // Quadratic coefficients 'a' 'b' and 'c'
      double qA =
        + aC
        + aA*fi*fi
        - p*aC
        - p*aA*fi;

      double qB = p*aA*aC*t;

      //XX double qC = -2*aA*aC*MAX_EXTEND_FROM_IDLE;
      double qC = -2*aA*aC*d;

      // find roots with quadratic
      double det = qB*qB - 4*qA*qC;
      if( det < 0){
        errCode = 2; // no real roots
        return;
      }
      double sqrtDet = sqrt(det);
      double den = 2*qA;
      double root1 = (-qB + sqrtDet)/den;
      double root2 = (-qB - sqrtDet)/den;
      // now pick best one and put in root1
      if( root1 > 0 && root2 > 0){
        vA = min(root1, root2); // pick lower of two possible
      }else if(root1 > 0){
        vA = root1;
      }else if(root2 > 0){
        vA = root2;
      }else{
        errCode = 3; // no positive roots
        return;
      }

      // fill out remaining params
      vB = vA*fi;
      tA = vA/aA;
      tC = vB/aC;
      tB = t - tA - tC;
      if( tB < 0 ){
        errCode = 4; // negative time segment
        return;
      }
      aB = (vA - vB)/tB;
      dA = aA*tA*tA/2.0f;
      dB = (vA + vB)/2.0f*tB;
      dC = aC*tC*tC/2.0f;

      motionInstr[0].accBabySteps = (uint64_t)(aA * BABY_STEPS_PER_REV / TICS_PER_SEC / TICS_PER_SEC); //Accel
      motionInstr[0].endVelBabySteps = (int64_t)(vA * BABY_STEPS_PER_REV / TICS_PER_SEC);
      motionInstr[0].dtTics = (uint64_t)(tA * TICS_PER_SEC);
      motionInstr[0].velSign = 1; // this profile only extends

      motionInstr[1].accBabySteps = - (uint64_t)(aB * BABY_STEPS_PER_REV / TICS_PER_SEC / TICS_PER_SEC); //Accel
      motionInstr[1].endVelBabySteps = (int64_t)(vB * BABY_STEPS_PER_REV / TICS_PER_SEC);
      motionInstr[1].dtTics = (uint64_t)(tB * TICS_PER_SEC);
      motionInstr[1].velSign = 1;

      motionInstr[2].accBabySteps = - (uint64_t)(aC * BABY_STEPS_PER_REV / TICS_PER_SEC / TICS_PER_SEC); //Accel
      motionInstr[2].endVelBabySteps = 0;
      motionInstr[2].dtTics = (uint64_t)(tC * TICS_PER_SEC);
      motionInstr[2].velSign = 1;

      prBuf.print(" stp:");
      prBuf.print(d*MICRO_STEPS_PER_REV);
      prBuf.println(); //XX

    }

    // implement profile interface:
    virtual ProfileInstr * getFirstInstr(){
      return motionInstr;
    }
    virtual uint32_t getNumInstr(){
      return 3;
    }

};

#endif // _PROF_INHALE_H_
