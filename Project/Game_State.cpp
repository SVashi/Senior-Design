/*
 * Game_State.cpp
 *
 *  Created on: Dec 1, 2021
 *      Author: fnbates
 */
#include "Game_State.h"

#pragma PERSISTENT
    gameState GameState = {0};

uint8_t getState(){
    return GameState.state;
}

void clearGame(){
    setState(0);
    clearButtonPress();
    clearSucceed();
    clearFail();
    clearRoomChallenge();
}

void setState(uint8_t stateUpdate){
    GameState.oldState = GameState.state;
    GameState.state = stateUpdate;
}

uint8_t getOldState(){
    return GameState.oldState;
}

uint8_t getReturnState(){
    return GameState.returnState;
}

void setReturnState(){
    GameState.returnState = GameState.state;
}

uint8_t getButtonPress(int _buttonNum){
    int buttonNum = _buttonNum;
    uint8_t numPresses;
    switch(buttonNum){
    case 1: numPresses = GameState.b1Press; break;
    case 2: numPresses = GameState.b2Press; break;
    case 3: numPresses = GameState.b3Press; break;
    case 4: numPresses = GameState.b4Press; break;
    }
    return numPresses;
}

void clearButtonPress(){
    GameState.b1Press = 0;
    GameState.b2Press = 0;
    GameState.b3Press = 0;
    GameState.b4Press = 0;
}

void addButtonPress(int _buttonNum){
    int buttonNum = _buttonNum;
    switch(buttonNum){
        case 1: GameState.b1Press +=1; break;
        case 2: GameState.b2Press +=1; break;
        case 3: GameState.b3Press +=1; break;
        case 4: GameState.b4Press +=1; break;
        }
}

uint8_t getSucceed(){
    return GameState.numSucceed;
}

void clearSucceed(){
    GameState.numSucceed = 0;
}

void addSucceed(){
    GameState.numSucceed +=1;
}

uint8_t getFail(){
    return GameState.numFail;
}

void clearFail(){
    GameState.numFail = 0;
}

void addFail(){
    GameState.numFail +=1;
}

uint8_t getRoomChallenge(){
    return GameState.roomChallenge;
}

void clearRoomChallenge(){
    GameState.roomChallenge = 0;
}

void addRoomChallenge(){
    GameState.roomChallenge +=1;
}

void subRoomChallenge(){
    GameState.roomChallenge -=1;
}
