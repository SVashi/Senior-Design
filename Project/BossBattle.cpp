
#include "BossBattle.h"


int getAttack()
{
    int count = (getButtonPress(2)+getButtonPress(3))/2;
    return count;
}

int playerTurn(int currentHP)
{
    clearButtonPress();
    int minButtonPress = 5;
    EPD_FullScreen(IMAGE_BOSS1);
    if(fullChargeFlag&BIT2) return currentHP;

    while(getAttack() < minButtonPress){
        if(fullChargeFlag&BIT2) return currentHP; //break for low power
        __low_power_mode_3();
        if(fullChargeFlag&BIT2) return currentHP; //break for low power
    }
    EPD_FullScreen(IMAGE_BOSS2);
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
    EPD_FullScreen(IMAGE_BOSSINT);
    if(fullChargeFlag&BIT2) return; //break for low power

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
    EPD_FullScreen(IMAGE_BOSSSUC);
    if(fullChargeFlag&BIT2) return; //break for low power
}

