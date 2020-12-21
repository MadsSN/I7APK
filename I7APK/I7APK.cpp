// I7APK.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <variant>
#include "boost/signals2.hpp"
#include "Pokemon.h"
#include "PokemonFightCalculator.h"
#include "FightPokemon.h"
#include "Pokedex.h"

int main()
{
	using namespace std::placeholders;
	bool continueProgram = true;
	PokemonFightCalculator pokemonFightCalculator{};
	
	
	//std::function<PokemonVariant (PokemonVariant, PokemonVariant)> fn = &Fight3;
	//Try to find a way to adopt pokemon fight calculator with fight 3 system..
	//Requeries that the interface changes to something like TypePokemon<T1> from Pokemon
	auto fn = [](auto&& arg1, auto&& arg2) { return std::visit(PokeBattleVisitor(), arg1, arg2);; };
	pokemonFightCalculator.connect(fn);
	pokemonFightCalculator.connect(std::bind(fn, std::placeholders::_2, std::placeholders::_1));
	
	Pokedex pokedex{};
	while (continueProgram) {
		std::cout << "********* MENU ***************" << std::endl;
		std::cout << "Press 1 for viewing all pokemons for range" << std::endl;
		std::cout << "Press 2 for viewing all pokemons with iterator" << std::endl;
		std::cout << "Press 3 for viewing all pokemon names, sorted by name" << std::endl;
		std::cout << "Press 4 for starting random fights every few seconds." << std::endl;
		std::cout << "Press 5 for one predestined fight." << std::endl;
		std::cout << "Press 6 for WeakType to all other types" << std::endl;
		std::cout << "Press 7 for a small tutorial on some of the pokemon types" << std::endl;
		std::cout << "Press 8 for starting slow turnbased battle" << std::endl;
		std::cout << "Press q for exiting" << std::endl;

		std::cout << "Dit valg: ";
		char input;
		std::cin >> input;
		switch (input) {
			case '1':
				pokedex.printAllRangeBased();
				break;
			case '2':
				pokedex.printAllIterator();
				break;
			case '3':
				pokedex.printSortedByName();
				break;
			case '4':
				pokedex.randomPokemonFights(pokemonFightCalculator);
				break;
			case '5':
				pokedex.predestinedBattle();
				break;
			case '6':
				pokedex.comparePokemontypes();
				break;
			case '7':
				pokedex.startPokemonTypeTutorial();
				break;
			case '8':
				pokedex.asyncTurnBattle();
				break;
			case 'q':
				continueProgram = false;
				break;
		}
	}
}
