/* EpSvc.h
 *
 * Licence: See "LICENSE.txt"
 *
 * Author: James Jensen (Mechtro LLC)
 *         tiagojen@gmail.com
 *
 */

#ifndef _EP_SVC_H_
#define _EP_SVC_H_

#include "Arduino.h"

// --------------------------------------------------------------------
// a base class for classes that encapsulate a sequence to execute on
// an agent
extern const char * PRECHECK_OK_MSG;
extern const char * ABORT_OK_MSG;
extern const char * DEFAULT_SVC_NAME;

class Svc{

  protected:
	uint8_t started;
	uint8_t stopped;

	const char * pAbortMsg;
	const char * pPrecheckFailMsg;

  public:
  	Svc(){
  	}

  	const char * pSvcName;

  	virtual void setup(){}
  	virtual void eval();
  	virtual void precheck(){}

  	void resetFlags(){
  		started = false;
  		stopped = false;
  		pAbortMsg = ABORT_OK_MSG;
  		pPrecheckFailMsg = PRECHECK_OK_MSG;
  	}

  	void setStarted(){ started = true; }
  	void setStopped(){ stopped = true; }
  	void setAbortMsg(const char * pMsg){ pAbortMsg = pMsg; }
  	void setPrecheckFailMsg(const char * pMsg){ pPrecheckFailMsg = pMsg; }

  	const char * getAbortMsg(){ return pAbortMsg; }
  	const char * getPrecheckFailMsg(){ return pPrecheckFailMsg; }

  	uint8_t wasPrecheckBad(){ return pPrecheckFailMsg != PRECHECK_OK_MSG; }

  	uint8_t wasStarted(){
  		return started;
  	}
    uint8_t wasStopped(){
      return stopped;
    }
  	uint8_t isActive(){
  		return started && !stopped;
  	}

  	uint8_t wasAborted(){
  		return pAbortMsg != ABORT_OK_MSG;
  	}
  	uint8_t wasSuccessful(){
  		return (pPrecheckFailMsg == PRECHECK_OK_MSG) && started && stopped && (pAbortMsg == ABORT_OK_MSG);
  	}

};

// --------------------------------------------------------------------

#endif // _EP_SVC_H_
