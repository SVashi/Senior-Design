#include "CrankChallenge.h"

//sits in this stage forever until supercapacitor is charge to 4 volts for x amount of time
void CrankChallenge(uint16_t millis){
    P5IE  &= ~(BIT1 | BIT3 | BIT6 | BIT7); // disable button interrupts

    ADC12IER2 &= ~ADC12LOIE; //disable low side ADC interrupt
    //enable high level comparator
    ADC12IER2 |= ADC12HIIE;
    ADC12IER2 &= ~ADC12INIE;
    ADC12IFGR2 &= ~(ADC12INIFG|ADC12HIIFG);
    while(1){
        fullChargeFlag = 0;
        //wait until super-capacitor charged to high voltage threshold
        while(!fullChargeFlag&BIT0){
            __low_power_mode_3();
        }

        //Initialize Timer_B0 to use as delay counter
        TB0CCTL0 = CCIE_0;                            // TBCCR0 interrupt disabled
        TB0CCR0 = 0;                                // Set CCR to 0
        TB0CTL = TBSSEL__ACLK | ID__8 | MC__UP;     // ACLK, Divided 8, up mode
        TB0EX0 = TBIDEX_3;                          // ACLK = 32/8/4 = 1kHz = 1ms/count
        TB0CCR0 = millis;                           // Set CCR to delay time
        TB0CCTL0 = CCIE;                            // TBCCR0 interrupt enabled

        while(fullChargeFlag == 0x01){
            __low_power_mode_3();
        }
        //if timer finsihed without low voltage detected
        //resume gameplay
        if(fullChargeFlag&BIT0){
            break;
        }
    }

     //disable high level ADC comparator
     ADC12IER2 &= ~(ADC12INIE|ADC12HIIE);
     ADC12IFGR2 &= ~(ADC12INIFG|ADC12HIIFG|ADC12LOIFG); //reset all ADC interrupts

     P5IFG = 0; //reset button presses
     P5IE  |= BIT1 | BIT3 | BIT6 | BIT7; //enable button interrupts
     ADC12IER2 |= ADC12LOIE; //enable low side ADC interrupts
}
