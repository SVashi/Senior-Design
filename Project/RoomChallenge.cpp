
#include "RoomChallenge.h"

int getDataHorz()
{
    int count = 0;
    for(int i=4; i>0; i--){
        count += getButtonPress(i);
    }
    return count;
}

int getDataVert()
{
    int count = 0;
    for(int i=4; i>0; i--){
        count += getButtonPress(i);
    }
    return count;
}

void RoomChallenge()
{
    uint8_t minButtonVal =0;
    clearButtonPress();
    //challenge determination
    if(getRoomChallenge()>0){           //challenge 2
        RoomChallenge2(5,3000); //5 button presses, 3 seconds of full crank turning
    } else{                             //challenge 1 (reset case)
        RoomChallenge1(10,2000);
    }
        addSucceed();
}

void RoomChallenge1(uint8_t minButtonVal, uint16_t minTimeVal){
    //disable button interrupts that are not used
    P5IE  &= ~(BIT3 | BIT6); // (vert buttons)
    EPD_FullScreen(IMAGE_R1);
    while((getButtonPress(1) < minButtonVal) && (getButtonPress(7) < minButtonVal)){    //enter LPM until challenge completed
       //break for low power
       if(fullChargeFlag&BIT2) {
           return;
       }
       __low_power_mode_3();
       //break for low power
       if(fullChargeFlag&BIT2) {
           return;
       }
    }

    //Crank Challenge
    EPD_FullScreen(IMAGE_R1);
    CrankChallenge(minTimeVal);

}

void RoomChallenge2(uint8_t minButtonVal, uint16_t minTimeVal){
    EPD_FullScreen(IMAGE_R2);
    minButtonVal = 10;
    addRoomChallenge();
}

