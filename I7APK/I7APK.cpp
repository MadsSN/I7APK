// I7APK.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <list>
#include <iterator>
#include <algorithm>
#include "boost/signals2.hpp"

template <typename T>
class TypeTraits;

struct WeakType {

};

struct StrongType {

};

template<>
struct TypeTraits<WeakType> {
	static size_t hp() { return 50; };
	static size_t attack() { return 10; };
};

template<>
struct TypeTraits<StrongType> {
	static size_t hp() { return 100; };
	static size_t attack() { return 5; };
};


class Pokemon {
public:
	Pokemon(size_t index, std::string navn, size_t hp, size_t attack) : _pokeIndex(index),
		_navn(navn),
		_hp(hp),
		_attack(attack) {

	}

	size_t _hp;
	size_t _attack;
	size_t _pokeIndex;
	std::string _navn;
};


std::ostream& operator<<(std::ostream& out, const Pokemon& c)
{
	out << "Index: " << c._pokeIndex << std::endl;
	out << "Navn: " << c._navn << std::endl;
	out << "HP: " << c._hp << std::endl;
	out << "Attack: " << c._attack << std::endl;
	return out;
}

template<typename TType>
class TypePokemon : public Pokemon {
public:
	TypePokemon(size_t index, std::string navn) : Pokemon(index, navn, TypeTraits<TType>::hp(), TypeTraits<TType>::attack())
	{};
};

using WeakPokemon = TypePokemon<WeakType>;
using StrongPokemon = TypePokemon<StrongType>;

class PrintPokemonNameFunctor {
public: 
	void operator() (std::string name) const {
		std::cout << name << std::endl;
	}
};

struct HelloWorld
{
	void operator()() const
	{
		std::cout << "Hello, World!" << std::endl;
	}
};

class PokemonFightCalculator
{
public:
	Pokemon operator()(Pokemon p1, Pokemon p2) const
	{
		//Look up in a compile time generated list?
		return p1;
	}
};

class Pokedex {
public:
	std::list<Pokemon> _pokemons;
	Pokedex() : _pokemons({
			WeakPokemon {1, "Bulbasaur"},
			WeakPokemon {2, "Ivysaur"},
			WeakPokemon {3, "Venusaur"},
			StrongPokemon {4, "Charmander"},
			StrongPokemon {5, "Charmeleon"},
			StrongPokemon {6, "Charizard"}
		}) {}

	//Delegating constructor
	Pokedex(std::list<Pokemon> pokemons) : _pokemons(pokemons) {	}

	void printAllRangeBased() {
		for (auto pokemon : _pokemons) {
			std::cout << pokemon << std::endl;
		}
	}

	void printAllIterator() {
		for (std::list<Pokemon>::const_iterator pokeIter = _pokemons.begin(); pokeIter != _pokemons.end(); ++pokeIter)
		{
			std::cout << *pokeIter << std::endl;
		}
	}

	void printSortedByName() {
		std::list<std::string> nameOfPokemon{};

		std::transform(_pokemons.begin(),
			_pokemons.end(),
			std::back_inserter(nameOfPokemon),
			[](Pokemon& pokemon) {return pokemon._navn;}
		);

		nameOfPokemon.sort();

		std::for_each(nameOfPokemon.begin(), nameOfPokemon.end(), PrintPokemonNameFunctor());
	}

	void randomPokemonFights()
	{
		//Attach many different fight calculators, and combine for a total result?
		boost::signals2::signal<Pokemon (Pokemon, Pokemon)> sig;
		sig.connect(PokemonFightCalculator());

		boost::optional<Pokemon> winner = sig(_pokemons.front(), _pokemons.back());
		std::cout << winner.get() << std::endl;
	}
};




int main()
{
	bool continueProgram = true;
	Pokedex pokedex{};
	while (continueProgram) {
		std::cout << "********* MENU ***************" << std::endl;
		std::cout << "Press 1 for viewing all pokemons for range" << std::endl;
		std::cout << "Press 2 for viewing all pokemons with iterator" << std::endl;
		std::cout << "Press 3 for viewing all pokemon names, sorted by name" << std::endl;
		std::cout << "Press 4 for starting random fights every few seconds." << std::endl;
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
				pokedex.randomPokemonFights();
				break;
			case 'q':
				continueProgram = false;
				break;
		}
	}
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
