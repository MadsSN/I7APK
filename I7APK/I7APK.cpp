// I7APK.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <list>
#include <iterator>
#include <variant>
#include <thread>
#include <algorithm>
#include <mutex>
#include "boost/signals2.hpp"
#include "IfThenElse.h"
#include "CompilePokemon.h"
#include "TypeTraits.h"
#include "Pokemon.h"
#include "TypePokemon.h"
#include "PokemonVariant.h"
#include "PokemonFightCalculator.h"
#include "FightCompilePokemon.h"

class PrintPokemonNameFunctor {
public: 
	void operator() (std::string name) const {
		std::cout << name << std::endl;
	}
};


struct FirstWinTag {};
struct SecondWinTag {};

template<typename P1, typename P2>
struct is_first_winner
{
	inline static constexpr bool value = P1::type_traits::hp_c / P2::type_traits::attack_c > P2::type_traits::hp_c / P1::type_traits::attack_c;
};

template<typename P1, typename P2>
PokemonVariant FightWithTags(P1& p1, P2& p2)
{
	return FightWithTagsImpl(p1, p2,
		typename IfThenElse<FirstWinTag, SecondWinTag, is_first_winner<P1, P2>::value>::Type());
}

template<typename P1, typename P2>
P1& FightWithTagsImpl(P1& p1, P2& p2, FirstWinTag)
{
	std::cout << "BATTLE MUSIC - First winner chicken dinner" << std::endl;
	std::cout << p1._navn << " won!" << std::endl;
	std::cout << "and " << p2._navn << " died for your entertainment" << std::endl;
	return p1;
}

template<typename P1, typename P2>
P2& FightWithTagsImpl(P1& p1, P2& p2, SecondWinTag)
{
	std::cout << "BATTLE MUSIC - Second winner chicken dinner" << std::endl;
	std::cout << p2._navn << " won!" << std::endl;
	std::cout << "and " << p1._navn << " died for your entertainment" << std::endl;
	return p2;
}

template<typename P1, typename P2>
const Pokemon* FightWithConstexpr2(const P1* p1, const P2* p2) {
	std::cout << "BATTLE MUSIC" << std::endl;
	if constexpr (is_first_winner<P1, P2>::value) {
		std::cout << p1->_navn << " won!" << std::endl;
		std::cout << "and " << p2->_navn << " died for your entertainment" << std::endl;
		return p1;
	}
	else {
		std::cout << p1->_navn << " died for your entertainment" << std::endl;
		std::cout << "and " << p2->_navn << " won!" << std::endl;
		return p2;
	}
};


struct PokeBattleVisitor
{
	template<typename P1, typename P2>
	PokemonVariant operator()(const P1& arg, const P2& challenger)
	{
		return FightWithTags(challenger, arg);
	}
};

PokemonVariant Fight3(PokemonVariant p1, PokemonVariant p2)
{
	return std::visit(PokeBattleVisitor(), p1, p2);
}



//By recursive
void ComparePokemonToAllOthers(PokemonVariant& challenger, std::list<PokemonVariant>& pokemons)
{
	for (std::list<PokemonVariant>::const_iterator pokeIter = pokemons.begin(); pokeIter != pokemons.end(); ++pokeIter)
	{
		std::visit([](auto&& arg, auto&& arg2)
		{
			FightWithTags(arg, arg2);
		}, challenger,*pokeIter);
	}
}

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
		pb._hp = (pb._hp - lastAttack) < 0 ? 0 : (pb._hp - lastAttack);
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

/// TypeList area
struct NullNodeType {};

template<typename L, typename R>
struct TypeList
{
	typedef L First;
	typedef R Rest;
};

template<typename TypeList, unsigned index>
struct AtIndex : AtIndex<typename TypeList::Rest, index - 1> {
};

template<typename TypeList>
struct AtIndex<TypeList, 0> {
	using type = typename TypeList::First;
};

std::string removeStructFromTypeName(std::string fullTypeName) {
	return fullTypeName.substr(7, fullTypeName.length() - 1);
}

void printIndexAndName(size_t index, std::string name) {
	std::cout << "#" << index << " " + name << std::endl;
}

template<typename TypeList>
struct printImpl {
	inline static const std::string Print = printImpl<typename TypeList::Rest>::Print + removeStructFromTypeName(typeid(typename TypeList::First).name())
		+ std::string(", ");
};

