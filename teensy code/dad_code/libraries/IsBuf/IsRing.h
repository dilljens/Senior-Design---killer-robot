/*
 * a generic byte ring buffer
 *
 * To use this, pass in a ptr to a pre allocated buffer with size
 * Be careful that they match.
 *
 * Licence: See "LICENSE.txt"
 *
 * Author: James Jensen (Mechtro LLC)
 *         tiagojen@gmail.com
 *
 */

#ifndef _IS_RING_H_
#define _IS_RING_H_


// =========================================================================
class IsRingBuffer{
    uint8_t * pBuf; // pointer to storage
    uint32_t size;

    uint8_t * pHead;
    uint8_t * pTail;

    uint32_t maxFill;

  public:
    IsRingBuffer(uint8_t * _pBuf, uint32_t _size):
        pBuf(_pBuf),
        size(_size),
        pHead(pBuf),
        pTail(pBuf),
        maxFill(0)
    {
    }

    bool push(uint8_t b){
        uint8_t *pH = pHead + 1;
        if( pH >= (pBuf + size) ){ pH = pBuf; } // handle wrap
        if( pH == pTail ){ // i.e. full
            return false;
        }else{
            *pHead = b;
            pHead = pH;
            maxFill = max( maxFill, filled() );
            return true;
        }
    }

    bool pop(uint8_t *pB){
        if( pHead == pTail ){
            return false;
        }else{
            *pB = *pTail;
            pTail++;
            if( pTail >= (pBuf + size) ){ pTail = pBuf; } // handle wrap
            return true;
        }
    }

    // copy contents of ring to buffer
    void copy(uint8_t *pNewBuf, uint32_t newBufSize){
        uint32_t offset = 0;
        uint8_t * pMark = pTail;

        while( pMark != pHead && offset < newBufSize-1){
            *(pNewBuf+offset) = *pMark;
            pMark++;
            if( pMark>= (pBuf + size) ){ pMark = pBuf; } // handle wrap
            offset++;
        }

        *(pNewBuf+offset) = 0; // null terminate
    }

    // send contents to a Print object
    void dump(Print * pPrint){
        uint8_t * pMark = pTail;

        while( pMark != pHead ){
            pPrint->print((char)(*pMark));
            pMark++;
            if( pMark>= (pBuf + size) ){ pMark = pBuf; } // handle wrap
        }
    }

    void clear(){
        pTail = pHead;
    }

    uint32_t free(){
        return size - 1 - filled();
    }

    uint32_t filled(){
        return (pHead + size - pTail) % size;
    }

    uint32_t maxFilled(){
        return maxFill;
    }

    void resetMaxFilled(){
        maxFill = 0;
    }

};


#endif // _IS_RING_H_
