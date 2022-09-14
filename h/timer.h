/*
 * timer.h
 *
 *  Created on: Jun 23, 2021
 *      Author: OS1
 */

#ifndef TIMER_H_
#define TIMER_H_


typedef void interrupt (*funInterrupt)(...);

void initTimer();

void restoreTimer();

void interrupt timer(...);

//void tick();

#endif /* TIMER_H_ */
