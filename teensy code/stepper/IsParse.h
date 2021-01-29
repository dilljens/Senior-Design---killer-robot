/*
 * Utils to parse a string
 *
 * Pass in a ptr to isOk, to null terminated string, and ptr to index into str.
 * Function will advance the index as it searches.
 *
 *
 *  Author: James Jensen (Mechtro LLC)
 *          tiagojen@gmail.com
 *
 */

#ifndef _PARSE_H_
#define _PARSE_H_

#include "Arduino.h"
#include "WCharacter.h"

void skipSpaces(bool * pIsOk, const char * pStr, uint16_t* pCaret);
void skipWhite(bool * pIsOk, const char * pStr, uint16_t* pCaret);

void matchChar(bool * pIsOk, const char * pStr, uint16_t* pCaret, char matchChar);

void extractChar(bool * pIsOk, const char * pStr, uint16_t* pCaret, char * pChar);
void extractToChar(bool * pIsOk, const char * pStr, uint16_t* pCaret, char dividerChar, char* pBuf, uint16_t bufSize);

// match to see if the MasterStr is in the Str
void matchStr(bool * pIsOk, const char * pStr, uint16_t* pCaret, const char * pMasterStr);
// match just the boundry to see that changes from word to non-word
void matchEndWord(bool * pIsOk, const char * pStr, uint16_t* pCaret);
// match to see if the MasterStr is in the Str, followed by an end word boundry
void matchWord(bool * pIsOk, const char * pStr, uint16_t* pCaret, const char * pMasterStr);

void parseUint8(bool * pIsOk, const char * pStr, uint16_t* pCaret, uint8_t * pValue);
void parseUint16(bool * pIsOk, const char * pStr, uint16_t* pCaret, uint16_t * pValue);
void parseUint32(bool * pIsOk, const char * pStr, uint16_t* pCaret, uint32_t * pValue);
void parseHexDigit(bool * pIsOk, const char * pStr, uint16_t* pCaret, uint8_t * pDigit);
void parseHexUint8(bool * pIsOk, const char * pStr, uint16_t* pCaret, uint8_t * pValue);
void parseHexUint16(bool * pIsOk, const char * pStr, uint16_t* pCaret, uint16_t * pValue);
void parseInt32(bool * pIsOk, const char * pStr, uint16_t* pCaret, int32_t * pValue);

// lenient parse to skip separators between parts of num
void parseHexAddr64(bool * pIsOk, const char * pStr, uint16_t* pCaret, uint64_t * pValue);

// parse until end or non-alphaNumeric, copy into parseBuf, limit size, and null terminate
void parseNextWord(bool * pIsOk, const char * pStr, uint16_t* pCaret, char * pParsedBuf, uint32_t bufSize );

#endif // _PARSE_H_
