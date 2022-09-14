/*
 * PCB.h
 *
 *  Created on: Jun 23, 2021
 *      Author: OS1
 */

#ifndef PCB_H_
#define PCB_H_
#include <dos.h>
#include "glFun.h"
#include "Thread.h"


typedef void (*startFun)();

extern unsigned switchOnDemand;
extern unsigned lockFlag;
const unsigned maxStackSize = 64 * 1024;
const unsigned defStackSize = 4096;
const int defTimeSlice = 2;


class PCB {

public:
	int timeSlice;
	unsigned stackSize;
	unsigned ss;
	unsigned sp;
	unsigned bp;
	unsigned* stack;
	unsigned finished;
	unsigned suspended;
	unsigned started;//dodao
	unsigned id;//dodao

	unsigned waitRet;

	Thread* myThread;

	 struct ElemWait
	 {
		 PCB* p;
		 ElemWait* next;
	 };

	 ElemWait* headWait;

	 void putToWait();

	PCB(int t, unsigned sSize, startFun fun, Thread* thr);
	PCB();
	~PCB();

	static Thread* getThreadById(unsigned id);
	static unsigned getRunningId();

	//fork
	static unsigned fork();
	static void exit();
	static void waitForForkChildren();
	unsigned numChildren;
	PCB* father;
	unsigned waitForChildren;

	//endfork

	void start();
	unsigned getId();

	void run();

};


#endif /* PCB_H_ */
