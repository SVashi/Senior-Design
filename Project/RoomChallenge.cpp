#include <stdio.h>
#include <msp430.h>
#include "EPD_SPI.h"
#include "IMG.h"
using namespace std;

void initGpio(void);

void initGpio(void)
{
    /* Configure P5.6 for hi/lo transition interrupt. */
    P5OUT |= BIT6;
    P5REN |= BIT6;
    P5DIR = 0xFF ^ BIT6;
    P5IES |= BIT6;
    P5IE |= BIT6;

    /* Disable the GPIO power-on default high-impedance mode. */
    PM5CTL0 &= ~LOCKLPM5;

    /* Clear pending interrupts. */
    P5IFG = 0;
}

//Placeholder for proper data retrievals
int getData()
{
    int count = 0;
    bool buttonPressed = false;
    while(count < 5)
    {
        if(!P5IN && !buttonPressed)
        {
            count = count + 1;
            buttonPressed = true;
        }
        if (P5IN && buttonPressed)
        {
            buttonPressed = false;
        }
    }
    return count;
}

void RoomChallenge()
{
    int buttonMinimumValue = 5;
    int buttonPresses;

    buttonPresses = getData();
    if (buttonPresses >= buttonMinimumValue)
    {
        printf("Success with a value of: %d\n", buttonPresses);
        EPD_Init();
        EPD_ClearScreen();
        Delay_ms(100);
        EPD_DisplayImage(IMAGE_BOSS1);
        EPD_Sleep();
    }
    else
    {
        printf("Failure with a value of: %d\n", buttonPresses);
    }
}


