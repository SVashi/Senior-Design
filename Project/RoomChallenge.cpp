
#include "RoomChallenge.h"

void RoomChallenge()
{
    clearButtonPress();
    //challenge determination
    if(getRoomChallenge()>0){           //challenge 2
        if(RoomChallenge2(5,3000)){ //5 button presses, 3 seconds of full crank turning
            clearRoomChallenge();
            addSucceed();
        }
    } else{                             //challenge 1 (reset case)
        if(RoomChallenge1(10,2000)){
            addRoomChallenge();
            addSucceed();
        }
    }

}

bool RoomChallenge1(uint8_t minButtonVal, uint16_t minTimeVal){
    EPD_FullScreen(IMAGE_R1);
    while((getButtonPress(1) < minButtonVal) && (getButtonPress(4) < minButtonVal)){    //enter LPM until challenge completed
       //break for low power
       if(fullChargeFlag&BIT2) {
           return false;
       }
       __low_power_mode_3();
       //break for low power
       if(fullChargeFlag&BIT2) {
           return false;
       }
    }

    //Crank Challenge
    EPD_FullScreen(IMAGE_R3);
    CrankChallenge(minTimeVal); //blocking statment

    return true;
}

bool RoomChallenge2(uint8_t minButtonVal, uint16_t minTimeVal){
    EPD_FullScreen(IMAGE_R2);
    while((getButtonPress(2) < minButtonVal) && (getButtonPress(3) < minButtonVal)){    //enter LPM until challenge completed
       //break for low power
       if(fullChargeFlag&BIT2) {
           return false;
       }
       __low_power_mode_3();
       //break for low power
       if(fullChargeFlag&BIT2) {
           return false;
       }
    }

    //Crank Challenge
    EPD_FullScreen(IMAGE_R3);
    CrankChallenge(minTimeVal); //blocking statment

    return true;
}

