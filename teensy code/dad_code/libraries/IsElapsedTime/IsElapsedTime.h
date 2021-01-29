/*
 *  A library to manage multiple elapsed time trackers
 *
 *  Ms32 can represent upto 49 weeks
 *  Ms64 can represent upto 580000000 Yr (forever)

 *  Us32 can represent upto 71 minutes
 *  Us64 can represent upto 580000 Yr (forever)
 *
 *  Licence: See "LICENSE.txt"
 *
 *  Author: James Jensen (Mechtro LLC)
 *          tiagojen@gmail.com
 *
 */

#ifndef _IS_ELAPSED_TIME_H_
#define _IS_ELAPSED_TIME_H_

#include "Arduino.h"

#define IS_ELAPSED_TIME_LIB_VER 0x00010002ul
//#define UINT32_MAX 0xFFFFFFFFul

// =======================================================
// Class predeclarations
class IsElapsedMs32;
//class ElapsedMs64;
class IsElapsedUs32;
//class ElapsedUs64;

/*========================================================
 * Class to advance collection of elapsed times
 *========================================================*/

class IsAdvancer{
    // some linked lists of managed times
    IsElapsedMs32 * pHeadElapsedMs32;
    //ElapsedMs64 * pHeadElapsedMs64;
    IsElapsedUs32 * pHeadElapsedUs32;
    //ElapsedUs64 * pHeadElapsedUs64;

    uint32_t lastMicros;
    uint32_t millisRemainderUs; // accum micros to incr millis

  public:
    IsAdvancer(){
        pHeadElapsedMs32 = NULL;
        pHeadElapsedUs32 = NULL;
        lastMicros = 0;
        millisRemainderUs = 0;
    };

    void link(IsElapsedMs32 *pNode);
    void unlink(IsElapsedMs32 *pNode);
    void link(IsElapsedUs32 *pNode);
    void unlink(IsElapsedUs32 *pNode);

    // System needs to call this frequently to update
    // the time stamps
    void advance();
    void advanceUs(uint32_t micros);

    friend class IsElapsedMs32;
    friend class IsElapsedUs32;
};

/*========================================================
 * Class track 32bit elapsed Millisecond times
 *========================================================*/
class IsElapsedMs32{
  private:

    IsAdvancer * pAdvancer;
    IsElapsedMs32 * pNext; // linked list node
    bool enabled;

    void update(uint32_t deltaMs){
        // prevent overflow
        uint32_t maxDelta = UINT32_MAX - elapsed;
        elapsed += (deltaMs < maxDelta) ? deltaMs : maxDelta;
    }

    void setup(){
        pNext = NULL;
        enabled = false;
        elapsed = 0;
        enable(true);
    }

  public:
    uint32_t elapsed;

    // warning!! don't call this version and forget to initialize
    // with the advancer ptr
    // this is needed for array of obj support
    IsElapsedMs32(){
        pNext = NULL;
        enabled = false;
        elapsed = 0;
    }

    // only for special case use with no arg constructor for arrays of obj
    void init(IsAdvancer * _pAdvancer){
        pAdvancer = _pAdvancer;
        setup();
    }


    IsElapsedMs32(IsAdvancer * _pAdvancer){
        pAdvancer = _pAdvancer;
        setup();
    }

    IsElapsedMs32(IsAdvancer & advancer){
        pAdvancer = &advancer;
        setup();
    }

    void enable(bool en){
        if(en && !enabled){ // only link on first enable
            pAdvancer->link(this);
        }else if(!en && enabled){ // only unlink on first disable
            pAdvancer->unlink(this);
        }
        enabled = en;
    };
    bool isEnabled(){ return enabled; }


    friend class IsAdvancer; // allow advancer to update
};

/*========================================================
 * Class track 32bit elapsed Microsecond times
 *========================================================*/
class IsElapsedUs32{
  private:
    IsAdvancer * pAdvancer;
    IsElapsedUs32 * pNext; // linked list node
    bool enabled;

    void update(uint32_t deltaUs){
        // prevent overflow
        uint32_t maxDelta = UINT32_MAX - elapsed;
        elapsed += (deltaUs < maxDelta) ? deltaUs : maxDelta;
    };

    void setup(){
        pNext = NULL;
        enabled = false;
        elapsed = 0;
        enable(true);
    }

  public:
    uint32_t elapsed;

    // warning!! don't call this version and forget to initialize
    // with the advancer ptr
    // this is needed for array of obj support
    IsElapsedUs32(){
        pNext = NULL;
        enabled = false;
        elapsed = 0;
    }

    // only for special case use with no arg constructor for arrays of obj
    void init(IsAdvancer * _pAdvancer){
        pAdvancer = _pAdvancer;
        setup();
    }


    IsElapsedUs32(IsAdvancer * _pAdvancer){
        pAdvancer = _pAdvancer;
        setup();
    }

    IsElapsedUs32(IsAdvancer & advancer){
        pAdvancer = &advancer;
        setup();
    }

    void enable(bool en){
        if(en && !enabled){ // only link on first enable
            pAdvancer->link(this);
        }else if(!en && enabled){ // only unlink on first disable
            pAdvancer->unlink(this);
        }
        enabled = en;
    };
    bool isEnabled(){ return enabled; }


    friend class IsAdvancer; // allow advancer to update
};

#endif // _IS_ELAPSED_TIME_H_
