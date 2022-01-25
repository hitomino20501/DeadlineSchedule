/*
 * task.h
 *
 *  Created on: 2022¦~1¤ë23¤é
 *      Author: FoSky
 */

#ifndef TASK_H_
#define TASK_H_

struct Task{
    int taskIndex = 0;
    int slaveId = 0;
    bool isDispatch = false;
    bool isFinish = false;
    omnetpp::simtime_t startTime = 0;
    omnetpp::simtime_t finisdTime = 0;
    omnetpp::simtime_t renderTime = 0;
};
#endif /* TASK_H_ */
