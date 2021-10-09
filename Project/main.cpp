#include <msp430.h>
#include <stdio.h>
#include <stdlib.h>
#include "RoomChallenge.h"
#include "BossBattle.h"
using namespace std;

int main() {
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    RoomChallenge();
    BossBattle(7,0);
    return 0;
}
