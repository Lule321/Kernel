/*
 * PCB.cpp
 *
 *  Created on: Jun 23, 2021
 *      Author: OS1
 */

#include "PCB.h"
#include "timer.h"
#include "SCHEDULE.H"
#include <iostream.h>
#include "Fork.h"

extern unsigned nextId;
extern unsigned cntThread;
extern unsigned tableSize;
extern int* tableId;
extern PCB** tableThread;


#define lock asm{\
	pushf;\
	cli;\
}

#define unlock asm popf;

PCB::PCB(int t, unsigned sSize, startFun start, Thread* thr)
{
	//za id
	int tmp;
	PCB** tmpTable;
	int* tmpIds;
	int i;


	//velicina steka
	if(sSize > maxStackSize) sSize = maxStackSize;
	if(sSize <= 0) sSize = defStackSize;
	this->stackSize = sSize / 2;

	//inicijalizacija steka, ss, sp i bp

	lock;
	this->stack = new unsigned[this->stackSize];
	unlock;

	this->stack[this->stackSize - 1] = 0x200; //psw
	#ifndef BCC_BLOCK_IGNORE
	this->stack[this->stackSize - 2] = FP_SEG(start);
	this->stack[this->stackSize - 3] = FP_OFF(start);

	this->ss = FP_SEG(this->stack + this->stackSize - 12);
	this->sp = FP_OFF(this->stack + this->stackSize - 12);
	#endif
	this->bp = this->sp;

	//finished
	this->finished = 0;

	// timeSlice
	if(t < 0) t = defTimeSlice;
	if(t == 0) t = -1;
	this->timeSlice = t;

	//myThread
	this->myThread = thr;

	//suspended
	this->suspended = 0;

	//headWait
	this->headWait = 0;

	//started
	this->started = 0;

	//id

	lock;
	//podesavanje tabele sa nititma
	if(++cntThread == tableSize)
	{
		tableSize *= 2;
		tmpTable = new PCB*[tableSize];
		tmpIds = new int[tableSize];
		for(i = 0; i < cntThread; i++)
		{
			tmpTable[i] = tableThread[i];
			tmpIds[i] = tableId[i];
		}

		for(i = cntThread; i < tableSize; i++)
		{
			tmpTable[i] = 0; //nullptr
			tmpIds[i] = 1; //sledece mesto slobodno
		}

		delete[] tableThread;
		delete[] tableId;

		tableThread = tmpTable;
		tableId = tmpIds;
	}
	id = nextId;
	tmp = nextId;
	nextId = tableId[nextId] + nextId;
	tableId[tmp] = 0;
	tableThread[tmp] = this;
	unlock;

	//waitRet
	this->waitRet = 0;

	//numChildren
	this->numChildren = 0;

	//father
	this->father = 0;

	//waitForChildren
	this->waitForChildren = 0;

}

PCB::PCB() //samo za main thread
{
	this->timeSlice = defTimeSlice;
	this->finished = 0;
	this->stack = 0;
	this->suspended = 0;
	this->headWait = 0;
	this->started = 0;
	this->id = -1; //nije stvarno -1 al to je za main PCB
	this->waitRet = 0;
	this->numChildren = 0;
	this->father = 0;
	this->waitForChildren = 0;
	this->myThread = 0;
}

PCB::~PCB()
{
	int tmp;
	if(stack)
	{
		lock;
		delete[] stack;
		stack = 0;

		//podesavanje tabele za Thread

		tmp = nextId - this->id;
		nextId = this->id;
		tableId[nextId] = tmp;
		tableThread[nextId] = 0;
		cntThread--;
		unlock;
	}



}


void PCB::putToWait()
{
	//stavljanje pcb-a na cekanje
	lock;
	if(!this->finished)
	{
		ElemWait* tmp = new ElemWait();
		tmp->p = running;
		tmp->next = headWait;
		headWait = tmp;
		running->suspended = 1;
		dispatch();
	}
	unlock;
}

Thread* PCB::getThreadById(unsigned id)// moguce da dodje do problema ako se obrise nesto kad ne treba
{
	Thread *ret = 0;
	//lock;
	if(id >= 0 && id < cntThread)
	{
	 ret = tableThread[id]->myThread;
	}
	//unlock;
	return ret;
}

unsigned PCB::getRunningId() //ako se vrati nula to je zaludni PCB, mada to ne bi smelo da se desi
{
	return running->id;
}

void PCB::start()
{
	lock;
	if(!this->started)
	{
		this->started = 1;
		Scheduler::put(this);
	}
	unlock;
}

unsigned PCB::getId()
{
	return id;
}

void PCB::run()
{
	myThread->run();
}

//fork

unsigned PCB::fork()
{

	Thread* thr;
	PCB* child;
	lock;
	thr = running->myThread->clone();
	unlock;

	if(thr == 0)
	{
		return -1;
	}

	child = thr->myPCB;

	running->numChildren++;
	child->father = running;
	child->started = 1;

	Fork* forkThr;
	lock;
	forkThr = new Fork(running, child);
	unlock;

	lock;
	forkThr->start();
	forkThr->waitToComplete();
	unlock;


	if(running->id != child->id)
	{
		lock;
		delete forkThr;
		unlock;
		return child->id;
	}
	else
	{
		return 0;
	}


}




void PCB::exit()
{

	//sve niti koje su cekale na ovu se oslobadjaju
	lock;
	while(running->headWait)
		{
			PCB::ElemWait* tmp = running->headWait;
			PCB* pcbTmp = tmp->p;
			pcbTmp->suspended = 0;

			Scheduler::put(pcbTmp);

			running->headWait = running->headWait->next;


			delete tmp;

		}

	//ako je otac stavio waitForForkChildren
		if(running->father)
		{
			running->father->numChildren--;

			if(!(running->father->numChildren) && running->father->waitForChildren)
			{
				running->father->waitForChildren = 0;
				running->father->suspended = 0;
				Scheduler::put(running->father);
			}
		}
		running->finished = 1;
		unlock;
		dispatch();
}

void PCB::waitForForkChildren()
{
	lock;

	if(running->numChildren)
	{
	running->suspended = 1;
	running->waitForChildren = 1;
	dispatch();
	}
	unlock;
}


