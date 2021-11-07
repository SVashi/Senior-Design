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

/* Define Display Resolution */
#define EPD_WIDTH   200
#define EPD_HEIGHT  200

void SPI_Init(void);
void Delay_ms(uint16_t millis);
void EPD_Init(void);
void EPD_ClearScreen(void);
void EPD_DisplayImage(const uint8_t *Image);
void EPD_DisplayBlack(void);
void EPD_Sleep(void);

#endif /* EPD_SPI_H_ */
