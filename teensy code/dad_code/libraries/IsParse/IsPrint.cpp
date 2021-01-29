/*
*  Licence: See "LICENSE.txt"
*
*  Author: James Jensen (Mechtro LLC)
*          tiagojen@gmail.com
*
 */

#include "IsPrint.h"

void printlnHex8(uint8_t u){
    printHex8(u);
    Serial.println();
}
void printHex8(uint8_t u){
    for( int i=0; i<2; i++){
        uint8_t b = (u & 0xf0) >> 4;
        if( b < 10 ){
            Serial.print((char)('0' + b));
        }else{
            Serial.print((char)('A' + b - 10));
        }
        u = u << 4;
    }
}
void printlnHex16(uint16_t u){
    printHex16(u);
    Serial.println();
}
void printHex16(uint16_t u){
    for( int i=0; i<4; i++){
        uint8_t b = (u & 0xf000) >> 12;
        if( b < 10 ){
            Serial.print((char)('0' + b));
        }else{
            Serial.print((char)('A' + b - 10));
        }
        u = u << 4;
    }
}
void printlnHex32(uint32_t u){
    printHex32(u);
    Serial.println();
}
void printHex32(uint32_t u){
    /*
    Serial.print("  XX hex32 ");
    Serial.println(u, HEX);
    Serial.println(u>>16, HEX);
    Serial.println(u & 0xffff, HEX);
    */

    printHex16( u >> 16 );
    Serial.print("_"); // put separator between 16bit chunks
    printHex16( u & 0xffff );
}
void printlnHex64(uint64_t u){
    printHex64(u);
    Serial.println();
}
void printHex64(uint64_t u){
    printHex32( u >> 32 );
    Serial.print("_"); // put separator between 16bit chunks
    printHex32( u & 0xffffffff );
}

void printlnBin(int v, int num_places){
    printBin(v, num_places);
    Serial.println();
}
void printBin(int v, int num_places){
    int mask=0, n;

    for (n=1; n<=num_places; n++){
        mask = (mask << 1) | 0x0001;
    }
    v = v & mask;  // truncate v to specified number of places

    while(num_places){
        if (v & (0x0001 << (num_places-1))){
             Serial.print("1");
        }
        else{
             Serial.print("0");
        }
        --num_places;
        if(((num_places%4) == 0) && (num_places != 0)){
            Serial.print("_");
        }
    }
}
// =================================================================
// Print to buffer
// =================================================================
// not optimal algorithms, but sufficent

// print base 10 uint, without leading zeros
void bufPrintUint32(char * pBuf, uint16_t * pCaret, uint16_t bufSize, uint32_t u){
    uint32_t remainder = u;
    uint32_t div = 1000000000;
    bool started = false;
    for(int i=9; i>=0; i--){
        if( *pCaret >= bufSize ) return; // nada
        uint8_t digit = remainder/div; // int div
        remainder -= digit*div;
        div /= 10;
        if( started || digit != 0 || i==0 ){
            char c = '0' + digit;
            *( pBuf + (*pCaret)) = c;
            (*pCaret) ++;
            started = true;
        }
    }
}

void bufPrintInt32(char * pBuf, uint16_t * pCaret, uint16_t bufSize, int32_t i){
    if( i < 0 ){
        bufPrintChar(pBuf, pCaret, bufSize, '-');
        bufPrintUint32(pBuf, pCaret, bufSize, (uint32_t)(-i));
    }else{
        bufPrintUint32(pBuf, pCaret, bufSize, (uint32_t)(i));
    }
}

void bufPrintUint8(char * pBuf, uint16_t * pCaret, uint16_t bufSize, uint8_t u){
    uint8_t remainder = u;
    uint8_t div = 100;
    bool started = false;
    for(int i=2; i>=0; i--){
        if( *pCaret >= bufSize ) return; // nada
        uint8_t digit = remainder/div; // int div
        remainder -= digit*div;
        div /= 10;
        if( started || digit != 0 || i==0 ){
            char c = '0' + digit;
            *( pBuf + (*pCaret)) = c;
            (*pCaret) ++;
            started = true;
        }
    }
}

void bufPrintInt8(char * pBuf, uint16_t * pCaret, uint16_t bufSize, int8_t i){
    if( i < 0 ){
        bufPrintChar(pBuf, pCaret, bufSize, '-');
        bufPrintUint8(pBuf, pCaret, bufSize, (uint8_t)(-i));
    }else{
        bufPrintUint8(pBuf, pCaret, bufSize, (uint8_t)(i));
    }
}

void bufPrintChar(char * pBuf, uint16_t * pCaret, uint16_t bufSize, char c){
    if( *pCaret >= bufSize ) return; // nada
    *( pBuf + (*pCaret)) = c;
    (*pCaret) ++;
}

void bufPrintStr(char * pBuf, uint16_t * pCaret, uint16_t bufSize, char * pStr){
    uint16_t index = 0;
    while(true){
        if( *pCaret >= bufSize ) return;
        char c = *(pStr + index);
        if( c == 0 ) return; // end of string
        *( pBuf + (*pCaret)) = c;
        (*pCaret) ++;
        index ++;
    }
}

// adds the null termination at the end of a string
void bufTerminateString(char * pBuf, uint16_t * pCaret, uint16_t bufSize){
    uint16_t index = min(*pCaret, bufSize);
    *( pBuf + index) = 0;
    if( *pCaret == index ) (*pCaret) ++;
}
