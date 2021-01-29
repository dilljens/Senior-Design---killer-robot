/*
 * a print ring buffer
 *
 * Licence: See "LICENSE.txt"
 *
 * Author: James Jensen (Mechtro LLC)
 *         tiagojen@gmail.com
 *
 */

#ifndef _IS_PRINT_RBUF_H_
#define _IS_PRINT_RBUF_H_

#include "print.h"
#include "IsRing.h"

//#define MAX_LINE_SIZE 100  //later

// =========================================================================
class IsPrintRingBuffer: public Print{

  public:
    IsRingBuffer ring;

    IsPrintRingBuffer(uint8_t * _pRawBuf, uint32_t _size):
        ring(_pRawBuf, _size)
    {

    }

    // -------------------------------------------------------------------
    // implement classes used by base class to print
    size_t write(uint8_t b){
        return ring.push(b) ? 1 : 0 ;
    }

    size_t write(const uint8_t *buffer, size_t size){
        if( size == 0 ) return 0;
        bool working = true;
        size_t offset = 0;
        while( working ){
            if( ring.push( *(buffer + offset) ) ){
                offset++;
                if( offset >= size ) working = false;
            }else{
                working = false;
            }
        }
        return offset;
    }

    // -------------------------------------------------------------------
    void prependMs(){
        print("[");
        print(millis());
        print("] ");
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

    // print base 10 uint, without leading zeros (probably not optimal algoritm)
    void uint64(uint64_t u){
        uint64_t remainder = u;
        uint64_t div = 10000000000000000000ULL;
        bool started = false;
        for(int i=19; i>=0; i--){
            uint8_t digit = remainder/div; // int div
            remainder -= digit*div;
            div /= 10;
            if( started || digit != 0 || i==0 ){
                char c = '0' + digit;
                print(c);
                started = true;
            }
        }
    }

};


#endif // _IS_PRINT_RBUF_H_
