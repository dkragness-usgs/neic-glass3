/*
 * librdkafka - Apache Kafka C library
 *
 * Copyright (c) 2012-2013, Magnus Edenhill
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "rdkafka_int.h"
#include "rd.h"
#include "rdtime.h"
#include "rdsysqueue.h"


static RD_INLINE void rd_kafka_timers_lock (rd_kafka_timers_t *rkts) {
        mtx_lock(&rkts->rkts_lock);
}

static RD_INLINE void rd_kafka_timers_unlock (rd_kafka_timers_t *rkts) {
        mtx_unlock(&rkts->rkts_lock);
}


static RD_INLINE int rd_kafka_timer_started (const rd_kafka_timer_t *rtmr) {
	return rtmr->rtmr_interval ? 1 : 0;
}


static RD_INLINE int rd_kafka_timer_scheduled (const rd_kafka_timer_t *rtmr) {
	return rtmr->rtmr_next ? 1 : 0;
}


static int rd_kafka_timer_cmp (const void *_a, const void *_b) {
	const rd_kafka_timer_t *a = _a, *b = _b;
	return (int)(a->rtmr_next - b->rtmr_next);
}

static void rd_kafka_timer_unschedule (rd_kafka_timers_t *rkts,
                                       rd_kafka_timer_t *rtmr) {
	TAILQ_REMOVE(&rkts->rkts_timers, rtmr, rtmr_link);
	rtmr->rtmr_next = 0;
}

static void rd_kafka_timer_schedule (rd_kafka_timers_t *rkts,
				     rd_kafka_timer_t *rtmr, int extra_us) {
	rd_kafka_timer_t *first;

	/* Timer has been stopped */
	if (!rtmr->rtmr_interval)
		return;

        /* Timers framework is terminating */
        if (unlikely(!rkts->rkts_enabled))
                return;

	rtmr->rtmr_next = rd_clock() + rtmr->rtmr_interval + extra_us;

	if (!(first = TAILQ_FIRST(&rkts->rkts_timers)) ||
	    first->rtmr_next > rtmr->rtmr_next) {
		TAILQ_INSERT_HEAD(&rkts->rkts_timers, rtmr, rtmr_link);
                cnd_signal(&rkts->rkts_cond);
	} else
		TAILQ_INSERT_SORTED(&rkts->rkts_timers, rtmr,
                                    rd_kafka_timer_s, rtmr_link,
				    rd_kafka_timer_cmp);
}

/**
 * Stop a timer that may be started.
 * If called from inside a timer callback 'lock' must be 0, else 1.
 */
void rd_kafka_timer_stop (rd_kafka_timers_t *rkts, rd_kafka_timer_t *rtmr,
                          int lock) {
	if (lock)
		rd_kafka_timers_lock(rkts);

	if (!rd_kafka_timer_started(rtmr)) {
		if (lock)
			rd_kafka_timers_unlock(rkts);
		return;
	}

	if (rd_kafka_timer_scheduled(rtmr))
		rd_kafka_timer_unschedule(rkts, rtmr);

	rtmr->rtmr_interval = 0;

	if (lock)
		rd_kafka_timers_unlock(rkts);
}


/**
 * Start the provided timer with the given interval.
 * Upon expiration of the interval (us) the callback will be called in the
 * main rdkafka thread, after callback return the timer will be restarted.
 *
 * Use rd_kafka_timer_stop() to stop a timer.
 */
void rd_kafka_timer_start (rd_kafka_timers_t *rkts,
			   rd_kafka_timer_t *rtmr, int interval,
			   void (*callback) (rd_kafka_timers_t *rkts, void *arg),
			   void *arg) {
	rd_kafka_timers_lock(rkts);

	rd_kafka_timer_stop(rkts, rtmr, 0/*!lock*/);

	rtmr->rtmr_interval = interval;
	rtmr->rtmr_callback = callback;
	rtmr->rtmr_arg      = arg;

	rd_kafka_timer_schedule(rkts, rtmr, 0);

	rd_kafka_timers_unlock(rkts);
}


/**
 * Delay the next timer invocation by 'backoff_us'
 */
void rd_kafka_timer_backoff (rd_kafka_timers_t *rkts,
			     rd_kafka_timer_t *rtmr, int backoff_us) {
	rd_kafka_timers_lock(rkts);
	if (rd_kafka_timer_scheduled(rtmr))
		rd_kafka_timer_unschedule(rkts, rtmr);
	rd_kafka_timer_schedule(rkts, rtmr, backoff_us);
	rd_kafka_timers_unlock(rkts);
}


