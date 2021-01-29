 /* EpCommon.cpp
  *
  * Licence: See "LICENSE.txt"
  *
  * Author: James Jensen (Mechtro LLC)
 *         tiagojen@gmail.com
  *
  */

#include "EpCommon.h"

IsAdvancer advancer;

uint8_t rawPrBuf[PR_BUF_SIZE];
IsPrintRingBuffer prBuf(rawPrBuf, PR_BUF_SIZE);
IsBufSerialSeq prBufSeq(prBuf.ring, Serial);
