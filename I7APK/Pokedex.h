#pragma once
#include <iostream>
#include <list>
#include <iterator>
#include <thread>
#include <mutex>
#include "PokemonVariant.h"
#include "PokemonFightCalculator.h"
#include "CompilePokemon.h"
#include "FightRecursivePokemon.h"
#include "FightPokemon.h"
#include "TypeList.h"
#include "FightPokemonAsync.h"

class PrintPokemonNameFunctor {
public:
	void operator() (std::string name) const {
		std::cout << name << std::endl;
	}
};

//By recursive
void ComparePokemonToAllOthers(PokemonVariant& challenger, std::list<PokemonVariant>& pokemons)
{
	for (std::list<PokemonVariant>::const_iterator pokeIter = pokemons.begin(); pokeIter != pokemons.end(); ++pokeIter)
	{
		std::visit([](auto&& arg, auto&& arg2)
			{
				FightWithConstexpr(arg, arg2);
			}, challenger, *pokeIter);
	}
}

class Pokedex {
public:
	std::list<PokemonVariant> _pokemons;
	Pokedex()
	{
		_pokemons.emplace_back(WeakPokemon(1, "Bulbasaur"));
		_pokemons.emplace_back(WeakPokemon(2, "Ivysaur"));
		_pokemons.emplace_back(WeakPokemon(3, "Venusaur"));
		_pokemons.emplace_back(StrongPokemon(4, "Charmander"));
		_pokemons.emplace_back(StrongPokemon(5, "Charmeleon"));
		_pokemons.emplace_back(StrongPokemon(6, "Charizard"));
	}

	void printAllRangeBased() {
		for (const auto& pokemon : _pokemons) {
			pokemon.print();
		}
	}

	void printAllIterator() {
		for (std::list<PokemonVariant>::const_iterator pokeIter = _pokemons.begin(); pokeIter != _pokemons.end(); ++pokeIter)
		{
			std::visit(PokeVisitor(), *pokeIter);
		}
	}

	void printSortedByName() {

		std::list<std::string> nameOfPokemon{};

		std::transform(_pokemons.begin(),
			_pokemons.end(),
			std::back_inserter(nameOfPokemon),
			[](PokemonVariant pokemon) {return pokemon.name(); }
		);

		nameOfPokemon.sort();

		std::for_each(nameOfPokemon.begin(), nameOfPokemon.end(), PrintPokemonNameFunctor());

	}

	void randomPokemonFights(PokemonFightCalculator& pokemonFightCalculator)
	{
		pokemonFightCalculator.fight(_pokemons.front(), _pokemons.back());
	}

	void predestinedBattle() {
		CompilePokemon<WeakType, 20> Raticate("Raticate");
		CompilePokemon<StrongType, 149> Dragonite("Dragonite");

		FightRecursivePokemon(Raticate, Dragonite);
	}

	void comparePokemontypes() {
		PokemonVariant challenger = StrongPokemon(149, "Dragonite");
		ComparePokemonToAllOthers(challenger, _pokemons);
	}

	void startPokemonTypeTutorial() {
		pokemonTypeTutorial();
	}



	void asyncTurnBattle()
	{
		std::mutex m;
		std::condition_variable cond;
		int lastAttack = 0;
		std::thread t1(asyncThreadBattle, std::ref(m),
			std::ref(cond), std::ref(_pokemons.front()), std::ref(lastAttack));
		std::thread t2(asyncThreadBattle, std::ref(m),
			std::ref(cond), std::ref(_pokemons.back()), std::ref(lastAttack));

		t1.join();
		t2.join();
	}
};