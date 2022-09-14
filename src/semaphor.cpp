/*
 * semaphor.cpp
 *
 *  Created on: Jun 25, 2021
 *      Author: OS1
 */

#include "semaphor.h"
#include "KSem.h"
#define lock asm{\
					pushf;\
					cli;\
}

#define unlock asm popf

Semaphore::Semaphore(int init)
{
	lock;
	myImpl = new KernelSem(init, this);
	unlock;
}

Semaphore::~Semaphore()
{
	lock;
	delete myImpl;
	myImpl = 0;
	unlock;
}

int Semaphore::wait(Time w)
{
	return myImpl->wait(w);
}

void Semaphore::signal()
{
	myImpl->signal();
}

int Semaphore::val() const
{
	return myImpl->val();
}

