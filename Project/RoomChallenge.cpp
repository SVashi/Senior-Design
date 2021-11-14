#include <stdio.h>
#include <msp430.h>
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
    initGpio();
    while(count < 5)
    {
        if(!P5IN)
        {
            __delay_cycles(400000);
            count = count + 1;
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
    }
    else
    {
        printf("Failure with a value of: %d\n", buttonPresses);
    }
}


