
#include "RoomChallenge.h"

//Placeholder for proper data retrievals
int getData()
{
    int count = 0;
    for(int i=1; i<5; i++){
        count += getButtonPress(i);
    }
    return count;
}

void RoomChallenge()
{
    uint8_t minButtonVal =0;
    clearButtonPress();
    if(getRoomChallenge()>0){
        EPD_FullScreen(IMAGE_ROOM1);
        minButtonVal = 5;
        clearRoomChallenge();
    } else{
        EPD_FullScreen(IMAGE_ROOM1);
        minButtonVal = 10;
        addRoomChallenge();
    }
    while(getData() < minButtonVal){
        __low_power_mode_3();
    }
    addSucceed();
}


