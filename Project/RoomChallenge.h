#include <ctpl.h>
#include <msp430.h>
#include "EPD_SPI.h"
#include "Game_State.h"
#include "CrankChallenge.h"
#include "IMG.h"
void RoomChallenge();
bool RoomChallenge1(uint8_t minButtonVal, uint16_t minTimeVal);
bool RoomChallenge2(uint8_t minButtonVal, uint16_t minTimeVal);
