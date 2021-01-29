/* Profile.h - Motion instructions and Motion Profiles
 *
 * Licence: See "LICENSE.txt"
 *
 * Author: James Jensen (Mechtro LLC)
 *         tiagojen@gmail.com
 *
 * This class abstracts transitions between motor motion segments.
 * It represents acceleration for a specified time (or target vel)
 */


#ifndef _EP_MOTION_INSTR_H_
#define _EP_MOTION_INSTR_H_

class ProfileInstr{

  public:

    // To represent a const acc segment,
    // Distances are always unidirectional (positive)
    // Other code sets dir bit.
    //int32_t assumedPreVel; // (steps/sec) Not used directly, just for checking assumptions
    int64_t accBabySteps; // signed (microSteps/(sec*sec)) "Micro" refers to 1 millionth of a step
    uint64_t endVelBabySteps; //usigned (microSteps/sec)
    //uint64_t distMicroSteps; // (ideal steps)
    uint32_t dtTics; // delta time of acc and run
    int32_t velSign; // directionallity of Instruction

    void dump(){
      prBuf.print("INSTR a(");
      //prBuf.print(assumedPreVel);
      //prBuf.print(") a(");
      prBuf.print((float)accBabySteps);
      prBuf.print(") post(");
      prBuf.print((float)endVelBabySteps);
      //prBuf.print(") dist(");
      //prBuf.print((float)distMicroSteps);
      prBuf.print(") dt(");
      prBuf.print(dtTics);
      prBuf.println(")");
    }

    void clear(){
      accBabySteps = 0;
      endVelBabySteps = 0;
      dtTics = 0;
      velSign = 1.0f;
    }

};

class Profile{
  public:
    virtual ProfileInstr * getFirstInstr();
    virtual uint32_t getNumInstr();
};

#endif // _EP_MOTION_INSTR_H_
