/*
 * IVTEntry.cpp
 *
 *  Created on: Jun 27, 2021
 *      Author: OS1
 */

#include "IVTEntry.h"
#include "KernelEv.h"
#include <iostream.h>

extern KernelEv* evTable[256];
extern IVTEntry* entryTable[256];

#define lock asm{\
					pushf;\
					cli;\
}

#define unlock asm popf



IVTEntry::IVTEntry(unsigned char num, funInterrupt f)
{
	this->num = num;
	this->newFun = f;
	lock;
	if(entryTable[num])
	{
		//delete tableEntry[num];
		setvect(num, entryTable[num]->oldFun); //izmena
		entryTable[num] = 0;
	}
	this->oldFun = getvect(num);
	setvect(num, f);
	entryTable[num] = this;
	unlock;
}

IVTEntry::~IVTEntry()
{
	lock;
	entryTable[num] = 0;
	setvect(num, oldFun);
	unlock;
}

IVTEntry* IVTEntry::getByEntryNum(unsigned char i)
{
	return entryTable[i];
}

void IVTEntry::signal()
{
	evTable[num]->signal();
}
