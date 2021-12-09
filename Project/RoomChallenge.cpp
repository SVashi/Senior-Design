
#include "RoomChallenge.h"

void RoomChallenge()
{
    clearButtonPress();
    //challenge determination
    if(getRoomChallenge()<2){           //challenge 1
        if(RoomChallenge1(10,2000)){
            addSucceed();
        }
    } else{                             //challenge 2 (reset case)
        if(RoomChallenge2(5,3000)){ //5 button presses, 3 seconds of full crank turning
            clearRoomChallenge();
            addSucceed();
        }
    }

}

bool RoomChallenge1(uint8_t minButtonVal, uint16_t minTimeVal){
    if(getRoomChallenge()==0){
        EPD_FullScreen(IMAGE_R1);
        while(1){    //enter LPM until challenge completed
           //break for low power
           if(fullChargeFlag&BIT2) {
               return false;
           }
           __low_power_mode_3();
           //break for low power
           if(fullChargeFlag&BIT2) {
               return false;
           }

           if(getButtonPress(2) > minButtonVal){
               if(getButtonPress(3) > minButtonVal){
                   break;
               }
           }
        }
        addRoomChallenge();
    }


    //Crank Challenge
    EPD_FullScreen(IMAGE_R3);
    CrankChallenge(minTimeVal); //blocking statement
    addRoomChallenge();

    return true;
}

bool RoomChallenge2(uint8_t minButtonVal, uint16_t minTimeVal){
    if(getRoomChallenge()==2){
        EPD_FullScreen(IMAGE_R2);
        while(1){    //enter LPM until challenge completed
           //break for low power
           if(fullChargeFlag&BIT2) {
               return false;
           }
           __low_power_mode_3();
           //break for low power
           if(fullChargeFlag&BIT2) {
               return false;
           }
           if(getButtonPress(1) > minButtonVal){
               if(getButtonPress(4) > minButtonVal){
                   break;
               }
           }
        }
        addRoomChallenge();
    }

    //Crank Challenge
    EPD_FullScreen(IMAGE_R3);
    CrankChallenge(minTimeVal); //blocking statement
    addRoomChallenge();

    return true;
}

