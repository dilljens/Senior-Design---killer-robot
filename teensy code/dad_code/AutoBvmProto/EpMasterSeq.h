/* EpMasterSeq.h
 *
 * Licence: See "LICENSE.txt"
 *
 * Author: James Jensen (Mechtro LLC)
 *         tiagojen@gmail.com
 *
 */

#ifndef _EP_MASTER_SEQ_H_
#define _EP_MASTER_SEQ_H_

#include "IsElapsedTime.h"
#include "IsStateMachine.h"
#include "IsSerialInSeq.h"
#include "IsParse.h"
#include "IsPrint.h"

#include "EpCommon.h"
#include "EpHw.h"
#include "EpSvcRunner.h"
#include "ProfRepos.h"
#include "ProfInhale.h"
#include "Stepper.h"
#include "SvcHome.h"
#include "SvcBreath.h"

// =========================================================================
class MasterSeq: public IsSeq{
    IsElapsedMs32 delayEms;
    IsElapsedMs32 tickEms;
    IsElapsedMs32 potDebugEms;
    IsElapsedMs32 tmpEms;
    IsElapsedMs32 timeoutEms;
    IsElapsedUs32 evalCycleEus; // measure performance

    uint32_t maxCycleTimeUs;

    IsSerialInSeq inSeq;

    uint32_t oldRate;
    uint32_t oldVolume;
    uint32_t oldRatio;

    ProfRepos consoleProfRepos; //used by console
    float consoleVelRps;
    float consoleAccRpss;

    ProfInhale consoleProfInhale;

    // --------- New Svc Support -------------
    SvcRunner svcRunner;

    SvcHome svcHome;
    SvcBreath svcBreath;

    // .........
    uint8_t aliveLed;

    uint32_t tickCnt = 0;

    uint32_t startupCountDown;

    uint32_t oldBufCnt = 0;

    void parse(); // defined in EpMasterSeqParse.cpp

    const char * setErr(const char * pErr){
        prBuf.prependMs();
        prBuf.println(pErr);
        err(pErr); // set err in seq
        return pErr;
    }

  public:
    MasterSeq():
        delayEms(advancer),
        tickEms(advancer),
        potDebugEms(advancer),
        tmpEms(advancer),
        timeoutEms(advancer),
        evalCycleEus(advancer),
        inSeq(&Serial)
    {
        prBuf.prependMs();
        prBuf.println("MasterSeq Constr");
        pLabel = "Master";
        aliveLed = 0;
        startupCountDown = 3;
        randomSeed(analogRead(20)); // underside pad
        consoleVelRps = 1.0f;
        consoleAccRpss = 4.0f;
        state = 0;
    }

    bool isReadyForCmd(){
        return state == 900;
    }

    void earlySetup(){
        //prBuf.prependMs();
        //prBuf.println("MasterSeq earlySetup");

        pinMode(LED_13, OUTPUT);
        digitalWriteFast(LED_13, LOW);

        pinMode(HOME_IN_PIN, INPUT_PULLUP);
        pinMode(TOGGLE1_IN_PIN, INPUT_PULLDOWN);
        pinMode(TOGGLE2_IN_PIN, INPUT_PULLDOWN);

        pinMode(SM1_STEP, OUTPUT);
        pinMode(SM1_DIR, OUTPUT);
        pinMode(SM1_EN, OUTPUT);
        digitalWriteFast(SM1_STEP, LOW);
        digitalWriteFast(SM1_DIR, LOW);
        digitalWriteFast(SM1_EN, HIGH); // disable, is active low

        pinMode(RED_LED_OUT_PIN, OUTPUT);
        pinMode(YELLOW_LED_OUT_PIN, OUTPUT);
        pinMode(GREEN_LED_OUT_PIN, OUTPUT);
        digitalWriteFast(RED_LED_OUT_PIN, LOW);
        digitalWriteFast(YELLOW_LED_OUT_PIN, LOW);
        digitalWriteFast(GREEN_LED_OUT_PIN, LOW);

        pinMode(RATE_APIN, INPUT);
        pinMode(VOLUME_APIN, INPUT);
        pinMode(RATIO_APIN, INPUT);

        hw.earlySetup();

    }

    void setup(){
        prBuf.prependMs();
        prBuf.println("MasterSeq setup");
    }

    bool initDone = false; // make init only happen once

