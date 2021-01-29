/* PinDef.h
 *
 * Licence: See "LICENSE.txt"
 *
 * Author: James Jensen (Mechtro LLC)
 *         tiagojen@gmail.com
 *
 */

#ifndef _PIN_DEF_H_
#define _PIN_DEF_H_

// Pin Definitions For Teensy 3.1 or 3.2

// --------------------------------------------------------------------
// Notes:
// PWM for Teensy 3.1 & 3.2:
//   Resource    Pins
//   FTM0        5, 6, 9, 10, 20, 21, 22, 23
//   FTM1        3, 4
//   FTM2        25, 32
//

// --------------------------------------------------------------------
#define SM1_STEP 3
#define SM1_DIR 4
#define SM1_EN 5

#define HOME_IN_PIN 8 // use internal pull up (High - extended out past home)
#define TOGGLE1_IN_PIN 22 // use internal pull down
#define TOGGLE2_IN_PIN 23 // use internal pull down

#define RED_LED_OUT_PIN 6
#define YELLOW_LED_OUT_PIN 9
#define GREEN_LED_OUT_PIN 10

#define RATIO_APIN A1   //D15, Right Pot
#define VOLUME_APIN A2 //D16, Mid Pot
#define RATE_APIN A3  //D17, Left Pot

#define LED_13 13

#endif // _PIN_DEF_H_
