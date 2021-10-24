/*
 * user.h
 *
 *  Created on: 2021�~10��13��
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
    int userWeight = 0;
    int finishJob = 0;
    int totalJob = 0;
    int renderingJob = 0;
    std::string userColor;
};

#endif /* USER_H_ */
