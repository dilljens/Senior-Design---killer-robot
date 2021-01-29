/*
 * Utils to parse a string
 *
 *
 *  Author: James Jensen (Mechtro LLC)
 *          tiagojen@gmail.com
 *
 */

#include "IsParse.h"

void skipSpaces(bool * pIsOk, const char * pStr, uint16_t* pCaret){
    if( *pIsOk ){
        while(true){
            char c = *(pStr + *pCaret);
            if( c == ' ' ){
                (*pCaret)++; // go to next char
            }else{
                break;
            }
        }
    }
}

void skipWhite(bool * pIsOk, const char * pStr, uint16_t* pCaret){
    if( *pIsOk ){
        while(true){
            char c = *(pStr + *pCaret);
            if(
                c == ' ' ||
                c == '\t' ||
                c == '\r' ||
                c == '\n'
            ){
                (*pCaret)++; // go to next char
            }else{
                break;
            }
        }
    }
}

void matchChar(bool * pIsOk, const char * pStr, uint16_t* pCaret, char matchChar){
    if( *pIsOk ){
        char c = *(pStr + *pCaret);
        if( c == matchChar ){
            (*pCaret)++; // go to next char
        }else{
            *pIsOk = false;
        }
    }
}

void extractChar(bool * pIsOk, const char * pStr, uint16_t* pCaret, char * pChar){
    if( *pIsOk ){
        char c = *(pStr + *pCaret);
        if( c == 0 ){
            *pIsOk = false; // encountered end of str
        }else{
            *pChar = c;
            (*pCaret)++; // go to next char
        }
    }
}

void extractToChar(bool * pIsOk, const char * pStr, uint16_t* pCaret, char dividerChar, char* pBuf, uint16_t bufSize){
    uint16_t bufOffset = 0;
    while( *pIsOk ){
        if( bufOffset >= (bufSize - 1)){
            *pIsOk = false; // ran out of buf room
        }else{
            char c = *(pStr + *pCaret);
            if( c == 0 ){
                *pIsOk = false; // ran out of string
            }else if( c == dividerChar ){ // i.e. div
                *(pBuf + bufOffset) = 0; // terminate string
                break;
            }else{
                bufOffset ++;
                (*pCaret) ++;
            }
        }
    }
}

void matchStr(bool * pIsOk, const char * pStr, uint16_t* pCaret, const char * pMasterStr){
    if( *pIsOk ){
        uint16_t charCnt = 0;
        while(true){
            char c = *(pStr + *pCaret);
            char mc = *(pMasterStr + charCnt);
            if( mc == 0){
                break; // at end of master str, still OK
            }else if( c == 0 ){ // end of str, before end of master
                *pIsOk = false;
                break;
            }else if( c != mc ){ // not a match
                *pIsOk = false;
                break;
            }
            (*pCaret)++; // go to next char
            charCnt++;
        }
    }
}

// looks to see if next is white or empty, doesn't advance caret
void matchEndWord(bool * pIsOk, const char * pStr, uint16_t* pCaret){
    if( *pIsOk ){
        char c = *(pStr + *pCaret);
        *pIsOk =  c == 0 || // end of string
                  c == ' ' ||
                  c == '\t' ||
                  c == '\r' ||
                  c == '\n' ;
    }
}

void matchWord(bool * pIsOk, const char * pStr, uint16_t* pCaret, const char * pMasterStr){
    matchStr(pIsOk, pStr, pCaret, pMasterStr);
    matchEndWord(pIsOk, pStr, pCaret);
}

void parseUint8(bool * pIsOk, const char * pStr, uint16_t* pCaret, uint8_t * pValue){
    if( *pIsOk ){
        uint8_t digitCnt = 0;
        uint16_t num = 0;
        while(true){
            char c = *(pStr + *pCaret);
            if( c >= '0' && c <= '9' ){
                num = num * 10 + (c - '0');
                if( num > 0xFF ){ // over range
                    *pIsOk = false;
                    break;
                }
                digitCnt++;
                (*pCaret)++; // go to next char
            }else{ // not a digit
                break;
            }
        }
        if( *pIsOk && digitCnt == 0){ // require at least one digit
            *pIsOk = false;
        }
        if( *pIsOk ){
            *pValue = (uint8_t)num;
        }
    }
}

void parseUint16(bool * pIsOk, const char * pStr, uint16_t* pCaret, uint16_t * pValue){
    if( *pIsOk ){
        uint8_t digitCnt = 0;
        uint32_t num = 0;
        while(true){
            char c = *(pStr + *pCaret);
            if( c >= '0' && c <= '9' ){
                num = num * 10 + (c - '0');
                if( num > 0xFFFF ){ // over range
                    *pIsOk = false;
                    break;
                }
                digitCnt++;
                (*pCaret)++; // go to next char
            }else{ // not a digit
                break;
            }
        }
        if( *pIsOk && digitCnt == 0){ // require at least one digit
            *pIsOk = false;
        }
        if( *pIsOk ){
            *pValue = (uint32_t)num;
        }
    }
}

