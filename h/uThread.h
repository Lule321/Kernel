/*
 * uThread.h
 *
 *  Created on: Jun 28, 2021
 *      Author: OS1
 */

#ifndef UTHREAD_H_
#define UTHREAD_H_

#include "Thread.h"

class uThread: public Thread {
public:
	uThread(int argc, char** argv, unsigned long stackSize = 4096, unsigned timeSlice = 2);
	virtual ~uThread();
	virtual void run();
	virtual Thread* clone() const;
	int retValue() const;

private:
	int argc;
	char** argv;
	unsigned long sSize;
	unsigned tSlice;
	int ret;
};

#endif /* UTHREAD_H_ */
