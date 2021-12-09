#include <msp430.h>
#include "EPD_SPI.h"
#include "IMG.h"
#include "RoomChallenge.h"
#include "BossBattle.h"
#include "Game_State.h"
#include <ctpl.h>


//Vout = Vin * (R2/(R1+R2))
#define R2                         10.0
#define R1                         11.0
#define ADC_MONITOR_THRESHOLD_HI   4
#define ADC_MONITOR_THRESHOLD_LO   3.2

#define ADC_MONITOR_FREQUENCY   100

#define MCLK_FREQUENCY          1000000
#define SMCLK_FREQUENCY         1000000
#define ACLK_FREQUENCY          32000

#define B1  BIT1
#define B2  BIT3
#define B3  BIT6
#define B4  BIT7

#define NUM_CHALLENGES  2

void GPIO_Init(void);
void SPI_Init(void);
void CLK_Init(void);
void ADC_Init(void);
void Comp_Init(void);

uint8_t fullChargeFlag = 0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;    // Stop watchdog timer

    /* Initialize peripherals */
    GPIO_Init();
    SPI_Init();
    CLK_Init();
    ADC_Init();
    //Comp_Init();
    __enable_interrupt();

    while(1){
        switch(getState()){
            case 0 : EPD_FullScreen(IMAGE_SPLASH);
                     if(fullChargeFlag&BIT2) break; //break for low power
                     __low_power_mode_3();
                     if(fullChargeFlag&BIT2) break; //break for low power
                     setState(1);
                     break;
            case 1 : EPD_FullScreen(IMAGE_SELROOM);
                     if(fullChargeFlag&BIT2) break; //break for low power
                     __low_power_mode_3();
                     if(fullChargeFlag&BIT2) break; //break for low power
                     setState(2);
                     break;
            case 2 : RoomChallenge();
                     if(fullChargeFlag&BIT2) break; //break for low power
                     setState(3);
                     break;
            case 3 : EPD_FullScreen(IMAGE_RMSUC);
                     if(fullChargeFlag&BIT2) break; //break for low power
                     __low_power_mode_3();
                     if(fullChargeFlag&BIT2) break; //break for low power
                     //decide if boss should be next
                     if(getSucceed() < NUM_CHALLENGES){
                         setState(2);
                     } else {
                         setState(4);
                     }
                     break;
            case 4 : BossBattle();
                     if(fullChargeFlag&BIT2) break; //break for low power
                     //EPD_ClearScreen();
                     //EPD_Sleep();
                     clearGame();
                     break;
            case 5 : EPD_FullScreen(IMAGE_SPLASH); //low power detected
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
                        TB0CCR0 = 3000;                           // Set CCR to delay time
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
                     setState(getReturnState());
                     //disable high level ADC comparator
                     ADC12IER2 &= ~(ADC12INIE|ADC12HIIE);
                     ADC12IFGR2 &= ~(ADC12INIFG|ADC12HIIFG|ADC12LOIFG);

                     P5IFG = 0; //reset button presses
                     P5IE  |= BIT1 | BIT3 | BIT6 | BIT7; //enable button interrupts
                     ADC12IER2 |= ADC12LOIE; //enable low side ADC interrupts
                     break;
            }

    }
}

