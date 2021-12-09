#include <EPD_SPI.h>

/*
 * EPD_SPI.c
 *
 *  Created on: Sep 27, 2021
 *      Author: Franklin Bates
 */

const uint8_t WF_PARTIAL[159] =
{
0x0,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x80,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x40,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0xF,0x0,0x0,0x0,0x0,0x0,0x1,
0x1,0x1,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x22,0x22,0x22,0x22,0x22,0x22,0x0,0x0,0x0,
0x02,0x17,0x41,0xB0,0x32,0x28,
};

void Delay_ms(uint16_t millis){

    //Initialize Timer_B0 to use as delay counter
    TB0CCTL0 = CCIE_0;                            // TBCCR0 interrupt disabled
    TB0CCR0 = 0;                                // Set CCR to 0
    TB0CTL = TBSSEL__ACLK | ID__8 | MC__UP;     // ACLK, Divided 8, up mode
    TB0EX0 = TBIDEX_3;                          // ACLK = 32/8/4 = 1kHz = 1ms/count
    TB0CCR0 = millis;                           // Set CCR to delay time
    TB0CCTL0 = CCIE;                            // TBCCR0 interrupt enabled
    __low_power_mode_3();                       // Turn off CPU, MCLK, SMCLK & enable interrupts

}

static void EPD_Reset(void){

    P4OUT |= BIT2;          // Set RST Pin high
    Delay_ms(200);
    P4OUT &= ~BIT2;         // Set RST Pin Low to reset EPD
    Delay_ms(2);
    P4OUT |= BIT2;          // Set RST Pin High
    Delay_ms(200);

}

static void EPD_SendCommand(uint8_t Reg){

    P4OUT &= ~BIT3;                             // Clear #D/C pin to signal command input to EPD
    P2OUT &= ~BIT5;                             // Clear #CS pin to enable EPD Receive
    while (!(UCB1IFG & UCTXIFG));               // Check for Tx Buffer to be ready
    UCB1TXBUF = Reg;                            // Load SPI Tx Buffer with command register
    while (!(UCB1IFG & UCTXIFG));               // Check for Tx to finish
    P2OUT |= BIT5;                              // Set #CS pin to signal Tx finished

}

static void EPD_SendData(uint8_t data){

    P4OUT |= BIT3;                              // Set #D/C pin to signal data input to EPD
    P2OUT &= ~BIT5;                             // Clear #CS pin to enable EPD Receive
    while (!(UCB1IFG & UCTXIFG));               // Check for Tx Buffer to be ready
    UCB1TXBUF = data;                           // Load SPI Tx Buffer with data byte
    while (!(UCB1IFG & UCTXIFG));               // Check for Tx to finish
    P2OUT |= BIT5;                              // Set #CS pin to signal Tx finished

}

static void EPD_BusyWait(void){

    //Wait while EPD busy signal is high
    while( P4IN & BIT1 ){
        Delay_ms(10);
    }

}

static void EPD_DisplayOnFull(void){

    EPD_SendCommand(0x22);          //Set Up Display
    EPD_SendData(0xF7);
    EPD_SendCommand(0x20);          //Master Activation
    EPD_BusyWait();

}

static void EPD_DisplayOnPart(void){
    EPD_SendCommand(0x22);          //Set Up Display
    EPD_SendData(0xFF);
    EPD_SendCommand(0x20);          //Master Activation
    EPD_BusyWait();

}

static void EPD_SetLUT(void){
    EPD_SendCommand(0x32);
    for(int i=0; i<153; i++){
        EPD_SendData(WF_PARTIAL[i]);
    }
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
    uint8_t Height = EPD_HEIGHT;
    uint8_t Width = (EPD_WIDTH / 8);

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

void EPD_DisplayImage(const uint8_t *image){

    short i, j;
    uint8_t Height = EPD_HEIGHT;
    uint8_t Width = (EPD_WIDTH / 8);

    uint16_t Addr = 0;
    EPD_SendCommand(0x24);
    for (i=0; i<Height; i++){
        for (j=0; j<Width; j++){
            Addr = Width * i + j;
            EPD_SendData(image[Addr]);
        }
    }
    EPD_DisplayOnFull();

}

void EPD_DisplayPartBase(const uint8_t *image){
    short i, j;
    uint8_t Height = EPD_HEIGHT;
    uint8_t Width = (EPD_WIDTH / 8);

    uint16_t Addr = 0;
    EPD_SendCommand(0x24);
    for (i=0; i<Height; i++){
        for (j=0; j<Width; j++){
            Addr = Width * i + j;
            EPD_SendData(image[Addr]);
        }
    }
    EPD_SendCommand(0x26);
            for (i=0; i<Height; i++){
                for (j=0; j<Width; j++){
                    Addr = Width * i + j;
                    EPD_SendData(image[Addr]);
                }
            }
    EPD_DisplayOnPart();
}

void EPD_DisplayPartImage(const uint8_t *Image){
    short i, j;
    uint8_t Height = EPD_HEIGHT;
    uint8_t Width = (EPD_WIDTH / 8);

    P4OUT &= ~BIT2;         // Set RST Pin Low to reset EPD
    Delay_ms(2);
    P4OUT |= BIT2;          // Set RST Pin High
    Delay_ms(5);

    EPD_SetLUT();
    EPD_SendCommand(0x37);
    EPD_SendData(0x00);
    EPD_SendData(0x00);
    EPD_SendData(0x00);
    EPD_SendData(0x00);
    EPD_SendData(0x00);
    EPD_SendData(0x40);
    EPD_SendData(0x00);
    EPD_SendData(0x00);
    EPD_SendData(0x00);
    EPD_SendData(0x00);

    EPD_SendCommand(0x3C);
    EPD_SendData(0x80);

    uint16_t Addr = 0;
    EPD_SendCommand(0x24);
    for (i=0; i<Height; i++){
        for (j=0; j<Width; j++){
            Addr = Width * i + j;
            EPD_SendData(Image[Addr]);
        }
    }

    EPD_DisplayOnPart();

}

void EPD_DisplayBlack(void){

    short i, j;
    uint8_t Height = EPD_HEIGHT;
    uint8_t Width = (EPD_WIDTH / 8);

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
    P4OUT &= ~BIT2; // clear rst pin P4.2
    Delay_ms(2);

}

void EPD_FullScreen(const uint8_t *image){
    uint8_t old = getOldState();
    uint8_t current = getState();
    //check if the current screen does not match current
<<<<<<< HEAD
    uint8_t old = getOldState();
    uint8_t current = getState();
=======
>>>>>>> branch 'main' of https://git.ece.iastate.edu/svashi/ccs-senior-design.git
    if(old != current){
    EPD_Init();
    EPD_DisplayImage(image);
    EPD_Sleep();
    }
}
// Timer B0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer0_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_B0_VECTOR))) Timer0_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
    fullChargeFlag |= BIT1;
    TB0CTL = MC__STOP;
    //__low_power_mode_off_on_exit();
    __bic_SR_register_on_exit(LPM4_bits);
}





