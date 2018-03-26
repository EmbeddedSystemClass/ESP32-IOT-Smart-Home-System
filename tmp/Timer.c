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
    timer->idx = TIMER_0;
    //timers_counter++;
    timer_init(CAYENNE_TIMER_GROUP, timer->idx, &config);
}

char TimerIsExpired(Timer* timer)
{
	//uint64_t counter_value = 0;

	timer_get_counter_value(CAYENNE_TIMER_GROUP, timer->idx, &timer->counter_value);
    //timer_get_counter_time_sec(CAYENNE_TIMER_GROUP, CAYENNE_TIMER_IDX, &counter_value);
    
    timer->counter_value = timer->counter_value / TIMER_SCALE;
    
    if(timer->counter_value > timer->timeout){
    	timer_pause(CAYENNE_TIMER_GROUP, timer->idx);
    	timer->counter_value = 0;
 	
 		return 1;
    }else{
    	return 0;
    }
}


void TimerCountdownMS(Timer* timer, unsigned int timeout)
{
    timer->timeout = (double)timeout/(double)1000;
	timer_set_counter_value(CAYENNE_TIMER_GROUP, timer->idx, 0x00000000ULL);

    timer_start(CAYENNE_TIMER_GROUP, timer->idx);
}


void TimerCountdown(Timer* timer, unsigned int timeout)
{

    timer->timeout = (double)timeout;
	timer_set_counter_value(CAYENNE_TIMER_GROUP, timer->idx, 0x00000000ULL);

    timer_start(CAYENNE_TIMER_GROUP, timer->idx);

}


int TimerLeftMS(Timer* timer)
{
	return timer->counter_value*1000;
}
