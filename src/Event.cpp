/*
 * Event.cpp
 *
 *  Created on: Jun 27, 2021
 *      Author: OS1
 */


#include "KernelEv.h"

extern IVTEntry* entryTable[256];

#define lock asm{\
					pushf;\
					cli;\
}

#define unlock asm popf

Event::Event(IVTNo ivtNo)
{

	lock;
	myImpl = new KernelEv(ivtNo, this);
	unlock;
}

Event::~Event()
{
	//dodao
	lock;
	if(entryTable[myImpl->num])
	{
	setvect(myImpl->num, entryTable[myImpl->num]->oldFun);
	entryTable[myImpl->num] = 0;
	}
	unlock;
	//kraj dodao
	lock;
	delete myImpl;
	unlock;
}

void Event::wait()
{
	myImpl->wait();
}

void Event::signal()
{
	myImpl-> signal();
}


