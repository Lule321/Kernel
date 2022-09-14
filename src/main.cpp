/*
 * main.cpp
 *
 *  Created on: Jun 23, 2021
 *      Author: OS1
 */
#include "PCB.h"
#include "timer.h"
#include <iostream.h>

#include "SCHEDULE.H"
#include "glFun.h"
#include "KSem.h"
#include "Thread.h"
#include "semaphor.h"
#include "Event.h"
#include "uThread.h"

extern unsigned switchOnDemand;
extern unsigned lockFLag;
extern PCB* running;


extern unsigned nextId;
extern unsigned cntThread;
extern unsigned tableSize;
extern int* tableId;
extern PCB** tableThread;
const int tableInitSize = 64;

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

void inline initSemTable();
void inline initThreadTable();
void inline cleanMemory();
void nothing();


int main(int argc, char* argv[])
{


	PCB* mainPCB;
	uThread* userThread = 0;

	int ret;
	lock;
	initTimer();
	initThreadTable();
	initSemTable();
	mainPCB = new PCB();
	nothingPCB = new PCB(1, 1024, nothing, 0);
	running = mainPCB;
	unlock;

	lock;
	userThread = new uThread(argc, argv);
	unlock;


	userThread->start();



	userThread->waitToComplete();



	ret = userThread->retValue();



	lock;
	cleanMemory();
	unlock;

	restoreTimer();

	delete mainPCB;
	delete nothingPCB;
	delete userThread;


	return ret;
}

void inline initThreadTable()
{
	int i;
	nextId = 0;
	cntThread = 0;
	tableSize = tableInitSize;
	tableId = new int[tableSize];
	tableThread = new PCB*[tableSize];
	for(i = 0; i < tableSize; i++)
	{
		tableId[i] = 1;
		tableThread[i] = 0;
	}
}

void inline initSemTable()
{
	int i;
	nextIdSem = 0;
	cntSem = 0;
	tableSizeSem = tableInitSize;
	tableIdSem = new int[tableSizeSem];
	tableSem = new KernelSem*[tableSizeSem];
	for(i = 0; i < tableSizeSem; i++)
	{
		tableIdSem[i] = 1;
		tableSem[i] = 0;
	}
}

void inline cleanMemory()
{
	delete[] tableId;
	delete[] tableThread;
	delete[] tableIdSem;
	delete[] tableSem;
}

void nothing()
{
	int j, k;
	while(1)
	{
		/*
		lock;
		cout << "Ne bi trebalo do ovde da dodjes, osim ako ne radis sa semaforom" << endl;
		unlock;

		for(j = 0; j < 30000; j++)
		{
			for(k =0 ; k < 30000; k++);
		}
	*/
	}
}
