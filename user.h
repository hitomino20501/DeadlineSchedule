/*
 * user.h
 *
 *  Created on: 2021¦~10¤ë13¤é
 *      Author: FoSky
 */

#ifndef USER_H_
#define USER_H_

struct User{
    std::string name;
    int priority;
    int userIndex = 0;
    int userErrorFrame = 0;
    int userFinishFrame = 0;
    int userRenderingFrame = 0;
    double userWeight = 0.0;
    int finishJob = 0;
    int totalJob = 0;
    int renderingJob = 0;
    int denominator = 0;
    int proportion = 0;
    int limitUserWeight = -1;
    std::string userColor;
};

#endif /* USER_H_ */
