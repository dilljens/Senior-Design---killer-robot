/* EpSvcRunner.h
 *
 * Licence: See "LICENSE.txt"
 *
 * Author: James Jensen (Mechtro LLC)
 *         tiagojen@gmail.com
 *
 */

#ifndef _EP_SVC_RUNNER_H_
#define _EP_SVC_RUNNER_H_

#include "EpSvc.h"

#define NUM_SVC 8

// --------------------------------------------------------------------
//
class SvcRunner{
    Svc * svcList[NUM_SVC];

    uint8_t debug;

  public:
    SvcRunner():
        debug(1)
    {

    }

    void eval(){
        for(int i=0; i<NUM_SVC; i++){
            Svc * pSvc = svcList[i];
            if( pSvc ){
                pSvc->eval();
                if( pSvc->wasStopped() ){
                    // cleanup and remove svc from runner
                    if( debug ){
                        printEnd(pSvc);
                    }
                    svcList[i] = NULL;
                }
            }
        }
    }

    uint32_t getNumBusy(){
        uint32_t count = 0;
        for(int i=0; i<NUM_SVC; i++){
            if( svcList[i] ){  count ++; }
        }
        return count;
    }

    uint32_t getNumIdle(){
        return NUM_SVC - getNumBusy();
    }

    // look for idle svc and start it
    // return true if successful
    uint8_t start(Svc & svc){
        return start(&svc);
    }
    uint8_t start(Svc * pSvc){
        prBuf.prependMs();
        prBuf.print("Start svc [");
        prBuf.print(pSvc->pSvcName);
        prBuf.println("]");
        uint8_t foundOne = false;
        int i = 0;
        while(i<NUM_SVC && !foundOne){
            prBuf.prependMs();
            prBuf.print("  svc ");
            prBuf.print(i);
            prBuf.print(" full=");
            prBuf.println(svcList[i] != NULL);
            if( ! svcList[i]){
                foundOne = true;
                svcList[i] = pSvc;
                pSvc->eval(); // do first eval so it will set flag as started
                /*
                if( debug ){
                    prBuf.print("Start svc [");
                    prBuf.print(pSvc->pSvcName);
                    prBuf.println("]");
                }
                */
            }
            i++;
        }
        return foundOne;
    }

    void abort(Svc & svc, const char * pMsg){
        abort(&svc, pMsg);
    }
    void abort(Svc * pSvc, const char * pMsg){
        for( int i=0; i<NUM_SVC; i++){
            if( svcList[i] == pSvc ){
                svcList[i] = NULL; // remove
                if( debug ){
                    printEnd(pSvc);
                }
                pSvc->setAbortMsg(pMsg);
                pSvc->eval(); // give svc a change to process abort
                pSvc->resetFlags();
                break;
            }
        }
    }

  private:
    void printEnd(Svc * pSvc){
        prBuf.prependMs();
        prBuf.print("End Svc [");
        prBuf.print(pSvc->pSvcName);
        prBuf.print("]");
        if(pSvc->getAbortMsg() != ABORT_OK_MSG){
            prBuf.print(" aborted[");
            prBuf.print(pSvc->getAbortMsg());
            prBuf.print("]");
        }
        prBuf.println();
    }

};

// --------------------------------------------------------------------

#endif // _EP_SVC_RUNNER_H_
