/*
 * KernelEv.h
 *
 *  Created on: Jun 27, 2021
 *      Author: OS1
 */

#ifndef KERNELEV_H_
#define KERNELEV_H_

#include "Event.h"
#include "PCB.h"
#include "IVTEntry.h"


class KernelEv {

public:
	KernelEv(unsigned char IVTNo, Event* e);
	~KernelEv();

	void wait();
	void signal();


	Event* myEv;
	PCB* created;
	unsigned value;
	unsigned char num;
	unsigned suspended;
	//IVTEntry* entry;

};

#endif /* KERNELEV_H_ */
