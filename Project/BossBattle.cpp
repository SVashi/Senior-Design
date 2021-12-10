
#include "BossBattle.h"


int playerTurn(int currentHP)
{
    if(getRoomChallenge() != 1){
        clearButtonPress();
        int minButtonPress = 5;
        EPD_FullScreen(IMAGE_BOSS1);
        if(fullChargeFlag&BIT2) return currentHP;

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

           if(getButtonPress(2) > minButtonPress){
               if(getButtonPress(3) > minButtonPress){
                   break;
               }
           }
        }
        EPD_FullScreen(IMAGE_BOSS2);
    }

    GameState.roomChallenge = 1; //on wakeup don't go to low power
    CrankChallenge(4000);
    currentHP = currentHP - 100;
    return currentHP;
}

int bossTurn(int currentHP)
{
    int damage = 1;
    return currentHP - damage * 10;
}

void BossBattle()
{
    if(getRoomChallenge() != 1){
        EPD_FullScreen(IMAGE_BOSSINT);
        if(fullChargeFlag&BIT2) return; //break for low power
    }

    int playerHP = 100 + getSucceed() * 10;
    int bossHP = 100 + getFail() * 10;
    while (playerHP > 0 && bossHP > 0)
    {
        bossHP = playerTurn(bossHP);
        if(fullChargeFlag&BIT2) return; //break for low power
        if (bossHP > 0)
        {
            playerHP = bossTurn(playerHP);
        }
    }

}

