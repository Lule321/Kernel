/*
 * KernelSem.cpp
 *
 *  Created on: Jun 25, 2021
 *      Author: OS1
 */

#include "KSem.h"
#include <iostream.h>
#include "Thread.h"

#define lock asm{\
					pushf;\
					cli;\
}

#define unlock asm popf

extern unsigned nextIdSem;
extern unsigned cntSem;
extern unsigned tableSizeSem;
extern int* tableIdSem;
extern KernelSem** tableSem;

extern PCB* running;

KernelSem::KernelSem(int v, Semaphore* s)
{

	//za id
	int tmp;
	KernelSem** tmpTable;
	int* tmpIds;
	int i;

	// value
	this->value = v;

	//mySem
	this->mySem = s;

	//liste
	this->headT = 0;
	//this->tailT = 0;
	this->headSig = 0;
	this->tailSig = 0;

	//tabele podesi i id takodje
	lock;
	if(++cntSem == tableSizeSem)
	{
		tableSizeSem *= 2;
		tmpTable = new KernelSem*[tableSizeSem];
		tmpIds = new int[tableSizeSem];
		for(i = 0; i < cntSem; i++)
		{
			tmpTable[i] = tableSem[i];
			tmpIds[i] = tableIdSem[i];
		}
		for(i = cntSem; i < tableSizeSem; i++)
		{
			tmpTable[i] = 0; //nullptr
			tmpIds[i] = 1; //sledece mesto slobodno
		}

		delete[] tableSem;
		delete[] tableIdSem;

		tableSem = tmpTable;
		tableIdSem = tmpIds;
	}
	id = nextIdSem;
	tmp = nextIdSem;
	nextIdSem = tableIdSem[nextIdSem] + nextIdSem;
	tableIdSem[tmp] = 0;
	tableSem[tmp] = this;
	unlock;



}

KernelSem::~KernelSem()
{
	int tmp;
	if(headSig)
	{
		lock;
		cout << "Bukavlno si obrisao semfor dok je jos nesto u njemu!!!" << endl;
		unlock;
	}

	lock;
	tmp = nextIdSem - this->id;
	nextIdSem = this->id;
	tableIdSem[nextIdSem] = tmp;
	tableSem[nextIdSem] = 0;
	cntSem--;
	unlock;
}


int KernelSem::val() const
{
	return this->value;
}


int KernelSem::wait(int t)
{
	int ret = 0;
	if(t < 0)
	{
		t = 0;
	}
	lock;
	if(--(this->value) < 0)
	{
		this->block(t);
		ret = running->waitRet;
		running->waitRet = 0;
	}
	unlock;
	return ret;
}

void KernelSem::block(int t)
{
	ElemSemSig* tmpSig = new ElemSemSig();
	tmpSig->p = running;
	tmpSig->next = 0;
	tmpSig->prev = 0;
	tmpSig->same = 0;

	//stavljanje u listu koje cekaju signal

	if(!this->headSig)
	{
		this->headSig = tmpSig;
	}
	else
	{
		this->tailSig->next = tmpSig;
		tmpSig->prev = this->tailSig;
	}
	this->tailSig = tmpSig;

	//stavljanje u listu koje cekaju vreme

	if(t != 0)
	{
		ElemSemT* tmpT = new ElemSemT();
		tmpT->p = running;
		tmpT->next = 0;
		tmpT->prev = 0;
		tmpT->same = tmpSig;
		tmpT->waiting = 0;

		ElemSemT* tmpTIt;
		ElemSemT* prevTIt = 0;
		for(tmpTIt = this->headT; tmpTIt && tmpTIt->waiting <= t; tmpTIt = tmpTIt->next)
		{
			prevTIt = tmpTIt;
			t -= tmpTIt->waiting;
		}

		tmpT->prev = prevTIt;
		tmpT->next = tmpTIt;
		tmpT->waiting = t;
		if(!prevTIt)
		{
			this->headT = tmpT;
		}
		else
		{
			prevTIt->next = tmpT;
		}

		if(tmpTIt)
		{
			tmpTIt->prev = tmpT;
		}

		tmpSig->same = tmpT;
	}

	running->suspended = 1;
	dispatch();
}

void KernelSem::signal()
{
	lock;
	if((this->value)++ < 0)
	{
		unblock();
	}
	unlock;
}

void KernelSem::unblock()
{
	ElemSemSig* tmpSig = this->headSig;
	headSig = headSig->next;
	if(!headSig)
	{
		tailSig = 0;
	}
	else
	{
		headSig->prev = 0;
	}
	ElemSemT* tmpT = tmpSig->same;
	if(tmpT)
	{
		if(!tmpT->prev)
		{
			headT = tmpT->next;
		}
		else
		{
			tmpT->prev->next = tmpT->next;
		}
		if(tmpT->next)
		{
			tmpT->next->prev = tmpT->prev;
			tmpT->next->waiting += tmpT->waiting;
		}
		delete tmpT;
		tmpT = 0;
	}

	PCB* tmpPCB = tmpSig->p;
	delete tmpSig;
	tmpSig = 0;

	tmpPCB->suspended = 0;
	tmpPCB->waitRet = 1;
	Scheduler::put(tmpPCB);
}
