#include <stdio.h>
using namespace std;

//Returns average to compare to main value
int calculateAverage(int data[])
{
    int average = 0;
    for (int i = 0; i < sizeof(data); i++)
    {
        average += data[i];
    }
    return average/(sizeof(data));
}

//Placeholder for proper data retrievals
int* getData()
{
    int data[4] = {23.5, 27.9, 30.1, 29.4};
    return data;
}

void RoomChallenge()
{
    int CrankMinimumValue = 25;
    int *data;

    data = getData();
    if (calculateAverage(data) > CrankMinimumValue)
    {
        printf("Success with a value of: %d\n", calculateAverage(data));
    }
    else
    {
        printf("Failure with a value of: %d\n", calculateAverage(data));
    }
}
