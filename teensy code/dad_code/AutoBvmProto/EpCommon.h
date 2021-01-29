/* EpCommon.h - Some project wide definitions
 *
 * Licence: See "LICENSE.txt"
 *
 * Author: James Jensen (Mechtro LLC)
 *         tiagojen@gmail.com
 *
 */

#ifndef _EP_COMMON_H_
#define _EP_COMMON_H_

#include "IsElapsedTime.h"

#include "IsRing.h"
#include "IsPrintRBuf.h"
#include "IsBufSerialSeq.h"

#include "PinDef.h"

// --------------------------------------------------------------------
#define MAJOR_VER "01"
#define MINOR_VER "01"

// --------------------------------------------------------------------
#define MAX_UINT32 0xFFFFFFFF

#define MAX_INT32 0x7FFFFFFF
#define MIN_INT32 0x80000000

#define MS_PER_SEC 1000
#define SECS_PER_MIN 60
#define MS_PER_MIN (1000 * SECS_PER_MIN)
#define USECS_PER_SEC 1000000

// ............................................................
#define MICRO_STEPS_PER_REV 1600  // drive setting
//#define MICRO_STEPS_PER_REV 3200  // drive setting

// logically subdivide micro steps up even more to allow int accum to have precision
// mimimal unit is called a 'baby step'
//                                 ,  ,  ,
#define BABY_STEPS_PER_MICRO_STEP 1000000000ull
#define BABY_STEPS_PER_REV ((uint64_t)MICRO_STEPS_PER_REV * BABY_STEPS_PER_MICRO_STEP)
// a small sub microStep nudge
#define MICRO_STEP_NUDGE_REVS_F (.25f/((float)MICRO_STEPS_PER_REV))

// define time constants
#define TICS_PER_SEC 20000 // Rate of stepper pulse generator
#define ISR_TIMER_FREQ (TICS_PER_SEC)
#define MSEC_PER_SEC 1000
#define TICS_PER_MSEC (TICS_PER_SEC/MSEC_PER_SEC)
#define USECS_PER_MSEC 1000
#define USECS_PER_SEC 1000000
#define USECS_PER_TIC (USECS_PER_SEC/TICS_PER_SEC)

// ............................................................
// positions relative to coord system with zero at home sensor, and extension posative
#define FULL_EXTEND_POS_REVS 3.0f// Just short of physical motion limit XX TBD
#define IDLE_POS_REVS -.3f // XX TBD
#define IDLE_POS_MICRO_STEPS ((int32_t)(IDLE_POS_REVS * MICRO_STEPS_PER_REV)) // XX TBD
#define FULL_RETRACT_POS_REVS -1.0f// Just short of physical motion limit XX TBD
#define UNUSED_EXTEND .5f // last part of extension that is not used

// When Tidal Volume set to max
#define MAX_EXTEND_FROM_IDLE (FULL_EXTEND_POS_REVS - IDLE_POS_REVS - UNUSED_EXTEND)

// ............................................................
// home constants
#define HOME_VEL_RPS 1.0f
#define HOME_ACC_RPSS 2000.0f
#define HOME_EXTEND_DIST_REV 1.0f
#define HOME_RETRACT_DIST_REV -3.3f

// ............................................................
// breath constants
#define TIDAL_VOLUME_MIN .2f // relative to max travel 1.0
#define TIDAL_VOLUME_MAX .95f

#define I_E_RATIO_MIN .25f
#define I_E_RATIO_MAX 2.0f

#define BREATHS_PER_MINUTE_MIN 10.0f
#define BREATHS_PER_MINUTE_MAX 30.0f

// amount the inhale stroke slows down by the time it is done. (For max stroke)
#define INHALE_REDUCTION_FACTOR .7f

// for exhale, retract faster then wait idle (always keep just a bit, to allow for error correction repo)
#define EXHALE_IDLE_RATIO .3f

// ............................................................
// used when returning a char ptr err code from function
#define NO_ERR "No Err"

// ====================================================================
// this advances all of the time tracking objects
extern IsAdvancer advancer;

// --------------------------------------------------------------------
// support for buffered printing (to avoid blocking)
// this allows the use of: prBufSeq.print(' ');
// instead: of Serial.print(' ');
#define PR_BUF_SIZE 2048
extern uint8_t rawPrBuf[];
extern IsPrintRingBuffer prBuf;
extern IsBufSerialSeq prBufSeq;

#endif // _EP_COMMON_H_
