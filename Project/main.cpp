#include <msp430.h>
#include <stdio.h>
#include <stdlib.h>
#include "EPD_SPI.h"
#include "IMG.h"
#include "RoomChallenge.h"
#include "BossBattle.h"
using namespace std;

int main() {
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    SPI_Init();

    // XT1 Setup
    CSCTL0_H = CSKEY_H;                     // Unlock CS registers
    CSCTL1 = DCOFSEL_0;                     // Set DCO to 1MHz
    CSCTL2 = SELA__LFXTCLK | SELS__DCOCLK | SELM__DCOCLK; // Set ACLK = XT1; MCLK = DCO
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;   // Set all dividers to 1
    CSCTL4 &= ~LFXTOFF;
    do
    {
        CSCTL5 &= ~LFXTOFFG;                // Clear XT1 fault flag
        SFRIFG1 &= ~OFIFG;
    }while (SFRIFG1 & OFIFG);               // Test oscillator fault flag
    CSCTL0_H = 0;                           // Lock CS registers

    EPD_Init();
    EPD_ClearScreen();
    Delay_ms(100);
    EPD_DisplayImage(IMAGE_SPLASH);
    EPD_Sleep();

    while(P5IN){;} //pretend generation charge

    printf("Room Select\n");

    EPD_Init();
    EPD_ClearScreen();
    Delay_ms(100);
    EPD_DisplayImage(IMAGE_SELROOM);
    EPD_Sleep();

    RoomChallenge();
    BossBattle(7,0);
    return 0;
}
