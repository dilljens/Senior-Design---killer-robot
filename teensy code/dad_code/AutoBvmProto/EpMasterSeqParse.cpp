/* EpMasterSeqParse.cpp
 *
 * Licence: See "LICENSE.txt"
 *
 * Author: James Jensen (Mechtro LLC)
 *         tiagojen@gmail.com
 *
 */

#include "EpMasterSeq.h"

// --------------------------------------------------------------------
void MasterSeq::parse(){
  bool isOk = true;
  uint16_t caret = 0;
  const char * pCmdStr = inSeq.getBuf();
  while(true){ // context to break from
    bool tmpOk = isOk;
    uint16_t tmpCaret = caret;

    // ..............................................
    tmpOk = isOk;
    tmpCaret = caret;
    matchWord(&tmpOk, pCmdStr, &tmpCaret, "qs"); //query status
    if(tmpOk){ // found match
        // handle cmd
        prBuf.println();
        prBuf.println("Status:");

        prBuf.print("  Max Loop Latency = ");
        prBuf.print( maxCycleTimeUs );
        prBuf.println(" us");

        prBuf.print("  Stepper isrCnt = ");
        prBuf.println( isrCnt);

        prBuf.print("  Stepper maxIsrDur = ");
        prBuf.print( maxIsrDur );
        prBuf.println(" us");

        prBuf.print("  pos = ");
        prBuf.println( stepper1.position);
        prBuf.print("  % of max step vel = "); // relative to max
        prBuf.println( (float)stepper1.maxStepVel/(float)BABY_STEPS_PER_MICRO_STEP);
        prBuf.print("XX _elapsedTics = ");
        prBuf.println( stepper1._elapsedTics);

        prBuf.println();
        break;
    }

    tmpOk = isOk;
    tmpCaret = caret;
    matchWord(&tmpOk, pCmdStr, &tmpCaret, "rs"); // reset status
    if(tmpOk){ // found match
        // handle cmd
        maxCycleTimeUs = 0;
        maxIsrDur = 0;
        stepper1.maxStepVel = 0;
        break;
    }

    // ..............................................
    tmpOk = isOk;  // Motor Enable
    tmpCaret = caret;
    matchWord(&tmpOk, pCmdStr, &tmpCaret, "me");
    if(tmpOk){ // found match
        skipSpaces(&tmpOk, pCmdStr, &tmpCaret);
        uint8_t enArg = 0;
        parseUint8(&tmpOk, pCmdStr, &tmpCaret, &enArg);
        // arg=1 enable, which is 0 for motor drv
        if(tmpOk){
            prBuf.print("Motor enable = ");
            prBuf.println(enArg);
            hw.stepper1En = enArg;
        }else{
            prBuf.println("Missing or bad enable [1 or 0] arg");
        }
        break;
    }


    // ..............................................
    tmpOk = isOk;
    tmpCaret = caret;
    matchWord(&tmpOk, pCmdStr, &tmpCaret, "svel");  // set vel in RPS/100
    if(tmpOk){ // found match
        skipSpaces(&tmpOk, pCmdStr, &tmpCaret);
        uint32_t vel = 0;
        parseUint32(&tmpOk, pCmdStr, &tmpCaret, &vel);
        // arg=1 enable, which is 0 for motor drv
        if(tmpOk){
            prBuf.println("Got 'Stepper Vel' cmd");
            consoleVelRps = vel/100.0f;
            /*
            if( vel < 1 || vel > 350 ){
                prBuf.println("Stepper Vel >= 1, <= 350");
                break;
            }
            */
        }else{
            prBuf.println("Bad Syntax");
        }
        break;
    }

    // ..............................................
    tmpOk = isOk;
    tmpCaret = caret;
    matchWord(&tmpOk, pCmdStr, &tmpCaret, "sacc");  // set vel in RPSS/100
    if(tmpOk){ // found match
        skipSpaces(&tmpOk, pCmdStr, &tmpCaret);
        uint32_t acc = 0;
        parseUint32(&tmpOk, pCmdStr, &tmpCaret, &acc);
        // arg=1 enable, which is 0 for motor drv
        if(tmpOk){
            prBuf.println("Got 'Stepper Acc' cmd");
            consoleAccRpss = acc/100.0f;
        }else{
            prBuf.println("Bad Syntax");
        }
        break;
    }
    // ..............................................
    tmpOk = isOk;
    tmpCaret = caret;
    matchWord(&tmpOk, pCmdStr, &tmpCaret, "mv");  // move stepper
    if(tmpOk){ // found match
        skipSpaces(&tmpOk, pCmdStr, &tmpCaret);
        int32_t delta = 0;
        parseInt32(&tmpOk, pCmdStr, &tmpCaret, &delta);
        // arg=1 enable, which is 0 for motor drv
        if(tmpOk){
            prBuf.println("Got 'Stepper Move' cmd");
            if( stepper1.isActive() ){
              prBuf.println("! Error, stepper is already active.");
              break;
            }

            //stepper1.position = 0; // reset counter
            prBuf.print("XX ");
            prBuf.println(delta/100.0f);
            consoleProfRepos.initWithDistVelAcc(delta/100.0f, consoleVelRps, consoleAccRpss);
            //consoleProfRepos.initWithDistTime(delta/100.0f, 1);
            consoleProfRepos.dump();
            stepper1.startProfile(consoleProfRepos);

        }else{
            prBuf.println("Bad Syntax");
        }
        break;
    }

    // ..............................................
    tmpOk = isOk;
    tmpCaret = caret;
    matchWord(&tmpOk, pCmdStr, &tmpCaret, "hm");  // run homing alg
    if(tmpOk){ // found match
      prBuf.println("Got 'Home' cmd");
      if( stepper1.isActive() ){
        prBuf.println("! Error, stepper is already active.");
        break;
      }
      if( svcHome.isActive() ){
        prBuf.println("! Error, svcHome is already active.");
        break;
      }
      svcHome.reset();
      svcRunner.start(svcHome);
      break;
    }

    // ..............................................
    tmpOk = isOk;
    tmpCaret = caret;
    matchWord(&tmpOk, pCmdStr, &tmpCaret, "in");  // inhale
    if(tmpOk){ // found match
      prBuf.println("Got 'inhale' cmd");
      if( stepper1.isActive() ){
        prBuf.println("! Error, stepper is already active.");
        break;
      }
      float reductionFactor = .5f;
      consoleProfInhale.init(MAX_EXTEND_FROM_IDLE*1.0f, 0.4f, reductionFactor);
      consoleProfInhale.dump();
      stepper1.startProfile(consoleProfInhale);
      break;
    }

    // ..............................................
    tmpOk = isOk;
    tmpCaret = caret;
    matchWord(&tmpOk, pCmdStr, &tmpCaret, "br");  // run breathing
    if(tmpOk){ // found match
      skipSpaces(&tmpOk, pCmdStr, &tmpCaret);
      uint8_t enArg = 0;
      parseUint8(&tmpOk, pCmdStr, &tmpCaret, &enArg);
      if(tmpOk){
        prBuf.println("Got 'Breath' cmd");
        if( enArg ){
          if( stepper1.isActive() ){
            prBuf.println("! Error, stepper is already active.");
            break;
          }
          if( svcBreath.isActive() ){
            prBuf.println("! Error, svcBreath is already active.");
            break;
          }
          svcBreath.reset();
          svcBreath.continuous = true;
          svcRunner.start(svcBreath);
        }else{
          svcBreath.continuous = false;
        }
      }else{
        prBuf.println("Bad Syntax");
      }
      break;
    }


    // ..............................................
    tmpOk = isOk;
    tmpCaret = caret;
    matchStr(&tmpOk, pCmdStr, &tmpCaret, "?");
    if(tmpOk){ // found match
      caret = tmpCaret; // advance past match
      // handle cmd
      prBuf.println("--------------------------------");
      prBuf.print("Firmware for: AutoBVM V");
      prBuf.print(MAJOR_VER);
      prBuf.print(".");
      prBuf.print(MINOR_VER);
      prBuf.println();
      prBuf.println("HW setup:");
      prBuf.print("  MICRO_STEPS_PER_REV = ");
      prBuf.println(MICRO_STEPS_PER_REV);

      prBuf.println("--------------------------------");
      prBuf.println();
      prBuf.println("Menu: ");
      prBuf.println("   ? - Show this menu");
      // prBuf.println("   qrc - query radio config");
      prBuf.println("   qs - query status");
      prBuf.println("   rs - reset status stats");
      prBuf.println("   me [1 0] - motor enable");
      prBuf.println("   svel [vel 1/100 RPS] - default stepper vel");
      prBuf.println("   sacc [acc 1/100 RPSS] - default stepper acc");
      prBuf.println("   mv [delta 1/100 Rev] - move delta");
      prBuf.println("   hm - start homing svc");
      prBuf.println("   in - start inhale motion");
      prBuf.println("   br [1 0] - start/stop breath svc");

      prBuf.println();
      prBuf.println();

      break;
    }

    break; // always break at end of while context
  } // end context to break from
}