    // --------------------------------------------------------------------
    void eval(){
        if( tmpEms.elapsed >= 1000){
            tmpEms.elapsed = 0;
            if( startupCountDown > 0 ){
                prBuf.prependMs();
                prBuf.print("Count = ");
                prBuf.println(startupCountDown);
                startupCountDown --;
            }else{
                if( !initDone ){
                    initDone = true;
                    // put late one time init here...
                    prBuf.prependMs();
                    prBuf.println("Starting ISR");
                    intervalTimer.begin( intervalTimerFun, USECS_PER_SEC/ISR_TIMER_FREQ); // microseconds

                    state = 900; // start init seq

                    // reset tracker vars
                    maxCycleTimeUs = 0;
                    evalCycleEus.elapsed = 0;
                    delayEms.elapsed = 0;
                    prBuf.prependMs();
                    prBuf.println("Ready, type '?' to see menu.");
                }
                // prBuf.print("IsrCnt=");
                // prBuf.print(isrCnt);

                //isrCnt = 0;
            }
            aliveLed = !aliveLed;
            digitalWrite(LED_13, aliveLed);
        }

        // wait until start, so term can hookup before msgs
        if( startupCountDown > 0 ){
            return;
        }

        // measure loop speed
        maxCycleTimeUs = (evalCycleEus.elapsed > maxCycleTimeUs )? evalCycleEus.elapsed: maxCycleTimeUs;
        evalCycleEus.elapsed = 0;

        // console input
        inSeq.eval();

        if(inSeq.gotCmd){
          //prBuf.print("  >> got input: '");
          //prBuf.print(inSeq.buf);
          //prBuf.println("'");
          //prBuf.println("");
          parse();
        }

        hw.eval();

        switch(state){
            case 900: // default idle state
                break;

        }

        //evalStateDebug();
        if( tickEms.elapsed >= 10000){
          tickEms.elapsed = 0;
          prBuf.prependMs();
          prBuf.print("tick ");
          prBuf.println(tickCnt);
          tickCnt++;
        }
        if( potDebugEms.elapsed >= 50){
          potDebugEms.elapsed = 0;
          // check if changed more than threshold
          uint32_t threshold = 10;
          if( abs(oldRate - hw.rateSetting) > threshold ){
            oldRate = hw.rateSetting;
            prBuf.print("Rate(");
            prBuf.print(hw.rateSetting);
            prBuf.print(") bpm(");
            prBuf.print(hw.getBreathsPerMinF());
            prBuf.println(")");
          }
          if( abs(oldVolume - hw.volumeSetting) > threshold ){
            oldVolume = hw.volumeSetting;
            prBuf.print("Volume(");
            prBuf.print(hw.volumeSetting);
            prBuf.print(") tv(");
            prBuf.print(hw.getTidalVolumeF());
            prBuf.println(")");
          }
          if( abs(oldRatio - hw.ratioSetting) > threshold ){
            oldRatio = hw.ratioSetting;
            prBuf.print("Ratio(");
            prBuf.print(hw.ratioSetting);
            prBuf.print(") ie(");
            prBuf.print(hw.get_I_E_RatioF());
            prBuf.println(")");
          }
        }

        // Debug
        // watch stepper for instruction events
        // grab them with interrupts disabled so they can't change
        // while accessing
        ProfileInstr * pStarted0;
        ProfileInstr * pStarted1;
        ProfileInstr * pDone0;
        ProfileInstr * pDone1;
        __disable_irq();
        // grab them
        pStarted0 = stepper1.lastInstrLoaded[0];
        pStarted1 = stepper1.lastInstrLoaded[1];
        pDone0 = stepper1.lastInstrCompleted[0];
        pDone1 = stepper1.lastInstrCompleted[1];
        // mark as handled
        pStarted0 = NULL;
        pStarted1 = NULL;
        pDone0 = NULL;
        pDone1 = NULL;
        __enable_irq();
        // handle
        if( pStarted1 ){ // 1 is older if exists
          prBuf.prependMs();
          prBuf.print("Start ");
          pStarted0->dump();
        }
        if( pStarted0 ){
          prBuf.prependMs();
          prBuf.print("Start ");
          pStarted0->dump();
        }
        if( pDone1 ){
          prBuf.prependMs();
          prBuf.print("End ");
          pDone1->dump();
        }
        if( pDone0 ){
          prBuf.prependMs();
          prBuf.print("End ");
          pDone0->dump();
        }

        // ...............................................
        // run any generic services
        svcRunner.eval();
    }

};

extern MasterSeq masterSeq; // instance in top .ino file

#endif // _EP_MASTER_SEQ_H_