void parseUint32(bool * pIsOk, const char * pStr, uint16_t* pCaret, uint32_t * pValue){
    if( *pIsOk ){
        uint8_t digitCnt = 0;
        uint64_t num = 0;
        while(true){
            char c = *(pStr + *pCaret);
            if( c >= '0' && c <= '9' ){
                num = num * 10 + (c - '0');
                if( num > 0xFFFFFFFF ){ // over range
                    *pIsOk = false;
                    break;
                }
                digitCnt++;
                (*pCaret)++; // go to next char
            }else{ // not a digit
                break;
            }
        }
        if( *pIsOk && digitCnt == 0){ // require at least one digit
            *pIsOk = false;
        }
        if( *pIsOk ){
            *pValue = (uint32_t)num;
        }
    }
}

void parseInt32(bool * pIsOk, const char * pStr, uint16_t* pCaret, int32_t * pValue){
    if( *pIsOk ){
        char c = *(pStr + *pCaret);
        uint32_t un = 0;
        uint8_t isNeg = false;
        if( c == '-'){
            isNeg = true;
            (*pCaret)++; // go to next char
        }
        parseUint32(pIsOk, pStr, pCaret, &un);
        if( *pIsOk ){
            // this check could handle the one extra number when negative, but
            // this is good enough for real cases
            if( un > 0x7FFFFFFF){
                *pIsOk = false;
                return;
            }
            if( isNeg ){
                *pValue = -un;
            }else{ // pos
                *pValue = un;
            }
        }
    }
}

void parseHexDigit(bool * pIsOk, const char * pStr, uint16_t* pCaret, uint8_t * pDigit){
    char c = *(pStr + *pCaret);
    if( c >= 0x30 && c <= 0x39 ){ // i.e. 0..9
        *pDigit = c - 0x30;
        (*pCaret)++; // go to next char
    }else if( c >= 0x41 && c <= 0x46 ){ // i.e. A..F
        *pDigit = c - 0x41 + 10;
        (*pCaret)++; // go to next char
    }else if( c >= 0x61 && c <= 0x66 ){ // i.e. a..f
        *pDigit = c - 0x61 + 10;
        (*pCaret)++; // go to next char
    }else{
        *pIsOk = false;
    }
}

void parseHexUint8(bool * pIsOk, const char * pStr, uint16_t* pCaret, uint8_t * pValue){
    if( *pIsOk ){
        uint8_t digitCnt = 0;
        uint8_t num = 0;
        while(true){
            uint8_t digit = 0;
            parseHexDigit(pIsOk, pStr, pCaret, &digit);
            if( !(*pIsOk) ){ // wasn't a digit
                if( digitCnt > 0 ){ // already found some digits, so num is good
                    *pIsOk = true;
                }
                break;
            }
            num = (num << 4) | digit;
            digitCnt ++;
            if( digitCnt >= 2 ){
                break;
            }
        }
        if( *pIsOk ){
            *pValue = num;
        }
    }
}

void parseHexUint16(bool * pIsOk, const char * pStr, uint16_t* pCaret, uint16_t * pValue){
    if( *pIsOk ){
        uint8_t digitCnt = 0;
        uint16_t num = 0;
        while(true){
            uint8_t digit = 0;
            parseHexDigit(pIsOk, pStr, pCaret, &digit);
            if( !(*pIsOk) ){ // wasn't a digit
                if( digitCnt > 0 ){ // already found some digits, so num is good
                    *pIsOk = true;
                }
                break;
            }
            num = (num << 4) | digit;
            digitCnt ++;
            if( digitCnt >= 4 ){
                break;
            }
        }
        if( *pIsOk ){
            *pValue = num;
        }
    }
}

// lenient parse to skip separators between parts of num
void parseHexAddr64(bool * pIsOk, const char * pStr, uint16_t* pCaret, uint64_t * pValue){
    if( *pIsOk ){
        uint8_t digitCnt = 0;
        uint64_t num = 0;
        while(true){
            uint8_t digit = 0;
            char c = *(pStr + *pCaret);
            if( c == '-' || c == '.' || c == ',' || c == '|'){ // skip separators
              (*pCaret)++; // go to next char
              continue;
            }
            parseHexDigit(pIsOk, pStr, pCaret, &digit);
            if( !(*pIsOk) ){ // wasn't a digit
                if( digitCnt > 0 ){ // already found some digits, so num is good
                    *pIsOk = true;
                }
                break;
            }
            num = (num << 4) | digit;
            digitCnt ++;
            if( digitCnt >= 16 ){
                break;
            }
        }
        if( *pIsOk ){
            *pValue = num;
        }
    }
}

void parseNextWord(bool * pIsOk, const char * pStr, uint16_t* pCaret, char * pParsedBuf, uint32_t bufSize ){
    uint16_t cnt = 0;
    while(*pIsOk){
        if( cnt <= (bufSize -1)){
            char c = *(pStr + *pCaret);
            if( c == 0 || !isAlphaNumeric(c) ){ // handle end of str in src
                *(pParsedBuf + cnt) = 0;
                if( cnt == 0 ){
                    *pIsOk = false;
                }
                return;
            }
            *(pParsedBuf + cnt) = c;
            cnt ++;
            (*(pCaret))++;
        }else{
            *pIsOk = false;
        }
    }
}
