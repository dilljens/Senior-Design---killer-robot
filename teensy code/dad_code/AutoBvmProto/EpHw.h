/* EpHw.h
 *
 * Licence: See "LICENSE.txt"
 *
 * Author: James Jensen (Mechtro LLC)
 *         tiagojen@gmail.com
 *
 */

#ifndef _EP_HW_H_
#define _EP_HW_H_

//#include "EpIicSeq.h"
#include "EpCommon.h"
#include "EpSvcRunner.h"
#include "ADC.h"

// ====================================================================
class Hw{
    IsElapsedUs32 miniEus; // for short delays
    IsElapsedMs32 debounceHomeEms;
    IsElapsedMs32 debounceToggle1Ems;
    IsElapsedMs32 debounceToggle2Ems;
    IsElapsedMs32 tmpEms;

    // ---------------------------------------------------
    // debug, keep track to show state changes
    uint8_t oldRedLed;
    uint8_t oldYellowLed;
    uint8_t oldGreenLed;

    ADC sysAdc;
    ADC_Module * pAdc;

    uint8_t currentAdc = RATE_APIN;

  public:
    uint8_t  snsHome = 0;
    uint8_t  snsToggle1 = 0;
    uint8_t  snsToggle2 = 0;

    uint8_t stepper1En; // enable

    uint8_t redLed;
    uint8_t yellowLed;
    uint8_t greenLed;

    uint32_t rateSetting;
    uint32_t volumeSetting;
    uint32_t ratioSetting;

    Hw():
        miniEus(advancer),
        debounceHomeEms(advancer),
        debounceToggle1Ems(advancer),
        debounceToggle2Ems(advancer),
        tmpEms(advancer)
    {
      pAdc = sysAdc.adc0; // use this module
      stepper1En = false;
    }

    void nextAnalogChannel(){
      //uint8_t prevAdc = currentAdc;
      uint32_t reading = pAdc->analogReadContinuous();
      switch(currentAdc){
        case RATE_APIN:
          rateSetting = reading;
          currentAdc = VOLUME_APIN;
          break;
        case VOLUME_APIN:
          volumeSetting = reading;
          currentAdc = RATIO_APIN;
          break;
        case RATIO_APIN:
          ratioSetting = reading;
          currentAdc = RATE_APIN;
          break;
        default:
          currentAdc = RATE_APIN; // incase not valid
      }
      pAdc->startContinuous(currentAdc);
    }

    // ---------------------------------------------------
    void earlySetup(){
      pAdc->setAveraging(32); // set number of averages
      pAdc->setResolution(10);
      pAdc->setConversionSpeed(ADC_CONVERSION_SPEED::LOW_SPEED);
      pAdc->setSamplingSpeed(ADC_SAMPLING_SPEED::LOW_SPEED);
      nextAnalogChannel();
    }

    float getTidalVolumeF(){
      return TIDAL_VOLUME_MIN +
        (float)volumeSetting/((float)1024)*(TIDAL_VOLUME_MAX - TIDAL_VOLUME_MIN);
    }
    float get_I_E_RatioF(){
      return I_E_RATIO_MIN +
        (float)ratioSetting/((float)1024)*(I_E_RATIO_MAX - I_E_RATIO_MIN);
    }
    float getBreathsPerMinF(){
      return BREATHS_PER_MINUTE_MIN +
        (float)rateSetting/((float)1024)*(BREATHS_PER_MINUTE_MAX - BREATHS_PER_MINUTE_MIN);
    }

    void eval(){
        // ...............................................
        // write outputs
        digitalWriteFast(RED_LED_OUT_PIN, redLed ? 1 : 0);
        digitalWriteFast(YELLOW_LED_OUT_PIN, yellowLed ? 1 : 0);
        digitalWriteFast(GREEN_LED_OUT_PIN, greenLed ? 1 : 0);

        // stepper is enabled low
        digitalWriteFast(SM1_EN, !stepper1En);

        // ...............................................
        // read analog without blocking
        if( pAdc->isComplete() ){
          // this should match adcPinTable order
          nextAnalogChannel();
        }

        // ...............................................
        // Manage digital input. Debounce raw input
        uint8_t rawSnsHome = digitalRead(HOME_IN_PIN);
        uint8_t rawSnsToggle1 = digitalRead(TOGGLE1_IN_PIN);
        uint8_t rawSnsToggle2 = digitalRead(TOGGLE2_IN_PIN);

        if( snsHome != rawSnsHome ){
          if( debounceHomeEms.elapsed > 50 ){
            snsHome = rawSnsHome;
            prBuf.prependMs();
            prBuf.print("SnsHome: ");
            prBuf.println(snsHome);
            debounceHomeEms.elapsed = 0;
          }
        }else{ // is equal, restart debounce
          debounceHomeEms.elapsed = 0;
        }
        if( snsToggle1 != rawSnsToggle1 ){
          if( debounceToggle1Ems.elapsed > 50 ){
            snsToggle1 = rawSnsToggle1;
            prBuf.prependMs();
            prBuf.print("SnsToggle1: ");
            prBuf.println(snsToggle1);
            debounceToggle1Ems.elapsed = 0;
          }
        }else{ // is equal, restart debounce
          debounceToggle1Ems.elapsed = 0;
        }
        if( snsToggle2 != rawSnsToggle2 ){
          if( debounceToggle2Ems.elapsed > 50 ){
            snsToggle2 = rawSnsToggle2;
            prBuf.prependMs();
            prBuf.print("SnsToggle2: ");
            prBuf.println(snsToggle2);
            debounceToggle2Ems.elapsed = 0;
          }
        }else{ // is equal, restart debounce
          debounceToggle2Ems.elapsed = 0;
        }

    }
};

extern Hw hw;


#endif // _EP_HW_H_
