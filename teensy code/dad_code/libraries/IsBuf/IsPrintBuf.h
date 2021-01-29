/*
 * a print buffer
 *
 * Licence: See "LICENSE.txt"
 *
 * Author: James Jensen (Mechtro LLC)
 *         tiagojen@gmail.com
 *
 */

#ifndef _IS_PRINT_BUF_H_
#define _IS_PRINT_BUF_H_

#include "Print.h"


// =========================================================================
class IsPrintBuf: public Print{

    uint8_t * pRawBuf;
    uint32_t rawSize;
    uint32_t offset;

  public:

    IsPrintBuf(uint8_t * _pRawBuf, uint32_t _rawSize):
        pRawBuf(_pRawBuf),
        rawSize(_rawSize)
    {
        clear();
    }

    void clear(){
        offset = 0;
        terminate();
    }

    void terminate(){
        // set next byte or end byte to 0
        uint32_t endOffset = min(offset, rawSize - 1);
        *(pRawBuf + endOffset) = 0;
    }

    const char * getCharBuf(){
        // make sure end of buf is terminated
        //*(pRawBuf + rawSize - 1) = 0;
        terminate();
        return (const char *)pRawBuf;
    }

    // -------------------------------------------------------------------
    // implement classes used by base class to print
    size_t write(uint8_t b){
        // don't allow to write to last char, so it will
        // stay 0 to terminate str
        if( offset >= rawSize - 2 ) return 0;
        *(pRawBuf + offset) = b;
        offset++;
        return 1;
    }

    size_t write(const uint8_t *buffer, size_t size){

        for(int32_t i=0; i<size; i++){
            if( offset >= rawSize - 2 ) return i;
            *(pRawBuf + offset) = *(buffer+i);
            offset++;
        }
        return size;
    }

    // -------------------------------------------------------------------
    // some print utilities for zero pre-padded hex numbers (i.e. shows precision and value)
    // see IsPrint.cpp for similar code
    void uint16Hex(uint16_t u){
        for( int i=0; i<4; i++){
            uint8_t b = (u & 0xf000) >> 12;
            if( b < 10 ){
                print((char)('0' + b));
            }else{
                print((char)('A' + b - 10));
            }
            u = u << 4;
        }
    }
    void uint32Hex(uint32_t u){
        uint16Hex( u >> 16 );
        print("_"); // put separator between 16bit chunks
        uint16Hex( u & 0xffff );
    }
    void uint64Hex(uint64_t u){
        uint32Hex( u >> 32 );
        print("_"); // put separator between 16bit chunks
        uint32Hex( u & 0xffffffff );
    }

};


#endif // _IS_PRINT_BUF_H_
