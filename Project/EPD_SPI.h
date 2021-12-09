/*
 * EPD_SPI.h
 *
 *  Created on: Sep 27, 2021
 *      Author: fnbat
 */

#ifndef EPD_SPI_H_
#define EPD_SPI_H_

#include <msp430.h>
#include "stdint.h"
#include "Game_State.h"

/* Define Display Resolution */
#define EPD_WIDTH   200
#define EPD_HEIGHT  200

//for use with timer flags
extern uint8_t fullChargeFlag; //BIT0=flagADC, BIT1=flagTimer

void Delay_ms(uint16_t millis);
void EPD_Init(void);
void EPD_ClearScreen(void);
void EPD_DisplayImage(const uint8_t *image);
void EPD_DisplayBlack(void);
void EPD_Sleep(void);
void EPD_FullScreen(const uint8_t *image);

#endif /* EPD_SPI_H_ */
