#include <stdlib.h>
#include <stdio.h>
#include "EPD_SPI.h"
using namespace std;

//Placeholder for pins
int getPinValues()
{
    int randVal = rand() % 3 + 1;
    if (randVal == 1)
    {
        return 11;
    }
    return 00;
}

int playerTurn(int currentHP)
{
    int pinVal = getPinValues();
    if (pinVal == 11)
    {
        currentHP = currentHP - 10;
    }
    return currentHP;
}

int bossTurn(int currentHP)
{
    int damage = rand() % 3 + 1;
    return currentHP - damage * 10;
}

void BossBattle(int successes, int failures)
{
    int playerHP = 100 + successes * 10;
    int bossHP = 100 + failures * 10;
    while (playerHP > 0 && bossHP > 0)
    {
        printf("Boss hp is %d, Player hp is %d\n", bossHP, playerHP);
        bossHP = playerTurn(bossHP);
        if (bossHP > 0)
        {
            playerHP = bossTurn(playerHP);
        }
    }
    if (playerHP <= 0)
    {
        printf("Player Loses!");
    }
    else
    {
        printf("Player Wins!");
    }

    EPD_Init();
    EPD_ClearScreen();
    EPD_Sleep();
}