void GPIO_Init(void){

    /* Configure GPIO to default state*/
    P1OUT  = 0; P1DIR  = 0xFF;
    P2OUT  = 0; P2DIR  = 0xFF;
    P3OUT  = 0; P3DIR  = 0xFF;
    P4OUT  = 0; P4DIR  = 0xFF;
    P5OUT  = 0; P5DIR  = 0xFF;
    P6OUT  = 0; P6DIR  = 0xFF;
    P7OUT  = 0; P7DIR  = 0xFF;
    P8OUT  = 0; P8DIR  = 0xFF;
    PJOUT  = 0; PJDIR  = 0xFFFF;


    // Set P3.0 to analog input for ADC
    P3SEL1 |= BIT0;
    P3SEL0 |= BIT0;

    // Set P5.0 UCB1SIMO (01)
    P5SEL1 &= ~BIT0;
    P5SEL0 |= BIT0;

    // Set P5.2 UCB1CLK (01)
    P5SEL1 &= ~BIT2;
    P5SEL0 |= BIT2;

    // Set P2.5 GPIO #CS Output (00)
    P2SEL1 &= ~BIT5;
    P2SEL0 &= ~BIT5;
    P2DIR  |= BIT5;
    P2OUT  &= ~BIT5;

    // Set P4.3 GPIO #D/C Output (00)
    P4SEL1 &= ~BIT3;
    P4SEL0 &= ~BIT3;
    P4DIR  |= BIT3;
    P4OUT  &= ~BIT3;

    // Set P4.2 GPIO RST Output (00)
    P4SEL1 &= ~BIT2;
    P4SEL0 &= ~BIT2;
    P4DIR  |= BIT2;
    P4OUT  &= ~BIT2;

    // Set P4.1 GPIO BUSY Input (00)
    P4SEL1 &= ~BIT1;
    P4SEL0 &= ~BIT1;
    P4DIR  &= ~BIT1;

    // Set Up LFXT pins for 32kHz crystal
    PJSEL0 |= BIT4 | BIT5;

    // Set up Buttons and Button interrupts
    P5DIR &= ~BIT1 & ~BIT3 & ~BIT6 & ~BIT7;
    P5OUT |= BIT1 | BIT3 | BIT6 | BIT7;               // button pull up
    P5REN |= BIT1 | BIT3 | BIT6 | BIT7;               // button pull up/down resistor enable
    P5IES |= BIT1 | BIT3 | BIT6 | BIT7;               // button Hi->lo edge
    P5IE  |= BIT1 | BIT3 | BIT6 | BIT7;              // button interrupt


    /* Disable the GPIO power-on default high-impedance mode. */
    PM5CTL0 &= ~LOCKLPM5;
    P5IFG = 0;
}

void SPI_Init(void){

    /** Setup SPI on UCB1 **/
    UCB1CTLW0 = UCSWRST;     // put B1 into SW reset
    UCB1CTLW0 |= UCCKPH | UCMSB | UCMST | UCSYNC | UCSSEL__SMCLK; //phase 01, MSB first, Master Mode, SMCLK clock
    UCB1CTLW0 &= ~UCSWRST;    // Take B1 out of SW Reset
}

void CLK_Init(void){
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
}

void ADC_Init(void){
    /* Initialize timer for ADC trigger. */
    TA0CCR0 = (ACLK_FREQUENCY/ADC_MONITOR_FREQUENCY); //period of adc clock
    TA0CCR1 = TA0CCR0/2; //half period
    TA0CCTL1 = OUTMOD_3; //clock setup
    TA0CTL = TASSEL__ACLK | MC__UP; //use ACLK as source


    /* Configure internal 2.0V reference. */
    while(REFCTL0 & REFGENBUSY);
    REFCTL0 |= REFVSEL_1 | REFON;
    while(!(REFCTL0 & REFGENRDY));

    /*
     * Initialize ADC12_B window comparator using the battery monitor.
     * The monitor will first enable the high side to the monitor voltage plus
     * 0.1v to make sure the voltage is sufficiently above the threshold. When
     * the high side is triggered the interrupt service routine will switch to
     * the low side and wait for the voltage to drop below the threshold. When
     * the voltage drops below the threshold voltage the device will invoke the
     * compute through power loss shutdown function to save the application
     * state and enter complete device shutdown.
     */

    //ADC set up for 16 ADC12CLK cycles in one sampling period. ADC12 turned on
    ADC12CTL0 = ADC12SHT0_2 | ADC12ON;
    //sets clock source and trigger source to timer TA0
    ADC12CTL1 = ADC12SHS_1 | ADC12SSEL_0 | ADC12CONSEQ_2 | ADC12SHP;
    //sets batmap to avcc
    //ADC12CTL3 = ADC12BATMAP;

    //enable comparator window, using A12 (P3.0), Vr+=2.0V
    ADC12MCTL0 = ADC12INCH_12 | ADC12VRSEL_1 | ADC12WINC;
    //claculate threshold values based off of 2V ref
    ADC12HI = (uint16_t)(4096*(ADC_MONITOR_THRESHOLD_HI*R2/(R1+R2))/(2.0));
    ADC12LO = (uint16_t)(4096*(ADC_MONITOR_THRESHOLD_LO*R2/(R1+R2))/(2.0));

    ADC12IFGR2 &= ~(ADC12HIIFG | ADC12LOIFG); //clear interrupts
    ADC12IER2 |= ADC12LOIE; //enable both low and high interrupts
    ADC12CTL0 |= ADC12ENC; //enable ADC
}

