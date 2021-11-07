#include <EPD_SPI.h>

/*
 * EPD_SPI.c
 *
 *  Created on: Sep 27, 2021
 *      Author: Franklin Bates
 */

void SPI_Init(void){

    /** Setup SPI on UCA3 **/
    UCA3CTLW0 = UCSWRST;     // put A3 into SW reset

    UCA3CTLW0 |= UCCKPH | UCMSB | UCMST | UCSYNC | UCSSEL__SMCLK;

    UCA3CTLW0 &= ~UCSWRST;    // Take A3 out of SW Reset


    /** Configure Ports **/

    // Set P6.0 UCA3SIMO (01)
    P6SEL1 &= ~BIT0;
    P6SEL0 |= BIT0;

    // Set P6.2 UCA3CLK (01)
    P6SEL1 &= ~BIT2;
    P6SEL0 |= BIT2;

    // Set p6.3 GPIO #CS Output (00)
    P6SEL1 &= ~BIT3;
    P6SEL0 &= ~BIT3;
    P6DIR  |= BIT3;
    P6OUT  |= BIT3;

    // Set P1.2 GPIO #D/C Output (00)
    P1SEL1 &= ~BIT2;
    P1SEL0 &= ~BIT2;
    P1DIR  |= BIT2;

    // Set P1.3 GPIO RST Output (00)
    P1SEL1 &= ~BIT3;
    P1SEL0 &= ~BIT3;
    P1DIR  |= BIT3;

    // Set P5.2 GPIO BUSY INPUT (00)
    P5SEL1 &= ~BIT2;
    P5SEL0 &= ~BIT2;
    P5DIR  &= ~BIT2;

    // Set Up LFXT pins for 32kHz crystal
    PJSEL0 |= BIT4 | BIT5;

    // Set up TEST BUTTON GPIO
    P8SEL1 &= ~BIT1;
    P8SEL0 &= ~BIT1;
    P8DIR  &= ~BIT1;
    P8OUT &= ~BIT1;               // button pull down
    P8REN |= BIT1;               // button pull up/down resistor enable
    P8IES &= ~BIT1;               // button Hi/lo edge
    P8IE  |= BIT1;               // button interrupt enabled

    // Set up TEST BUTTON GPIO

    P5DIR &= ~BIT5;
    P5OUT |= BIT5;               // button pull down
    P5REN |= BIT5;               // button pull up/down resistor enable
    P5IES |= BIT5;               // button Hi/lo edge
    P5IE  |= BIT5;               // button interrupt enabled

    // Enable I/O Configuration
    PM5CTL0 &= ~LOCKLPM5;       // Enable I/O

    P5IFG &= ~BIT5;
    P8IFG &= ~BIT1;
}

void Delay_ms(uint16_t millis){

    //Initialize Timer_B0 to use as delay counter
    TB0CCTL0 = CCIE;                            // TBCCR0 interrupt enabled
    TB0CCR0 = millis;                           // Set CCR to delay time
    TB0CTL = TBSSEL__ACLK | ID__8 | MC__UP;     // ACLK, Divided 8, up mode
    TB0EX0 = TBIDEX_3;                          // ACLK = 32/8/4 = 1kHz = 1ms/count
    __bis_SR_register(LPM3_bits | GIE);         // Turn off CPU, MCLK, SMCLK & enable interrupts

}

static void EPD_Reset(void){

    P1OUT |= BIT3;          // Set RST Pin high
    Delay_ms(200);
    P1OUT &= ~BIT3;         // Set RST Pin Low to reset EPD
    Delay_ms(2);
    P1OUT |= BIT3;          // Set RST Pin High
    Delay_ms(200);

}

static void EPD_SendCommand(uint8_t Reg){

    P1OUT &= ~BIT2;                             // Clear #D/C pin to signal command input to EPD
    P6OUT &= ~BIT3;                             // Clear #CS pin to enable EPD Receive
    while (!(UCA3IFG & UCTXIFG));               // Check for Tx Buffer to be ready
    UCA3TXBUF = Reg;                            // Load SPI Tx Buffer with command register
    while (!(UCA3IFG & UCTXIFG));               // Check for Tx to finish
    P6OUT |= BIT3;                              // Set #CS pin to signal Tx finished

}

