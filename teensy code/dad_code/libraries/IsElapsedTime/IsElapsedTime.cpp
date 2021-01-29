/*
 * Licence: See "LICENSE.txt"
 *
 * Author: James Jensen (Mechtro LLC)
 *         tiagojen@gmail.com
 *
 */
#include "IsElapsedTime.h"

//=========================================================

void IsAdvancer::link(IsElapsedMs32 *pNode){
    // insert at front of list
    IsElapsedMs32 * pOldHead = pHeadElapsedMs32;
    pNode->pNext = pOldHead;
    pHeadElapsedMs32 = pNode;
}

void IsAdvancer::unlink(IsElapsedMs32 *pNode){
    if( pHeadElapsedMs32 == pNode){
        pHeadElapsedMs32 = pNode->pNext;
        pNode->pNext = NULL;
    }else{
        bool foundIt = false;
        // find prev node in list
        IsElapsedMs32* pPrev = pHeadElapsedMs32;
        IsElapsedMs32* pSearch = pHeadElapsedMs32->pNext;
        while( !foundIt && pSearch ){
            if( pSearch == pNode ){
                foundIt = true;
            }else{
                pPrev = pSearch;
                pSearch = pSearch->pNext;
            }
        }
        if( foundIt ){ // link around node to be removed
            pPrev->pNext = pNode->pNext;
        }else{// error not found, do nothing
        }
        pNode->pNext = NULL;
    }
}

void IsAdvancer::link(IsElapsedUs32 *pNode){
    // insert at front of list
    IsElapsedUs32 * pOldHead = pHeadElapsedUs32;
    pNode->pNext = pOldHead;
    pHeadElapsedUs32 = pNode;
}

void IsAdvancer::unlink(IsElapsedUs32 *pNode){
    if( pHeadElapsedUs32 == pNode){
        pHeadElapsedUs32 = pNode->pNext;
        pNode->pNext = NULL;
    }else{
        bool foundIt = false;
        // find prev node in list
        IsElapsedUs32* pPrev = pHeadElapsedUs32;
        IsElapsedUs32* pSearch = pHeadElapsedUs32->pNext;
        while( !foundIt && pSearch ){
            if( pSearch == pNode ){
                foundIt = true;
            }else{
                pPrev = pSearch;
                pSearch = pSearch->pNext;
            }
        }
        if( foundIt ){ // link around node to be removed
            pPrev->pNext = pNode->pNext;
        }else{// error not found, do nothing
        }
        pNode->pNext = NULL;
    }
}

void IsAdvancer::advance(){
    advanceUs(micros());
}

void IsAdvancer::advanceUs(uint32_t micros){
    uint32_t deltaUs;
    if( lastMicros > micros ){ // rollover
        deltaUs = UINT32_MAX - lastMicros + micros + 1;
    }else{
        deltaUs = micros - lastMicros;
    }
    lastMicros = micros;

    millisRemainderUs += deltaUs;
    uint32_t deltaMs = millisRemainderUs / 1000;
    millisRemainderUs = millisRemainderUs % 1000;

    /*
    Serial.print("XX advance:  deltaMs=");
    Serial.print(deltaMs);
    Serial.print(" deltaUs=");
    Serial.print(deltaUs);
    Serial.print(" remUs=");
    Serial.print(millisRemainderUs);
    Serial.println();
    */

    // now update all elapsed on lists
    IsElapsedMs32 * pNextMs32 = pHeadElapsedMs32;
    while( pNextMs32 ){
        pNextMs32->update(deltaMs);
        pNextMs32 = pNextMs32->pNext;
    }

    IsElapsedUs32 * pNextUs32 = pHeadElapsedUs32;
    while( pNextUs32 ){
        pNextUs32->update(deltaUs);
        pNextUs32 = pNextUs32->pNext;
    }

}
