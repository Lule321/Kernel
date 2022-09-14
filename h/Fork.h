/*
 * Fork.h
 *
 *  Created on: Jun 30, 2021
 *      Author: OS1
 */

#ifndef FORK_H_
#define FORK_H_

#include "Thread.h"

class Fork: public Thread {
public:
	PCB* father;
	PCB* child;
	Fork(PCB* f, PCB* c, unsigned stackSize = 4096, unsigned timeSlice = 2);
	virtual ~Fork();
	virtual void run();
	virtual Thread* clone() const;
};

#endif /* FORK_H_ */
