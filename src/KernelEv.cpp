/*
 * KernelEv.cpp
 *
 *  Created on: Jun 27, 2021
 *      Author: OS1
 */

#include "KernelEv.h"
#include "SCHEDULE.H"
extern KernelEv* evTable[256];
extern PCB* running;

#define lock asm{\
					pushf;\
					cli;\
}

#define unlock asm popf

KernelEv::KernelEv(unsigned char IVTNo, Event* e)
{
	created = running;
	myEv = e;
	value = 0;
	num = IVTNo;
	suspended = 0;

	lock;
	evTable[num] = this;
	unlock;


}

KernelEv::~KernelEv()
{
	lock;
	evTable[num] = 0;
	unlock;
}

void KernelEv::signal()
{
	lock;
	if(this->suspended)
	{
		this->suspended = 0;
		this->created->suspended = 0;
		Scheduler::put(this->created);
	}
	else
	{
		this->value = 1;
	}
	unlock;
}

void KernelEv::wait()
{
	if(running == this->created)
	{
		lock;
		if(this->value == 1)
		{
			this->value = 0;
		}
		else
		{

			this->suspended = 1;
			running->suspended = 1;
			dispatch();
		}
		unlock;
	}
}