void Comp_Init(void)
{
    /* Setup COMP_E for P3.1 (C13) input with 1.5v reference. */
    CECTL0 = CEIPEN | CEIPSEL_13;//used for settin comparator to outputs of micro,
    CECTL1 = CEPWRMD_1 | CEMRVS;
    CECTL2 = CEREFL_2 | CERS_2 | CERSEL | CEREF0_23;
    CECTL3 = CEPD13;
    CEINT = CEIIE;
    CECTL1 |= CEON;

    /* Delay for the reference to settle */
    __delay_cycles(75);
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT5_VECTOR
__interrupt void  EXT_Button_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT5_VECTOR))) EXT_Button_ISR (void)
#else
#error Compiler not supported!
#endif
{
    if(P5IFG & B1){
        addButtonPress(1);
        P5IFG &= ~B1;
    }else if(P5IFG & B2){
        addButtonPress(2);
        P5IFG &= ~B2;
    }else if(P5IFG & B3){
        addButtonPress(3);
        P5IFG &= ~B3;
    }else if(P5IFG & B4){
        addButtonPress(4);
        P5IFG &= ~B4;
    }else {
        P5IFG=0;
    }
    //__low_power_mode_off_on_exit();
    __bic_SR_register_on_exit(LPM4_bits);
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(ADC12IV, ADC12IV_ADC12LOIFG)) {
        case ADC12IV_NONE:        break;        // Vector  0: No interrupt
        case ADC12IV_ADC12OVIFG:  break;        // Vector  2: ADC12MEMx Overflow
        case ADC12IV_ADC12TOVIFG: break;        // Vector  4: Conversion time overflow
        case ADC12IV_ADC12HIIFG:                // Vector  6: Window comparator high side

            fullChargeFlag |= BIT0;

            /* Disable the high side and enable the mid side interrupt. */
            ADC12IER2 &= ~ADC12HIIE;
            ADC12IER2 |= ADC12INIE;
            ADC12IFGR2 &= ~ADC12HIIFG;
            break;
        case ADC12IV_ADC12LOIFG:                // Vector  8: Window comparator low side

            /* Enter device shutdown with 64ms timeout. */
            if(getState()!=5) { setReturnState(); }
            setState(5);
            fullChargeFlag |= BIT2;
            P5IE  &= ~(BIT1 | BIT3 | BIT6 | BIT7); // disable button interrupts
            //ctpl_enterShutdown(CTPL_SHUTDOWN_TIMEOUT_64_MS);


            /* Disable the low side interrupt. */
            ADC12IER2 &= ~ADC12LOIE;
            ADC12IFGR2 &= ~ADC12LOIFG;
            break;
        //this case is for testing delete later
        case ADC12IV_ADC12INIFG:                // Vector  8: Window comparator low side
            fullChargeFlag &= ~BIT0;

            /* Disable the mid side and enable the high side interrupt. */
            ADC12IER2 |= ADC12HIIE;
            ADC12IER2 &= ~ADC12INIE;
            ADC12IFGR2 &= ~ADC12INIFG;
            break;


        default: break;
    }
    //__low_power_mode_off_on_exit();
    __bic_SR_register_on_exit(LPM4_bits);
}

#pragma vector=COMP_E_VECTOR
__interrupt void COMP_E_ISR(void)
{
    switch (__even_in_range(CEIV, CEIV_CERDYIFG)) {
        case CEIV_NONE:        break;
        case CEIV_CEIFG:    break;
        case CEIV_CEIIFG:
            /* Enter device shutdown with 64ms timeout. */
            ctpl_enterShutdown(CTPL_SHUTDOWN_TIMEOUT_64_MS);
            break;
        case CEIV_CERDYIFG:    break;
    }
}

