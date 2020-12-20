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


template <typename T>
class TypeTraits;

struct WeakType {};
struct StrongType {};
struct NoType {};

template<>
struct TypeTraits<WeakType> {
	static const int hp_c = 50;
	static const int attack_c = 5;
	typedef WeakType type;
};

template<>
struct TypeTraits<StrongType> {
	static const int hp_c = 100;
	static const int attack_c = 13;
	typedef StrongType type;
};

template<>
struct TypeTraits<NoType> {
	static const int hp_c = 100;
	static const int attack_c = 13;
	typedef NoType type;
};


class Pokemon {
public:
	Pokemon(size_t index, std::string navn, size_t hp, size_t attack) : _pokeIndex(index),
		_navn(navn),
		_hp(hp),
		_attack(attack) {
		std::cout << "Ordinary ctor " << _navn << "\n";
	}
	
	size_t _pokeIndex;
	std::string _navn;
	size_t _hp;
	size_t _attack;

	bool operator==(const Pokemon poke)
	{
		return _pokeIndex == poke._pokeIndex;
	}

	Pokemon(const Pokemon& poke) : Pokemon(poke._pokeIndex, poke._navn, poke._hp, poke._attack)
	{
		std::cout << "Copy ctor " << _navn  << "\n";
	}

	Pokemon(Pokemon&& poke) noexcept :
		_hp(0),
		_attack(0)
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


template<typename TType, size_t PokedexIndex = 0>
class CompilePokemon {
	BOOST_STATIC_ASSERT(
		boost::is_same<TType, WeakType>::value ||
		boost::is_same<TType, StrongType>::value ||
		boost::is_same<TType, NoType>::value);
public:
	CompilePokemon(std::string pokemonName) : name(pokemonName) {}

	std::string name;

	static const size_t pokedexIndex = PokedexIndex;

	typedef TypeTraits<TType> type_traits;
	typedef typename TypeTraits<TType>::type type;
};



template<typename TType>
class TypePokemon : public Pokemon {
	BOOST_STATIC_ASSERT(
		boost::is_same<TType, WeakType>::value ||
		boost::is_same<TType, StrongType>::value ||
		boost::is_same<TType, NoType>::value);
public:
	TypePokemon() : TypePokemon(0, "")
	{};
	
	TypePokemon(size_t index, std::string navn) : Pokemon(index, navn, TypeTraits<TType>::hp_c, TypeTraits<TType>::attack_c)
	{};

	typedef TypeTraits<TType> type_traits;
};


template<size_t index>
using WeakCompilePokemon = CompilePokemon<WeakType, index>;
template<size_t index>
using StrongCompilePokemon = CompilePokemon<StrongType, index>;
template<size_t index>
using NoCompilePokemon = CompilePokemon<NoType, index>;

template<size_t index>
using CompilePokemons = std::variant<WeakCompilePokemon<index>, StrongCompilePokemon<index>, NoCompilePokemon<index>>;


using WeakPokemon = TypePokemon<WeakType>;
using StrongPokemon = TypePokemon<StrongType>;
using NoPokemon = TypePokemon<NoType>;

std::ostream& operator<<(std::ostream& out, const Pokemon& c)
{
	out << "Index: " << c._pokeIndex << std::endl;
	out << "Navn: " << c._navn << std::endl;
	out << "HP: " << c._hp << std::endl;
	out << "Attack: " << c._attack << std::endl;
	return out;
}

struct PokeVisitor
{
	template<typename T>
	void operator()(const T& arg)
	{
		std::cout << arg;
	}
};

struct PokeGetBase
{
	template<typename T>
	const Pokemon& operator()(const T& arg)
	{
		return arg;
	}
};

struct PokemonVariant : std::variant<WeakPokemon, StrongPokemon, NoPokemon>
{
	PokemonVariant() = default;

	template<typename T>
	PokemonVariant(const T& x) : std::variant<WeakPokemon, StrongPokemon, NoPokemon>(x) {}

	std::string name(){
		return get()._navn;
	}

	size_t hp() {
		return get()._hp;
	}

	size_t attack() {
		return get()._attack;
	}

	size_t pokeIndex() {
		return get()._pokeIndex;
	}
	
	const Pokemon& get() const
	{
		return std::visit(PokeGetBase(), *this);
	}

	void print() const 
	{
		std::cout << get() << std::endl;
	}
};


std::ostream& operator<<(std::ostream& out, const PokemonVariant& c)
{
	out << c;
	return out;
}

class PrintPokemonNameFunctor {
public: 
	void operator() (std::string name) const {
		std::cout << name << std::endl;
	}
};

template<typename T>
class PercentageWinner
{
public:
	PercentageWinner()
	{
		_winnerRate = 0;
	}
	
	PercentageWinner(T pokemon, int winnerRate) : _pokemon(pokemon), _winnerRate(winnerRate)
	{
	}
	T _pokemon;
	int _winnerRate;
};

template<typename T>
struct PercentageWin
{
	typedef PercentageWinner<T> result_type;

	template <typename InputIterator>
	result_type operator()(InputIterator first,
		InputIterator last) const
	{
		//if (first == last) return result_type();
		T p1 = *first++;
		T p2 = p1;
		int results = 1;
		int p1Win = 1;
		for (; first != last; ++first) {
			results++;
			if (p1.index() == (*first).index()) {
				p1Win++;
			}else if(p1.index() == p2.index())
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
	typedef boost::signals2::signal<PokemonVariant(PokemonVariant, PokemonVariant), PercentageWin<PokemonVariant>> PokeSignal;
	PokeSignal sig;

	void connect(const PokeSignal::slot_function_type& slot )
	{
		sig.connect(slot);
	}
	
	void fight(PokemonVariant p1, PokemonVariant p2) const
	{
		PokeSignal::result_type pokemon = sig(p1,p2);
		std::cout << "Winner between: " << p1.name() << " and "
			<< p2.name() << " is: "
		<< pokemon._pokemon.name() << " with ratio of " << pokemon._winnerRate << std::endl;
	}
};



template<int health, int dmg>
struct Attack {
	inline static const int hp = (health - dmg) < 0 ? 0 : (health - dmg);
};

template<const int p1_dmg, const int p1_hp, const int p2_dmg, const int p2_hp>
void Fight(const std::string& p1_name, const std::string& p2_name) {
	std::cout << p1_name << " has " << p1_hp << " hp" << std::endl;

	if constexpr (p1_hp == 0) {
		std::cout << p1_name << " died for your entertainment" << std::endl;
	}
	else {
		// give dmg
		std::cout << p1_name << " survived and attacks " << p2_name << " for " << p1_dmg << " hp" << std::endl;
		// next round
		Fight<p2_dmg, Attack<p2_hp, p1_dmg>::hp, p1_dmg, p1_hp>(p2_name, p1_name);
	}
};

template<typename P1, typename P2>
void Fight(const P1& p1, const P2& p2) {
	std::cout << "BATTLE MUSIC" << std::endl;

	Fight<P1::type_traits::attack_c, P1::type_traits::hp_c, P2::type_traits::attack_c, P2::type_traits::hp_c>(p1.name, p2.name);
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

		Fight(Raticate, Dragonite);
		Fight(Dragonite, Raticate);
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
