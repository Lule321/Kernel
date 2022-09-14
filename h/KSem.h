/*
 * KernelSem.h
 *
 *  Created on: Jun 25, 2021
 *      Author: OS1
 */

#ifndef KSEM_H_
#define KSEM_H_
#include "semaphor.h"
#include "PCB.h"
#include "SCHEDULE.H"



class KernelSem {
public:
	Semaphore* mySem;
	int value;
	unsigned id;
	struct ElemSemT;
	struct ElemSemSig
	{
		PCB* p;
		ElemSemSig* next;
		ElemSemSig* prev;
		ElemSemT* same;
	};

	struct ElemSemT
	{
		PCB* p;
		ElemSemSig* same;
		ElemSemT* next;
		ElemSemT* prev;
		int waiting;
	};

	ElemSemSig* headSig; //cekanje na signal
	ElemSemSig* tailSig;

	ElemSemT* headT; //vremensko cekanje
	//ElemSemT* tailT;




	KernelSem(int v, Semaphore* s);
	~KernelSem();

	int val() const;

	int wait(int t);

	void block(int t);

	void signal();

	void unblock();

};

#endif /* KSEM_H_ */