static void EPD_SendData(uint8_t data){

    P1OUT |= BIT2;                              // Set #D/C pin to signal data input to EPD
    P6OUT &= ~BIT3;                             // Clear #CS pin to enable EPD Receive
    while (!(UCA3IFG & UCTXIFG));               // Check for Tx Buffer to be ready
    UCA3TXBUF = data;                           // Load SPI Tx Buffer with data byte
    while (!(UCA3IFG & UCTXIFG));               // Check for Tx to finish
    P6OUT |= BIT3;                              // Set #CS pin to signal Tx finished

}

static void EPD_BusyWait(void){

    //Wait while EPD busy signal is high
    while( P5IN & BIT2 ){
        Delay_ms(10);
    }

}

static void EPD_DisplayOnFull(void){

    EPD_SendCommand(0x22);          //Set Up Display
    EPD_SendData(0xF7);
    EPD_SendCommand(0x20);          //Master Activation
    EPD_BusyWait();

}

static void EPD_SetWindow(uint8_t Xstart, uint8_t Xend, uint8_t Ystart, uint8_t Yend){

    EPD_SendCommand(0x44);          //Set Ram X start & end
    EPD_SendData((Xstart>>3) & 0xFF);
    EPD_SendData((Xend>>3) & 0xFF);

    EPD_SendCommand(0x45);          //Set Ram Y start & end
    EPD_SendData(Ystart & 0xFF);
    EPD_SendData((Ystart>>8) & 0xFF);
    EPD_SendData(Yend & 0xFF);
    EPD_SendData((Yend>>8) & 0xFF);

}

static void EPD_SetCursor(uint8_t Xstart, uint8_t Ystart){

    EPD_SendCommand(0x4E);          //Set X counter
    EPD_SendData(Xstart & 0xFF);

    EPD_SendCommand(0x4F);          //Set Y counter
    EPD_SendData(Ystart & 0xFF);
    EPD_SendData((Ystart>>8) & 0xFF);

}

void EPD_Init(){

    EPD_Reset();

    EPD_BusyWait();
    EPD_SendCommand(0x12);      //Software Reset
    EPD_BusyWait();

    EPD_SendCommand(0x01);      //Driver Output Control Command
    EPD_SendData(0xC7);
    EPD_SendData(0x00);
    EPD_SendData(0x01);

    EPD_SendCommand(0x11);      //Data Entry Mode
    EPD_SendData(0x01);         //X,Y increment

    EPD_SetWindow( 0, EPD_WIDTH - 1, EPD_HEIGHT - 1, 0);

    EPD_SendCommand(0x3C);
    EPD_SendData(0x01);

    EPD_SendCommand(0x18);
    EPD_SendData(0x80);

    EPD_SendCommand(0x22);
    EPD_SendData(0xB1);
    EPD_SendCommand(0x20);

    EPD_SetCursor(0, EPD_HEIGHT-1);
    EPD_BusyWait();

}

void EPD_ClearScreen(void){

    short i, j;
    uint8_t Height, Width;
    Height = EPD_HEIGHT;
    Width = (EPD_WIDTH / 8);

    // Load B/W Ram
    EPD_SendCommand(0x24);
    for (i=0; i<Height; i++){
        for (j=0; j<Width; j++){
            EPD_SendData(0xFF);
        }
    }

    // Load RED Ram for B/W pixels
    EPD_SendCommand(0x26);
    for (i=0; i<Height; i++){
        for (j=0; j<Width; j++){
            EPD_SendData(0xFF);
        }
    }

    EPD_DisplayOnFull();
}

void EPD_DisplayImage(const uint8_t *Image){

    short i, j;
    uint8_t Height, Width;
    Height = EPD_HEIGHT;
    Width = (EPD_WIDTH / 8);

    uint16_t Addr = 0;
    EPD_SendCommand(0x24);
    for (i=0; i<Height; i++){
        for (j=0; j<Width; j++){
            Addr = Width * i + j;
            EPD_SendData(Image[Addr]);
        }
    }
    EPD_DisplayOnFull();

}

void EPD_DisplayBlack(void){

    short i, j;
    uint8_t Height, Width;
    Height = EPD_HEIGHT;
    Width = (EPD_WIDTH / 8);

    EPD_SendCommand(0x24);
    for (i=0; i<Height; i++){
        for (j=0; j<Width; j++){
            EPD_SendData(0x00);
        }
    }
    EPD_DisplayOnFull();

}

void EPD_Sleep(void){

    EPD_SendCommand(0x10);
    EPD_SendData(0x01);
    Delay_ms(100);
    P1OUT &= ~BIT3;
    Delay_ms(2);

}

// Timer B1 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer0_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_B0_VECTOR))) Timer0_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
    __low_power_mode_off_on_exit();
}





