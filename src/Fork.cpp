/*
 * Fork.cpp
 *
 *  Created on: Jun 30, 2021
 *      Author: OS1
 */

#include "Fork.h"
#include <dos.h>
#include <iostream.h>
#include "PCB.h"
#include "SCHEDULE.H"
#define lock asm{\
					pushf;\
					cli;\
}

#define unlock asm popf

Fork::Fork(PCB* f, PCB* c, unsigned stackSize, unsigned timeSlice):Thread(stackSize, timeSlice)
{
	father = f;
	child = c;
}

Fork::~Fork()
{

}

void Fork::run()
{
	unsigned tmpOFF;
	unsigned tmpREG;
	int i;
	int first = 0;
	unsigned tmpI;

	tmpREG = father->bp;


	for(i = 0; tmpREG != FP_OFF(father->stack + i); i++);
	/*
	int j = i;
	for(; i < father->stackSize; i++)
	{
		lock;
		cout << i << ": " << father->stack[i] << endl;
		cout << i << ": " << FP_OFF(father->stack + i) << endl;
		unlock;
	}

	i = j;
	*/

	for(; i < father->stackSize; i++)
	{
		tmpOFF = FP_OFF(father->stack + i);
		if(tmpREG == tmpOFF)
		{
			if(!first)
			{
				child->bp = FP_OFF(child->stack + i);
				child->sp = FP_OFF(child->stack + i);
				child->ss = FP_SEG(child->stack + i);
				first = 1;
			}
			else
			{
				child->stack[tmpI] = FP_OFF(child->stack + i);
			}
			tmpI = i;
			tmpREG = father->stack[i];
		}
		else
		{
			child->stack[i] = father->stack[i];
		}
	}

	/*
	tmpREG = child->bp;
	lock;
	cout << "---------------------------" << endl;
	cout << tmpREG << endl;
	cout << child->stackSize << endl;
	unlock;

	for(i = 0; tmpREG != FP_OFF(child->stack + i); i++);
	for(; i < child->stackSize; i++)
	{
		lock;
		cout << i << ": " << child->stack[i] << endl;
		cout << i << ": " << FP_OFF(child->stack + i) << endl;
		unlock;
	}*/

	lock;
	Scheduler::put(child);
	unlock;
}

Thread* Fork::clone() const
{
	return 0; //clone not allowed
}

