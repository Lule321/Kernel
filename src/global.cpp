/*
 * global.cpp
 *
 *  Created on: Jun 23, 2021
 *      Author: OS1
 */

#ifndef GLOBAL_CPP_
#define GLOBAL_CPP_
#include "KSem.h"
#include "PCB.h"
#include "Thread.h"

#include "KernelEv.h"
#include "IVTEntry.h"

PCB* running = 0;
unsigned lockFlag = 1;
unsigned switchOnDemand = 0;

unsigned nextId = 0;
unsigned cntThread = 0;
unsigned tableSize = 0;
int* tableId = 0;
PCB** tableThread = 0;


PCB* nothingPCB = 0;

unsigned nextIdSem = 0;
unsigned cntSem = 0;
unsigned tableSizeSem = 0;
int* tableIdSem = 0;
KernelSem** tableSem = 0;

int buffer[30] = {0};//nebitno za prkeojekat
int posPro = 0;
int posCon = 0;

IVTEntry* entryTable[256] = {0};

KernelEv* evTable[256] = {0};

#endif /* GLOBAL_CPP_ */
