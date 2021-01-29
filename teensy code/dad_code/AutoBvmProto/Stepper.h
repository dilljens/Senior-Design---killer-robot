/* Stepper.h
 *
 * Licence: See "LICENSE.txt"
 *
 * Author: James Jensen (Mechtro LLC)
 *         tiagojen@gmail.com
 *
 */

#ifndef _STEPPER_H_
#define _STEPPER_H_

#include "EpCommon.h"
#include "Profile.h"
#include "ProfStop.h"

class Stepper{
  public:
    // a free-running partStep position accumulator
    int32_t position;

  // implementation: not intended to access outside, but leave public for debug
    // ---------------------------------------------------------------
    uint8_t _dirPin;
    uint8_t _stepPin;

    // ---------------------------------------------------------------
    // data for current running instr
    ProfileInstr * _pFirstInstr;
    uint32_t _numInstr;
    uint32_t _instrIndex;

    ProfileInstr * lastInstrLoaded[2]; //useful for debug
    ProfileInstr * lastInstrCompleted[2]; //useful for debug

    // ---------------------------------------------------------------
    ProfStop profStop; // used to stop stepper

    // ---------------------------------------------------------------
    // keep track of run state, used by ISR
    uint32_t _elapsedTics;
    int64_t _posAccumBabyStep; // used to accum one step
    bool _stepIsRequested;
    bool _stepIsActive;

    int64_t _currentAccBabySteps;
    int64_t _currentVelBabySteps; // signed for easier math, but clamped to be positive
    int32_t _velSign; // direction flag

    uint64_t maxStepVel; // for debug

    // ===============================================================
    Stepper(uint8_t stepPin, uint8_t dirPin){
      _stepPin = stepPin;
      _dirPin = dirPin;
      position = 0;
      _currentVelBabySteps = 0;
      _currentAccBabySteps = 0;
      _velSign = 1;
      _stepIsRequested = 0;
    }

    inline void _loadMotorInstr(ProfileInstr * pInstr){
      _currentAccBabySteps = pInstr->accBabySteps;
      _velSign = pInstr->velSign;
      digitalWriteFast(_dirPin, _velSign < 0); // pos extends actuator
      //instrEndTimeUs += pInstr->dt;
      lastInstrLoaded[0] = pInstr; //save two deep
      lastInstrLoaded[1] = lastInstrLoaded[0];
      _elapsedTics = 0;
    }

    void startProfile(Profile * pProf){
      if( pProf ){
        _pFirstInstr = pProf->getFirstInstr();
        _numInstr = pProf->getNumInstr();
        _instrIndex = 0;
        _posAccumBabyStep = 0;
        _loadMotorInstr(_pFirstInstr);
      }
    }
    inline void startProfile(Profile & prof){
      startProfile(&prof);
    }

    bool isActive(){
      return _pFirstInstr != NULL;
    }

    // set acc to stop
    void stop(float accRpss){
      if(isActive() || _currentVelBabySteps != 0){
        float velRps = ((float)_currentVelBabySteps)*((float)TICS_PER_SEC)/((float)BABY_STEPS_PER_REV);
        profStop.init(velRps, accRpss);
        startProfile(profStop);
      }// else already stopped
    }

    void halt(){
      _pFirstInstr = NULL;
      _currentVelBabySteps = 0;
    }

    // get current velocity in Rev/Sec with correct sign
    float getCurrentVelRpsF(){
      return ((float)_currentVelBabySteps)/1000000.0f/(float)(BABY_STEPS_PER_REV*_velSign);
    }

    bool isProfileRunning(){
      return _pFirstInstr != NULL;
    }

    inline void _doAccum(){
      _currentVelBabySteps += _currentAccBabySteps;
      // never let vel be neg
      if( _currentVelBabySteps < 0) _currentVelBabySteps = 0;
      // clamp to 1 step per isr
      // If want to monitor margin, watch current vel prop
      if( maxStepVel < (uint64_t) _currentVelBabySteps ){
        maxStepVel = _currentVelBabySteps;
      }
      if( _currentVelBabySteps > (int64_t)BABY_STEPS_PER_MICRO_STEP) _currentVelBabySteps = BABY_STEPS_PER_MICRO_STEP;
      _posAccumBabyStep += _currentVelBabySteps;
      if( _posAccumBabyStep >= (int64_t)BABY_STEPS_PER_MICRO_STEP){
        _stepIsRequested = true;
        _posAccumBabyStep -= BABY_STEPS_PER_MICRO_STEP;
        position += _velSign; //
      }
    }

    void _processFromIsr(){

      if( _pFirstInstr ){ // a profile is active
        uint32_t startStamp = micros();

        // first thing, check for prev step req, then do calc during
        // step delay for next one
        if( _stepIsRequested ){
          digitalWriteFast(_stepPin, 1);
          _stepIsRequested = false;
          _stepIsActive = true;
        }

        // move to next instr when done
        ProfileInstr * pThisInstr = _pFirstInstr + _instrIndex;
        if( _elapsedTics >= pThisInstr->dtTics){
          lastInstrCompleted[0] = pThisInstr; //save two deep
          lastInstrCompleted[1] = lastInstrCompleted[0];
          _instrIndex ++;
          if( _instrIndex >= _numInstr ){
            // Profile Done, no more instructions
            _pFirstInstr = NULL; // stop running instructions
            _currentAccBabySteps = 0; // make sure no acc is left
            // let vel be non zero to keep going last commanded vel
          }else{
            // still have more,
            _loadMotorInstr(_pFirstInstr + _instrIndex); // next one
          }
        }else{
          _elapsedTics ++;
        }
        _doAccum();

        // block/hardspin, wait for end of pulse delay
        if( _stepIsActive ){
          while((micros() - startStamp) < 5);
          digitalWriteFast(_stepPin, 0);
        }

      }else if(_currentVelBabySteps){ // if not set to zero, keeps running last setting
        // don't incr elapsed tics when not running an instruction
        _doAccum();
      }
    }

};

extern uint32_t isrCnt;
// --------------------------------------------------------------------
extern Stepper stepper1;
//extern Stepper stepper2;
//extern Stepper stepper3;

// --------------------------------------------------------------------
extern uint32_t maxIsrDur;

// called from ISR
void intervalTimerFun();

extern IntervalTimer intervalTimer;

#endif //_STEPPER_H_
