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

class HP
{
public:
	HP(unsigned long long hp) : _hp(hp){}
	size_t _hp;
};

HP operator"" _hp(unsigned long long n)
 {
	return HP(n);
}

std::ostream& operator<<(std::ostream& out, const HP& c)
{
	out << c._hp;
	return out;
}

class Attack
{
public:
	Attack(unsigned long long attack) : _attack(attack) {}
	size_t _attack;
};

Attack operator"" _attack(unsigned long long n)
{
	return Attack(n);
}

std::ostream& operator<<(std::ostream& out, const Attack& c)
{
	out << c._attack;
	return out;
}

template<>
struct TypeTraits<WeakType> {
	static HP hp() { return 50_hp; };
	static Attack attack() { return 10_attack; };
};

template<>
struct TypeTraits<StrongType> {
	static HP hp() { return 100_hp; };
	static Attack attack() { return 5_attack; };
};


class Pokemon {
public:
	Pokemon(size_t index, std::string navn, HP hp, Attack attack) : _pokeIndex(index),
		_navn(navn),
		_hp(hp),
		_attack(attack) {
		std::cout << "Ordinary ctor " << _navn << "\n";
	}

	HP _hp;
	Attack _attack;
	size_t _pokeIndex;
	std::string _navn;

	bool operator==(const Pokemon poke)
	{
		return _pokeIndex == poke._pokeIndex;
	}

	Pokemon(const Pokemon& poke) : Pokemon(poke._pokeIndex, poke._navn, poke._hp, poke._attack)
	{
		std::cout << "Copy ctor " << _navn  << "\n";
	}

	Pokemon(Pokemon&& poke) noexcept :
		_hp(0_hp),
		_attack(0_attack)
	{
		swap(*this, poke);
		std::cout << "Move ctor " << _navn << "\n";
	}

	Pokemon& operator=(const Pokemon& poke)
	{
		std::cout << "Assignment ctor " << _navn << "\n";
		Pokemon copy(poke);
		swap(*this, copy);
		return *this;
	}

	Pokemon& operator=(Pokemon&& poke) noexcept
	{
		if(this != &poke)
		{
			swap(*this, poke);
		}
		std::cout << "Move Assignment ctor " << _navn << "\n";
		return *this;
	}

	void swap(Pokemon& left, Pokemon& rigth) const noexcept
	{
		std::swap(left._pokeIndex, rigth._pokeIndex);
		std::swap(left._navn, rigth._navn);
		std::swap(left._attack, rigth._attack);
		std::swap(left._hp, rigth._hp);
	}
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

class PokemonWinner
{
public:
	PokemonWinner() : _pokemon(new WeakPokemon{ 0, "Something went wrong" })
	{
		_winnerRate = 0;
	}
	
	PokemonWinner(Pokemon* pokemon, int winnerRate) : _pokemon(pokemon), _winnerRate(winnerRate)
	{
	}
	Pokemon* _pokemon;
	int _winnerRate;
};

struct percentage
{
	typedef PokemonWinner result_type;

	template <typename InputIterator>
	PokemonWinner operator()(InputIterator first,
		InputIterator last) const
	{
		if (first == last) return result_type();
		Pokemon* p1 = *first++;
		Pokemon* p2 = p1;
		int results = 1;
		int p1Win = 1;
		for (; first != last; ++first) {
			results++;
			if (p1 == *first) {
				p1Win++;
			}else if(p1 == p2)
			{
				p2 = *first;
			}
		}
		int winProcent = (100 * p1Win) / results;
		if(winProcent > 50)
		{
			return result_type(p1, winProcent);
		}else
		{
			return result_type(p2, 100 - winProcent);
		}
		
	}
};

class PokemonFightCalculator
{
public:
	typedef boost::signals2::signal<Pokemon*(Pokemon*, Pokemon*), percentage> PokeSignal;
	PokeSignal sig;

	void connect(const PokeSignal::slot_function_type& slot )
	{
		sig.connect(slot);
	}
	
	void fight(Pokemon* p1, Pokemon* p2) const
	{
		PokeSignal::result_type pokemon = sig(p1,p2);
		std::cout << "Winner between: " << p1->_navn << " and "  << p2->_navn << " is: "
		<< pokemon._pokemon->_navn << " with ratio of " << pokemon._winnerRate << std::endl;
	}
};

class FightFirstWins
{
public:
	Pokemon* operator()(Pokemon* p1, Pokemon* p2) const
	{
		return p1;
	}
};



class Pokedex {
public:
	std::list<Pokemon*> _pokemons;
	Pokedex()
	{
		_pokemons.emplace_back(new WeakPokemon(1, "Bulbasaur"));
		_pokemons.emplace_back(new WeakPokemon(2, "Ivysaur" ));
		_pokemons.emplace_back(new WeakPokemon(3, "Venusaur" ));
		_pokemons.emplace_back(new StrongPokemon(4, "Charmander" ));
		_pokemons.emplace_back(new StrongPokemon(5, "Charmeleon" ));
		_pokemons.emplace_back(new StrongPokemon(6, "Charizard" ));
	}

	//Delegating constructor
	Pokedex(std::list<Pokemon*> pokemons) : _pokemons(pokemons) {}

	void printAllRangeBased() {
		for (const auto& pokemon : _pokemons) {
			std::cout << *pokemon << std::endl;
		}
	}

	void printAllIterator() {
		for (std::list<Pokemon*>::const_iterator pokeIter = _pokemons.begin(); pokeIter != _pokemons.end(); ++pokeIter)
		{
			std::cout << **pokeIter << std::endl;
		}
	}

	void printSortedByName() {
		std::list<std::string> nameOfPokemon{};

		std::transform(_pokemons.begin(),
			_pokemons.end(),
			std::back_inserter(nameOfPokemon),
			[](Pokemon* pokemon) {return pokemon->_navn;}
		);

		nameOfPokemon.sort();

		std::for_each(nameOfPokemon.begin(), nameOfPokemon.end(), PrintPokemonNameFunctor());
	}

	void randomPokemonFights(PokemonFightCalculator& pokemonFightCalculator)
	{
		pokemonFightCalculator.fight(_pokemons.front(), _pokemons.back());
	}
};





int main()
{
	using namespace std::placeholders;
	bool continueProgram = true;
	PokemonFightCalculator pokemonFightCalculator{};

	std::function<Pokemon* (Pokemon*, Pokemon*)> fn = FightFirstWins();
	pokemonFightCalculator.connect(fn);

	//Reverses the input so the second pokemon starts with bind
	pokemonFightCalculator.connect(std::bind(fn, _2, _1));
	
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
				pokedex.randomPokemonFights(pokemonFightCalculator);
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
