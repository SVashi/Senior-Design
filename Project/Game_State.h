/*
 * Game_State.h
 *
 *  Created on: Dec 1, 2021
 *      Author: fnbat
 */

#ifndef GAME_STATE_H_
#define GAME_STATE_H_

#include "stdint.h"

typedef struct GameState {
    uint8_t state;
    uint8_t oldState;
    uint8_t b1Press;
    uint8_t b2Press;
    uint8_t b3Press;
    uint8_t b4Press;
    uint8_t numSucceed;
    uint8_t numFail;
    uint8_t roomChallenge;
}gameState;

extern gameState GameState;

uint8_t getState();
void setState(uint8_t stateUpdate);
void setOldState();
void clearGame();
uint8_t getButtonPress(int buttonNum);
void clearButtonPress();
void addButtonPress(int buttonNum);
uint8_t getSucceed();
void clearSucceed();
void addSucceed();
uint8_t getFail();
void clearFail();
void addFail();
uint8_t getRoomChallenge();
void clearRoomChallenge();
void addRoomChallenge();


#endif /* GAME_STATE_H_ */