template<>
struct printImpl<NullNodeType> {
	inline static const std::string Print = std::string();
};

template<typename TypeList>
inline static const std::string Print = printImpl<TypeList>::Print;


class Pokedex {
public:
	std::list<PokemonVariant> _pokemons;
	Pokedex()
	{
		_pokemons.emplace_back(WeakPokemon(1, "Bulbasaur"));
		_pokemons.emplace_back(WeakPokemon(2, "Ivysaur" ));
		_pokemons.emplace_back(WeakPokemon(3, "Venusaur" ));
		_pokemons.emplace_back(StrongPokemon(4, "Charmander" ));
		_pokemons.emplace_back(StrongPokemon(5, "Charmeleon" ));
		_pokemons.emplace_back(StrongPokemon(6, "Charizard" ));
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

		FightCompilePokemon(Raticate, Dragonite);
	}

	void comparePokemontypes() {
		PokemonVariant challenger = StrongPokemon(149, "Dragonite");
		ComparePokemonToAllOthers(challenger, _pokemons);
	}

	void pokemonTypeTutorial() {
		std::cout << "\nOh hello my grandson! what was your name again?" << std::endl;
		std::cout << "Nevermind. Today I will teach you about the types of the first 6 pokedex entries of this region." << std::endl;

		std::cout << "Our pokedex currently contains:" << std::endl;

		CompilePokemon<WeakType, 1> bulbasaur("Bulbasaur");
		printIndexAndName(bulbasaur.pokedexIndex, bulbasaur.name);
		
		CompilePokemon<WeakType, 2> ivysaur("Ivysaur");
		printIndexAndName(ivysaur.pokedexIndex, ivysaur.name);

		CompilePokemon<WeakType, 3> venusaur("Venusaur");
		printIndexAndName(venusaur.pokedexIndex, venusaur.name);

		CompilePokemon<StrongType, 4> charmander("Charmander");
		printIndexAndName(charmander.pokedexIndex, charmander.name);

		CompilePokemon<WeakType, 5>  charmeleon("Charmeleon");
		printIndexAndName(charmeleon.pokedexIndex, charmeleon.name);

		CompilePokemon<StrongType, 6> charizard("Charizard");
		printIndexAndName(charizard.pokedexIndex, charizard.name);

#define TYPELIST6(T1, T2, T3, T4, T5, T6) TypeList<T6, TypeList<T5, TypeList<T4, TypeList<T3, TypeList<T2, TypeList<T1, NullNodeType>>>>>>
		typedef TYPELIST6(
			decltype(bulbasaur)::type,
			decltype(ivysaur)::type,
			decltype(venusaur)::type,
			decltype(charmander)::type,
			decltype(charmeleon)::type,
			decltype(charizard)::type) PokemonTypeList;

		std::cout << "Lets examine the type of " << charmeleon.name << " at pokedex entry #" << charmeleon.pokedexIndex<< std::endl;

		std::string charmeleonType = typeid(AtIndex<PokemonTypeList, decltype(charmeleon)::pokedexIndex>::type).name();
		std::cout << charmeleon.name + " has type " << removeStructFromTypeName(charmeleonType);

		std::cout << "\n\nThe types of all the six are:" << std::endl;
		std::cout << Print<PokemonTypeList> << std::endl;
	}


	
	void asyncTurnBattle()
	{
		std::mutex m;
		std::condition_variable cond;
		int lastAttack = 0;
		std::thread t1(asyncThreadBattle, std::ref(m), 
			std::ref(cond), std::ref(_pokemons.front()),std::ref(lastAttack));
		std::thread t2(asyncThreadBattle, std::ref(m), 
			std::ref(cond), std::ref(_pokemons.back()), std::ref(lastAttack));
		
		t1.join();
		t2.join();
	}
};





int main()
{
	using namespace std::placeholders;
	bool continueProgram = true;
	PokemonFightCalculator pokemonFightCalculator{};
	
	
	//std::function<PokemonVariant (PokemonVariant, PokemonVariant)> fn = &Fight3;
	//Try to find a way to adopt pokemon fight calculator with fight 3 system..
	//Requeries that the interface changes to something like TypePokemon<T1> from Pokemon
	auto fn = [](auto&& arg1, auto&& arg2) { return Fight3(arg1, arg2); };
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
				pokedex.pokemonTypeTutorial();
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
