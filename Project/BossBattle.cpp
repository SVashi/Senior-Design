
#include "BossBattle.h"

int getAttack()
{
    int count = 0;
    for(int i=1; i<5; i++){
        count += getButtonPress(i);
    }
    return count;
}

int playerTurn(int currentHP)
{
    clearButtonPress();
    int minButtonPress = 5;

    while(getAttack() < minButtonPress){
        if(fullChargeFlag&BIT2) break; //break for low power
        __low_power_mode_3();
    }
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
    EPD_FullScreen(IMAGE_BOSS1);
    int playerHP = 100 + getSucceed() * 10;
    int bossHP = 100 + getFail() * 10;
    while (playerHP > 0 && bossHP > 0)
    {
        if(fullChargeFlag&BIT2) break; //break for low power
        bossHP = playerTurn(bossHP);
        if (bossHP > 0)
        {
            playerHP = bossTurn(playerHP);
        }
    }






}
