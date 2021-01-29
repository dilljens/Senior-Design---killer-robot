/*
*  Licence: See "LICENSE.txt"
*
*  Author: James Jensen (Mechtro LLC)
*          tiagojen@gmail.com
*
*/

#include "Arduino.h"

#ifndef _IS_PRINT_H_
#define _IS_PRINT_H_


// ========================================================
// Alternate print for hex numbers, include leading zero
// ========================================================
void printHex8(uint8_t u);
void printlnHex8(uint8_t u);
void printHex16(uint16_t u);
void printlnHex16(uint16_t u);
void printHex32(uint32_t u);
void printlnHex32(uint32_t u);
void printHex64(uint64_t u);
void printlnHex64(uint64_t u);
void printBin(int v, int num_places);
void printlnBin(int v, int num_places);

// ========================================================
// print to buffer
// ========================================================
void bufPrintUint32(char * pBuf, uint16_t * pCaret, uint16_t bufSize, uint32_t u);
void bufPrintInt32(char * pBuf, uint16_t * pCaret, uint16_t bufSize, int32_t i);

void bufPrintUint8(char * pBuf, uint16_t * pCaret, uint16_t bufSize, uint8_t u);
void bufPrintInt8(char * pBuf, uint16_t * pCaret, uint16_t bufSize, int8_t i);

void bufPrintChar(char * pBuf, uint16_t * pCaret, uint16_t bufSize, char c);
void bufPrintStr(char * pBuf, uint16_t * pCaret, uint16_t bufSize, char * pStr);
void bufTerminateString(char * pBuf, uint16_t * pCaret, uint16_t bufSize);

#endif //_IS_PRINT_H_