/**
 * Interrupt rd_kafka_timers_run().
 * Used for termination.
 */
void rd_kafka_timers_interrupt (rd_kafka_timers_t *rkts) {
	rd_kafka_timers_lock(rkts);
	cnd_signal(&rkts->rkts_cond);
	rd_kafka_timers_unlock(rkts);
}


/**
 * Returns the delta time to the next timer to fire, capped by 'timeout_ms'.
 */
rd_ts_t rd_kafka_timers_next (rd_kafka_timers_t *rkts, int timeout_us,
			      int do_lock) {
	rd_ts_t now = rd_clock();
	rd_ts_t sleeptime = 0;
	rd_kafka_timer_t *rtmr;

	if (do_lock)
		rd_kafka_timers_lock(rkts);

	if (likely((rtmr = TAILQ_FIRST(&rkts->rkts_timers)) != NULL)) {
		sleeptime = rtmr->rtmr_next - now;
		if (sleeptime < 0)
			sleeptime = 0;
		else if (sleeptime > (rd_ts_t)timeout_us)
			sleeptime = (rd_ts_t)timeout_us;
	} else
		sleeptime = (rd_ts_t)timeout_us;

	if (do_lock)
		rd_kafka_timers_unlock(rkts);

	return sleeptime;
}


/**
 * Dispatch timers.
 * Will block up to 'timeout' microseconds before returning.
 */
void rd_kafka_timers_run (rd_kafka_timers_t *rkts, int timeout_us) {
	rd_ts_t now = rd_clock();
	rd_ts_t end = now + timeout_us;

        rd_kafka_timers_lock(rkts);

	while (!rd_atomic32_get(&rkts->rkts_rk->rk_terminate) && now <= end) {
		int64_t sleeptime;
		rd_kafka_timer_t *rtmr;

		if (timeout_us != RD_POLL_NOWAIT) {
			sleeptime = rd_kafka_timers_next(rkts,
							 timeout_us,
							 0/*no-lock*/);

			if (sleeptime > 0) {
				cnd_timedwait_ms(&rkts->rkts_cond,
						 &rkts->rkts_lock,
						 (int)(sleeptime / 1000));

			}
		}

		now = rd_clock();

		while ((rtmr = TAILQ_FIRST(&rkts->rkts_timers)) &&
		       rtmr->rtmr_next <= now) {

			rd_kafka_timer_unschedule(rkts, rtmr);
                        rd_kafka_timers_unlock(rkts);

			rtmr->rtmr_callback(rkts, rtmr->rtmr_arg);

                        rd_kafka_timers_lock(rkts);
			/* Restart timer, unless it has been stopped, or
			 * already reschedueld (start()ed) from callback. */
			if (rd_kafka_timer_started(rtmr) &&
			    !rd_kafka_timer_scheduled(rtmr))
				rd_kafka_timer_schedule(rkts, rtmr, 0);
		}

		if (timeout_us == RD_POLL_NOWAIT) {
			/* Only iterate once, even if rd_clock doesn't change */
			break;
		}
	}

	rd_kafka_timers_unlock(rkts);
}


void rd_kafka_timers_destroy (rd_kafka_timers_t *rkts) {
        rd_kafka_timer_t *rtmr;

        rd_kafka_timers_lock(rkts);
        rkts->rkts_enabled = 0;
        while ((rtmr = TAILQ_FIRST(&rkts->rkts_timers)))
                rd_kafka_timer_stop(rkts, rtmr, 0);
        rd_kafka_assert(rkts->rkts_rk, TAILQ_EMPTY(&rkts->rkts_timers));
        rd_kafka_timers_unlock(rkts);

        cnd_destroy(&rkts->rkts_cond);
        mtx_destroy(&rkts->rkts_lock);
}

void rd_kafka_timers_init (rd_kafka_timers_t *rkts, rd_kafka_t *rk) {
        memset(rkts, 0, sizeof(*rkts));
        rkts->rkts_rk = rk;
        TAILQ_INIT(&rkts->rkts_timers);
        mtx_init(&rkts->rkts_lock, mtx_plain);
        cnd_init(&rkts->rkts_cond);
        rkts->rkts_enabled = 1;
}