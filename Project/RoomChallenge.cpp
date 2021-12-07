
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
    EPD_FullScreen(IMAGE_ROOM1);
    if(getRoomChallenge()>0){           //challenge 1
        EPD_FullScreen(IMAGE_C2);
        minButtonVal = 5;
        clearRoomChallenge();
    } else{                             //challenge 2
        EPD_FullScreen(IMAGE_C1);
        minButtonVal = 10;
        addRoomChallenge();
    }
    while(getData() < minButtonVal){    //enter LPM until challenge completed
        __low_power_mode_3();
    }
    EPD_FullScreen(IMAGE_RMSUC);
    addSucceed();
}


