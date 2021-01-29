/*
 * Used to hookup a buffer to a serial
 *
 * Licence: See "LICENSE.txt"
 *
 * Author: James Jensen (Mechtro LLC)
 *         tiagojen@gmail.com
 *
 */

#ifndef _IS_BUF_SERIAL_SEQ_H_
#define _IS_BUF_SERIAL_SEQ_H_

#include "IsRing.h"
#include "IsPrintRBuf.h"

// =========================================================================
class IsBufSerialSeq{

    IsRingBuffer & ringBuffer;

    // unfortunate hack, since each of these classes has common method, but not a common superclass with
    // that method
    HardwareSerial * pHwSerial;
    usb_serial_class * pUsbSerial;


  public:

    IsBufSerialSeq(IsRingBuffer & _ringBuffer, HardwareSerial & _serial ):
        ringBuffer(_ringBuffer),
        pHwSerial(&_serial),
        pUsbSerial(NULL)
    {
    }

    IsBufSerialSeq(IsRingBuffer & _ringBuffer, usb_serial_class & _serial ):
        ringBuffer(_ringBuffer),
        pHwSerial(NULL),
        pUsbSerial(&_serial)
    {
    }

    void eval(){
        size_t room;
        if( pHwSerial ){
            room = pHwSerial->availableForWrite();
        }else{
            room = pUsbSerial->availableForWrite();
        }
        if( room > 0 ){
            size_t filled = ringBuffer.filled();
            size_t xfer = min(filled, room);
            for(size_t i=0; i<xfer; i++){
                uint8_t b = 0;
                ringBuffer.pop(&b);
                if( pHwSerial ){
                    pHwSerial->print((char)b);
                }else{
                    pUsbSerial->print((char)b);
                }
            }
        }
    }

};


#endif // _IS_BUF_SERIAL_SEQ_H_
