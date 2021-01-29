/* AutoBvmProto.ino - AutoBVM prototype demonstration
 *
 * Licence: See "LICENSE.txt"
 *
 * Author: James Jensen (Mechtro LLC)
 *         tiagojen@gmail.com
 *
 */


/*
*/
#include "IsElapsedTime.h"
#include "IsStateMachine.h"
#include "IsSerialInSeq.h"
#include "IsParse.h"
#include "IsPrint.h"
#include "IsRing.h"
#include "IsPrintRBuf.h"
#include "IsBufSerialSeq.h"

#include "EpCommon.h"
#include "EpMasterSeq.h"

// the setup routine runs once when you press reset:
void setup() {
  pinMode(13, OUTPUT); // show that it started
  digitalWriteFast(13, 1);

  masterSeq.earlySetup();

  masterSeq.setup();
}

// the loop routine runs over and over again forever:
void loop() {

  advancer.advance();

  masterSeq.eval();

  prBufSeq.eval();

}
