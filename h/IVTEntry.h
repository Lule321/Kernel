/*
 * IVTEntry.h
 *
 *  Created on: Jun 27, 2021
 *      Author: OS1
 */

#ifndef IVTENTRY_H_
#define IVTENTRY_H_
#include <dos.h>
#include "Thread.h"

class IVTEntry;


typedef void interrupt (*funInterrupt)(...);

class IVTEntry {
public:

	IVTEntry(unsigned char num, funInterrupt f);
	~IVTEntry();
	void signal();

	static IVTEntry* getByEntryNum(unsigned char i);

	funInterrupt oldFun;
	funInterrupt newFun;
	unsigned char num;

};

#define PREPAREENTRY(entryNum, callOld)\
	void interrupt inter##entryNum(...);\
	IVTEntry entry##entryNum(entryNum, inter##entryNum);\
	void interrupt inter##entryNum(...){\
		entry##entryNum.signal();\
		if(callOld){\
			entry##entryNum.oldFun();\
		}\
		dispatch();\
	}
#endif /* IVTENTRY_H_ */
