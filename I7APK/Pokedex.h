#pragma once
#include <iostream>
#include <list>
#include <iterator>
#include <thread>
#include <mutex>
#include <exception>
#include <memory_resource>
#include "PokemonVariant.h"
#include "PokemonFightCalculator.h"
#include "CompilePokemon.h"
#include "FightRecursivePokemon.h"
#include "FightPokemon.h"
#include "TypeList.h"
#include "FightPokemonAsync.h"
#include "PokemonStatsResource.h"

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
				FightWithConstexpr(std::move(arg), std::move(arg2));
			}, challenger, *pokeIter);
	}
}

namespace pokepmr {
	template<typename T>
	using list = std::list<T, std::pmr::polymorphic_allocator<T>>;
}

class Pokedex {
public:
	

	PokemonStatsResource _pokemonStatsResource{};
	pokepmr::list<PokemonVariant> _pokemons{ &_pokemonStatsResource };

	Pokedex() 
	{
		createNewPokemon<WeakPokemon>(1, "Bulbasaur");
		createNewPokemon<WeakPokemon>(2, "Ivysaur");
		createNewPokemon<WeakPokemon>(3, "Venusaur");
		createNewPokemon<StrongPokemon>(4, "Charmander");
		createNewPokemon<StrongPokemon>(5, "Charmeleon");
		createNewPokemon<StrongPokemon>(6, "Charizard");
		try
		{
			createNewPokemon<WeakPokemon>(1, "Bulbasaur");
		} catch(std::exception& e) {
			std::cout << "Tried creating duplicate pokemon with index 1"  << std::endl;
		}

		_pokemonStatsResource.printStats();
	}

	template<typename T, typename... Args>
	void createNewPokemon(Args&&... args)
	{
		T pokemon = T(std::forward<Args>(args)...);
		for (std::list<PokemonVariant>::const_iterator pokeIter = _pokemons.begin(); pokeIter != _pokemons.end(); ++pokeIter)
		{
			if(pokemon._pokeIndex == std::visit(PokeGetBase(), *pokeIter)._pokeIndex)
			{
				throw std::exception();
			};
		}
		_pokemons.emplace_back(pokemon);
		//_pokemonsPmr.emplace_back(pokemon);
	}

	void printAllRangeBased() {
		for (const auto& pokemon : _pokemons) {
			std::visit(PokeVisitor(), pokemon);
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
			[](const PokemonVariant& pokemon) {return std::visit(PokeGetBase(), pokemon)._navn; }
		);
		nameOfPokemon.sort();
		std::for_each(nameOfPokemon.begin(), nameOfPokemon.end(), PrintPokemonNameFunctor());

	}

	void randomPokemonFights(PokemonFightCalculator& pokemonFightCalculator)
	{
		pokemonFightCalculator.fight(_pokemons.front(), _pokemons.back());
	}

	void predestinedBattle() {
		WeakPokemon Raticate(20, "Raticate");
		StrongPokemon Dragonite(149, "Dragonite");

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