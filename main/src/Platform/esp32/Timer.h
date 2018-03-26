/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Allan Stockdill-Mander - initial API and implementation and/or initial documentation
 *******************************************************************************/

#if !defined(__TIMER_h)
#define __TIMER_h

#include <stdio.h>

#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "soc/timer_group_struct.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"

#define TIMER_DIVIDER         16  //  Hardware timer clock divider
#define TIMER_SCALE           (TIMER_BASE_CLK / TIMER_DIVIDER)  // convert counter value to seconds

#define CAYENNE_TIMER_IDX 	  TIMER_0
#define CAYENNE_TIMER_GROUP   TIMER_GROUP_0

//#include <sys/time.h>
 /**
 * Countdown timer struct.
 */
static timer_idx_t timers_counter = 0;

typedef struct Timer
{
	uint64_t timeout;
	uint64_t counter_value; /**< Countdown end time. */
	timer_idx_t idx;
	//struct timeval end_time; /**< Countdown end time. */
} Timer;


														
/**
* Initialize countdown timer.
* @param[in] timer Pointer to Timer struct
*/
void TimerInit(Timer* timer);

/**
* The countdown timer has expired.
* @param[in] timer Pointer to Timer struct
* @return 1 if countdown has expired, 0 otherwise.
*/
char TimerIsExpired(Timer* timer);

/**
* Start countdown in milliseconds.
* @param[in] timer Pointer to Timer struct
* @param[in] timeout Number of milliseconds to count down.
*/
void TimerCountdownMS(Timer* timer, unsigned int timeout);

/**
* Start countdown in seconds.
* @param[in] timer Pointer to Timer struct
* @param[in] timeout Number of seconds to count down.
*/
void TimerCountdown(Timer* timer, unsigned int timeout);

/**
* Get the number of milliseconds left in countdown.
* @param[in] timer Pointer to Timer struct
* @return Number of milliseconds left.
*/
int TimerLeftMS(Timer* timer);

#endif
