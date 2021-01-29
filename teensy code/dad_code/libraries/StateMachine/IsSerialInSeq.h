/*
 * A state machine to accumulate a line of characters and set a flag
 * when the whole "cmd" is ready
 * Licence: See "LICENSE.txt"
 *
 * Author: James Jensen (Mechtro LLC)
 *         tiagojen@gmail.com
 *
 */

#ifndef _IS_SERIAL_IN_SEQ_H_
#define _IS_SERIAL_IN_SEQ_H_
#include "IsStateMachine.h"

// =========================================================================
#define IS_CONSOLE_IN_BUF_SIZE 256

class IsSerialInSeq: public IsSeq{
  public:

    char buf[IS_CONSOLE_IN_BUF_SIZE];
    uint16_t bufCnt;
    uint32_t evalCnt;
    Stream *pSerial;

    IsSerialInSeq(){
        init(&Serial);
    }
    IsSerialInSeq(Stream * _pSerial){
        init(_pSerial);
    }
    void init(Stream * _pSerial){
        pSerial = _pSerial;
        bufCnt = 0;
        gotCmd = false;
        evalCnt = 0;
    }

    const char * getBuf(){ return buf; };
    uint16_t getBufCount(){ return bufCnt; };

    bool gotCmd;

    void clearCmd(){
        gotCmd = false;
        bufCnt = 0;
    }
    // override
    void eval(){
        evalCnt++;
        if( gotCmd == true ){  // reset from last time if needed
            clearCmd();
        }
        if( pSerial->available() ){
            char c = pSerial->read();

            if( c == 13 || c == 10 ){ //CR or NL, skip
                buf[bufCnt] = 0; // terminate str
                if( bufCnt > 0 ){
                  gotCmd = true;
                }
            }else{
                // leave one spot on end of buf for terminator
                if( bufCnt < (IS_CONSOLE_IN_BUF_SIZE - 1) ){
                    buf[bufCnt++] = c;
                }
            }
        }
    }
};


#endif // _SERIAL_IN_SEQ_H_
