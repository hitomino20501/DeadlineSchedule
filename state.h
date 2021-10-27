/*
 * state.h
 *
 *  Created on: 2021¦~10¤ë13¤é
 *      Author: FoSky
 */

#ifndef STATE_H_
#define STATE_H_

enum WorkerState{
   FRAME_WAITING = 0,
   FRAME_RUNNING = 1,
   FRAME_SUCCEEDED = 2,
   FRAME_RETRY = 3,
   FRAME_ERROR = 4,
   REQUEST_JOB = 5,
   Dispatch_JOB = 6,
   SUBMIT_JOB = 7,
   NO_Dispatch_JOB = 8,
   LOG_TIMER = 9,
   SHUT_DOWN_SLAVE = 10,
   JOB_START = 11,
   JOB_FINISH = 12,
};

#endif /* STATE_H_ */
