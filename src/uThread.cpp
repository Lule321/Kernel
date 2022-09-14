/*
 * uThread.cpp
 *
 *  Created on: Jun 28, 2021
 *      Author: OS1
 */

#include "uThread.h"
#include <iostream.h>

int userMain(int argc, char* argv[]);

uThread::uThread(int argc, char** argv, unsigned long stackSize, unsigned timeSlice):Thread(stackSize, timeSlice)
{
	this->argc = argc;
	this->argv = argv;
	this->ret = 0;
	this->tSlice = timeSlice;
	this->sSize = stackSize;
}

uThread::~uThread()
{

}

void uThread::run()
{
	this->ret = userMain(argc, argv);
}

int uThread::retValue() const
{
	return ret;
}

Thread* uThread::clone() const
{
	return new uThread(argc, argv, sSize, tSlice);
}
