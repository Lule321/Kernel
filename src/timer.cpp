/*
 * timer.cpp
 *
 *  Created on: Jun 23, 2021
 *      Author: OS1
 */

#include "timer.h"
#include <dos.h>
#include "SCHEDULE.H"
#include "PCB.h"
#include "KSem.h"
#include <iostream.h>
#include <stdlib.h>

funInterrupt oldTimer = 0;
volatile int cnt = 2;
volatile unsigned tsp;
volatile unsigned tss;
volatile unsigned tbp;
extern unsigned switchOnDemand;
extern unsigned lockFLag;
extern PCB* running;

extern PCB* nothingPCB;

extern unsigned nextIdSem;
extern unsigned cntSem;
extern unsigned tableSizeSem;
extern int* tableIdSem;
extern KernelSem** tableSem;


#define lock asm{\
					pushf;\
					cli;\
}

#define unlock asm popf

inline void semTimePassed();
inline void singleSemTimePassed(KernelSem* k);

extern void tick();

void initTimer()
{
	lock;
#ifndef BCC_BLOCK_IGNORE

	oldTimer = getvect(0x08);
	setvect(0x08, timer);
	setvect(0x60, oldTimer);
#endif
	unlock;
}

void restoreTimer()
{
	lock;
#ifndef BCC_BLOCK_IGNORE
	if(oldTimer)
	{
	setvect(0x08, oldTimer);
	}
#endif
	unlock;
}

void interrupt timer(...)
{
	//int prevId = running->id;

	if(!switchOnDemand)// pise da nesto ovako treba da se uradi
	{
		tick();
		semTimePassed();
	}
	if(!switchOnDemand && cnt > 0)
	{
		cnt--;
	}
	if(!cnt || switchOnDemand)
	{
		if(lockFlag)
		{
			//cuvanje konteksta
			asm{
				mov tsp, sp
				mov tss, ss
				mov tbp, bp
			}

			running->sp = tsp;
			running->ss = tss;
			running->bp = tbp;



			//ne stavljaj ako se zavrsila ili je suspendovana
			if(!(running -> finished) && !(running->suspended) && running != nothingPCB)
			{
				Scheduler::put(running);
			}



			running = Scheduler::get();

			if(!running)
			{
				running = nothingPCB;
			}

			//vracanje konteksta




			tsp = running->sp;
			tbp = running->bp;
			tss = running->ss;
			cnt = running->timeSlice;




			asm{
				mov ss, tss
				mov sp, tsp
				mov bp, tbp
			}
		}
	}

	if(!switchOnDemand)
	{
		asm int 60h;
	}


	if(!lockFlag)
	{
		switchOnDemand = 1;
	}
	else
	{
		switchOnDemand = 0;
	}



}



inline void semTimePassed()
{
	// funkcija za smanjivanje vremena na semaforima
	int i;
	for(i = 0; i < tableSizeSem; i++)
	{
		if(tableSem[i])
		{
			singleSemTimePassed(tableSem[i]);
		}
	}
}

inline void singleSemTimePassed(KernelSem* k)
{
	//funkcija za smanjivanje vremena na semaforu, ali i vracanje PCB-ova u Scheduler
	KernelSem::ElemSemT* tmpT;
	//KernelSem::ElemSemT* headTmpT = 0; //Da pokusamo sa ovim

	if(k->headT)
	{
		--(k->headT->waiting);
		while(k->headT && k->headT->waiting == 0)
		{
			tmpT = k->headT;
			k->headT = k->headT->next;
			if(k->headT)
			{
				k->headT->prev = 0;
			}

			KernelSem::ElemSemSig* tmpSig = tmpT->same;

			if(!tmpSig->prev)
			{
				k->headSig = tmpSig->next;
				if(!k->headSig)
				{
					k->tailSig = 0;
				}
			}
			else
			{
				tmpSig->prev->next = tmpSig->next;
			}

			if(tmpSig->next)
			{
				tmpSig->next->prev = tmpSig->prev;
			}

			k->value++;

			//tmpT->next = headTmpT;
			//headTmpT = tmpT;

			PCB* tmpPCB = tmpT->p;
			tmpPCB->suspended = 0;
			tmpPCB->waitRet = 0;
			Scheduler::put(tmpPCB);

			delete tmpT;
			tmpT = 0;
			delete tmpSig;
			tmpSig = 0;
		}
	/*	while(headTmpT)
		{
			//cout << "Ovde sam: " << (void*)headTmpT;
			tmpT = headTmpT;
			headTmpT = headTmpT->next;
			PCB* tmpPCB = tmpT->p;
			tmpPCB->suspended = 0;
			tmpPCB->waitRet = 0;
			Scheduler::put(tmpPCB);
			delete tmpT;
			tmpT = 0;

		}*/
	}

}
