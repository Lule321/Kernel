/*
 * globalFun.cpp
 *
 *  Created on: Jun 23, 2021
 *      Author: OS1
 */
#include "glFun.h"
#include "PCB.h"
#include "timer.h"
#include "SCHEDULE.H"
#include <iostream.h>

#define lock asm{\
	pushf;\
	cli;\
}

#define unlock asm popf;

void wrapperRun()
{
	//run
	running->run();
	//vrati niti koje su cekale


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

	if(running->father)
	{
		running->father->numChildren--;
		if( !(running->father->numChildren) && running->father->waitForChildren)
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
