/*
 * A wrapper to a print buffer to add Json content to the buffer
 *
 * Licence: See "LICENSE.txt"
 *
 * Author: James Jensen (Mechtro LLC)
 *         tiagojen@gmail.com
 *
 */

#include "Time.h"

#include "IsPrintBuf.h"

#ifndef _CO_JBUF_H_
#define _CO_JBUF_H_

#define TIME_ENTRY_BUF_SIZE 40

// --------------------------------------------------------------------
extern IsAdvancer advancer;

// ====================================================================
class IsJBuf{

    IsPrintBuf * pPb;

  public:

    IsJBuf( IsPrintBuf * pIsPrintBuf ):
        pPb( pIsPrintBuf)
    {
    }

    // --------------------------------------------------------------------

    void clear(){
        pPb->clear();
    }

    // copy contents of jbuf to end of this jbuf
    void append(IsJBuf * pContentJBuf){
        pPb->print( pContentJBuf->getCharBuf() );
    }

    void terminate(){
        pPb->terminate();
    }

    const char * getCharBuf(){
        terminate();
        return pPb->getCharBuf();
    }

    // --------------------------------------------

    void startObj(){ // start obj
        pPb->print("{");
    }
    void endObj(){ // end obj
        pPb->print("}");
    }

    // --------------------------------------------
    void key(const char * nameStr){ // keyname and :
        pPb->print("\'");
        pPb->print(nameStr);
        pPb->print("\'");
        pPb->print(":");
    }

    void sep(){ // separator
        pPb->print(",");
    }

    // --------------------------------------------
    void strVal(const char * valStr){
        pPb->print("\'");
        pPb->print(valStr);
        pPb->print("\'");
    }
    void int32Val(int32_t val){
        pPb->print(val);
    }
    void uint32Val(uint32_t val){
        pPb->print(val);
    }

    void uint16HexVal(uint16_t u){
        pPb->uint16Hex(u);
        /*
        for( int i=0; i<4; i++){
            uint8_t b = (u & 0xf000) >> 12;
            if( b < 10 ){
                pPb->print((char)('0' + b));
            }else{
                pPb->print((char)('A' + b - 10));
            }
            u = u << 4;
        }
        */
    }
    void uint32HexVal(uint32_t u){
        pPb->uint32Hex(u);
        /*
        uint16HexVal( u >> 16 );
        pPb->print("_"); // put separator between 16bit chunks
        uint16HexVal( u & 0xffff );
        */
    }
    void uint64HexVal(uint64_t u){
        pPb->uint64Hex(u);
        /*
        uint32HexVal( u >> 32 );
        pPb->print("_"); // put separator between 16bit chunks
        uint32HexVal( u & 0xffffffff );
        */
    }


    // add an time entry that starts with year and works down in precision
    // with space separated values
    void dateTimeVal(time_t t){
        pPb->print("\'");
        pPb->print( max(0, year(t)-2000) );
        pPb->print(" ");
        pPb->print( month(t) );
        pPb->print(" ");
        pPb->print( day(t) );
        pPb->print(" ");
        pPb->print( hour(t) );
        pPb->print(" ");
        pPb->print( minute(t) );
        pPb->print(" ");
        pPb->print( second(t) );
        pPb->print("\'");
    }

    void endLine(){
        pPb->print("\r\n");
    }


};

#endif // _CO_JBUF_H_
