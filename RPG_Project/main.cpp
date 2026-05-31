#include "Character.h"
#include <iostream>

int main()
{
    Character hero("勇者", 100, 20);
    hero.DisplayInfo();
    hero.TakeDamage(30);
    hero.Heal(10);
    return 0;
}