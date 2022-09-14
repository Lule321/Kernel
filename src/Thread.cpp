/*
 * Thread.cpp
 *
 *  Created on: Jun 24, 2021
 *      Author: OS1
 */

#include "Thread.h"
#include "glFun.h"
#include "PCB.h"
#include "timer.h"
#include <iostream.h>
#include <stdlib.h>

#include "SCHEDULE.H"

extern unsigned switchOnDemand;

#define lock asm{\
	pushf;\
	cli;\
}

#define unlock asm popf;

void dispatch()
{
	lock;
	switchOnDemand = 1;
	timer();
	unlock;
}


Thread::Thread(StackSize stackSize, Time timeSlice)
{


	lock;
	//pravljenje PCB-a
	myPCB = new PCB(timeSlice, stackSize, wrapperRun, this);

	unlock;




}


Thread::~Thread()
{
	waitToComplete();
	//brisanje PCB-a
	if(this->myPCB)
	{
		lock;
		delete myPCB;
		myPCB = 0;
		unlock;
	}



}


ID Thread::getId()
{
	return this->myPCB->getId();
}

void Thread::waitToComplete()
{
	this->myPCB->putToWait(); // stavlja running da ceka
}


void Thread::start()
{
	this->myPCB->start();
}

ID Thread::getRunningId()
{
	return PCB::getRunningId();
}

Thread* Thread::getThreadById(ID id)
{
	return PCB::getThreadById(id);
}

Thread* Thread::clone() const
{
	unsigned sSize = running->stackSize;
	unsigned timeSlice = running->timeSlice;
	Thread* cloneThread = 0;
	lock;
	cloneThread = new Thread(sSize * 2, timeSlice);
	unlock;
	return cloneThread;
}

ID Thread::fork()
{
	return PCB::fork();
}

void Thread::exit()
{
	PCB::exit();
}

void Thread::waitForForkChildren()
{
	PCB::waitForForkChildren();
}


