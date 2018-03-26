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

#include "Timer.h"

void print_timer_counter(Timer* timer)
{
	uint64_t counter_value = 0;
	//double time;
	//double time_sec;

/*    printf("Counter: 0x%08x%08x\n", (uint32_t) (counter_value >> 32),
                                    (uint32_t) (counter_value));*/
    
    timer_get_counter_value(TIMER_GROUP_0, CAYENNE_TIMER_IDX, &counter_value);
    //timer_get_counter_time_sec(TIMER_GROUP_0, CAYENNE_TIMER_IDX, &counter_value);
    

    
    if(counter_value <= 0){
    	timer_pause(TIMER_GROUP_0, CAYENNE_TIMER_IDX);
    	timer->counter_value = 0;
 	
    }else{
    	//printf("ATime   : %f s | ", time_sec);
    	//printf("BTime   : %f \n", time / TIMER_SCALE);

    	printf("Counter: 0x%08x%08x\n", (uint32_t) (counter_value >> 32),
                                    (uint32_t) (counter_value));
    	printf("Time   : %.8f s\n", (double) counter_value / TIMER_SCALE);
    }
}

void TimerInit(Timer* timer)
{
	/* Select and initialize basic parameters of the timer */
    timer_config_t config;
    config.divider = TIMER_DIVIDER;
    config.counter_dir = TIMER_COUNT_UP;
    config.counter_en = TIMER_PAUSE;
    config.alarm_en = TIMER_ALARM_DIS;
    config.intr_type = TIMER_INTR_LEVEL;
    config.auto_reload = TIMER_AUTORELOAD_DIS;
    timer_init(TIMER_GROUP_0, CAYENNE_TIMER_IDX, &config);
}

char TimerIsExpired(Timer* timer)
{
	struct timeval now, res;
	gettimeofday(&now, NULL);
	timersub(&timer->end_time, &now, &res);		
	return res.tv_sec < 0 || (res.tv_sec == 0 && res.tv_usec <= 0);
}


void TimerCountdownMS(Timer* timer, unsigned int timeout)
{
	/* Timer's counter will initially start from value below.
       Also, if auto_reload is set, this value will be automatically reload on alarm */
    timer_set_counter_value(TIMER_GROUP_0, CAYENNE_TIMER_IDX, (double)timeout * TIMER_SCALE / 1000);
    timer_start(TIMER_GROUP_0, CAYENNE_TIMER_IDX);
    printf("Timer Start\n");
}


void TimerCountdown(Timer* timer, unsigned int timeout)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	struct timeval interval = {timeout, 0};
	timeradd(&now, &interval, &timer->end_time);
}


int TimerLeftMS(Timer* timer)
{
	struct timeval now, res;
	gettimeofday(&now, NULL);
	timersub(&timer->end_time, &now, &res);
	//printf("left %d ms\n", (res.tv_sec < 0) ? 0 : res.tv_sec * 1000 + res.tv_usec / 1000);
	return (res.tv_sec < 0) ? 0 : res.tv_sec * 1000 + res.tv_usec / 1000;
}
