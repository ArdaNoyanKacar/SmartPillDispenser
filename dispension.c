#include "dispension.h"
#include <stdbool.h>
#include <stdio.h>




uint8_t num_dispensions = 0;
dispension_t dispension_schedule[MAX_DISPENSIONS];

bool dispension_add(dispension_t new_dispension)
{
    if (num_dispensions >= MAX_DISPENSIONS)
    {
        printf("WARNING: Dispensions maxed out, cannot add new dispension!\n");
        return false;
    }

    for (int i = 0; i < num_dispensions; i++)
    {
        if (((new_dispension.day_of_week == dispension_schedule[i].day_of_week)) && (new_dispension.minute == dispension_schedule[i].minute) && new_dispension.pill_type == dispension_schedule[i].pill_type)
        {
            printf("WARNING: Dispension at that time already exists!\n");
            return false;
        }
    }
    dispension_schedule[num_dispensions] = new_dispension;
    num_dispensions++;

    return true;
}


bool dispension_remove(uint8_t index)
{
    if (index >= num_dispensions)
    {
        printf("WARNING: Index not valid, cannot remove dispension!\n");
        return false;
    }

    // Shift remaining events down the array
    for (uint8_t i = index; i < num_dispensions; i++)
    {
        dispension_schedule[i] = dispension_schedule[i+1];
    } 
    num_dispensions--;

    return true;
}