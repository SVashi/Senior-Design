
#include "RoomChallenge.h"

int getData()
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
    if(getRoomChallenge()>0){           //challenge 1
        EPD_FullScreen(IMAGE_R2);
        minButtonVal = 5;
        clearRoomChallenge();
    } else{                             //challenge 2
        EPD_FullScreen(IMAGE_R1);
        minButtonVal = 10;
        addRoomChallenge();
    }
    while(getData() < minButtonVal){    //enter LPM until challenge completed
        //break for low power
        if(fullChargeFlag&BIT2) {
            if(getRoomChallenge()>0) {
                subRoomChallenge();
            }
            else {
                addRoomChallenge();
            }
            return;
        }

        __low_power_mode_3();

        //break for low power
        if(fullChargeFlag&BIT2) {
            if(getRoomChallenge()>0) {
                subRoomChallenge();
            }
            else {
                addRoomChallenge();
            }
            return;
        }
    }
        addSucceed();
}


