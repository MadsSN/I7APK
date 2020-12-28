#pragma once
#include <mutex>
#include "PokemonVariant.h"
#include <iostream>

void asyncThreadBattle(std::mutex& m, std::condition_variable& c, PokemonVariant& p, int& lastAttack)
{
	Pokemon pb = std::visit(PokeGetBase(), p);
	std::unique_lock<std::mutex> ul(m);
	//Wait for battle to start
	while (pb._hp > 0)
	{
		lastAttack = pb._attack;
		//Notify and wait to get notified back.
		c.notify_one();
		c.wait(ul, [&] {return pb._attack != lastAttack; });
		//Win and exit condition
		if (lastAttack < 0)
		{
			std::cout << pb._navn << " won" << std::endl;
			return;
		}
		//Bussiness logic
		pb._hp = (static_cast<int>(pb._hp) - lastAttack) < 0 ? 0 : (pb._hp - lastAttack);
		std::cout << pb._navn << " has " << pb._hp << " after being attacked by " << lastAttack << std::endl;

		//Lose and exit condition
		if (pb._hp == 0)
		{
			std::cout << pb._navn << " lost" << std::endl;
			lastAttack = -1;
			c.notify_one();
			return;
		}
	}
}