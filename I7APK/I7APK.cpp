#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <mutex>
#include <thread>
#include <variant>
#include "boost/signals2.hpp"
#include "pokemonVariant.h"

template <typename T>
class TypeTraits;

struct WeakType {

};

struct StrongType {

};

struct NoType {

};


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

class PrintPokemonNameFunctor {
public:
	void operator() (std::string name) const {
		std::cout << name << std::endl;
	}
};

class Pokemon {
public:
	Pokemon(size_t index, std::string navn, size_t hp, size_t attack) :
		_pokeIndex(index),
		_navn(navn),
		_hp(hp),
		_attack(attack) {
		std::cout << "Ordinary ctor " << _navn << "\n";
	}

	size_t _hp;
	size_t _attack;
	size_t _pokeIndex;
	std::string _navn;

	bool operator==(const Pokemon poke)
	{
		return _pokeIndex == poke._pokeIndex;
	}

	Pokemon(const Pokemon& poke) : Pokemon(poke._pokeIndex, poke._navn, poke._hp, poke._attack)
	{
		std::cout << "Copy ctor " << _navn << "\n";
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
		if (this != &poke)
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

struct PokemonVariant;

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

	//static const size_t pokedexIndex_c = PokedexIndex;

	typedef TypeTraits<TType> type_traits;
	typedef typename TypeTraits<TType>::type type;
};


using WeakPokemon = TypePokemon<WeakType>;
using StrongPokemon = TypePokemon<StrongType>;
using NoPokemon = TypePokemon<NoType>;


struct PokeGetBase
{
	template<typename T>
	Pokemon operator()(const T& arg)
	{
		return arg;
	}
};

struct PokeVisitor
{
	template<typename T>
	void operator()(const T& arg)
	{
		std::cout << arg;
	}
};

struct PokeBattleVisitor
{
	template<typename P1, typename P2>
	PokemonVariant operator()(const P1& arg, const P2& challenger);
};

PokemonVariant Fight3(PokemonVariant p1, PokemonVariant p2);

struct PokemonVariant : public std::variant<WeakPokemon, StrongPokemon, NoPokemon>
{
	PokemonVariant() = default;

	template<typename T>
	PokemonVariant(const T& x) : std::variant<WeakPokemon, StrongPokemon, NoPokemon>(x) {}

	Pokemon get()
	{
		return std::visit(PokeGetBase(), *this);
	}

	Pokemon print()
	{
		std::visit(PokeVisitor(), *this);
	}

	Pokemon fight(PokemonVariant challenger)
	{
		std::visit(PokeBattleVisitor(), *this, challenger);
	}
};


template <typename P1, typename P2>
PokemonVariant PokeBattleVisitor::operator()(const P1& arg, const P2& challenger)
{
	return Fight3(challenger, arg);
}

class PokemonWinner
{
public:
	PokemonWinner()
	{
		_winnerRate = 0;
	}
	
	PokemonWinner(PokemonVariant pokemon, int winnerRate) : _pokemon(pokemon), _winnerRate(winnerRate)
	{
	}
	PokemonVariant _pokemon;
	int _winnerRate;
};

struct percentage
{
	typedef PokemonWinner result_type;

	template <typename InputIterator>
	PokemonWinner operator()(InputIterator first,
		InputIterator last) const
	{
		//if (first == last) return result_type();
		PokemonVariant p1 = *first++;
		PokemonVariant p2 = p1;
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
	typedef boost::signals2::signal<PokemonVariant(PokemonVariant, PokemonVariant), percentage> PokeSignal;
	PokeSignal sig;

	void connect(const PokeSignal::slot_function_type& slot )
	{
		sig.connect(slot);
	}
	
	void fight(PokemonVariant p1, PokemonVariant p2) const
	{
		PokeSignal::result_type pokemon = sig(p1,p2);
		std::cout << "Winner between: " << p1.get()._navn << " and "
			<< p1.get()._navn << " is: "
		<< p1.get()._navn << " with ratio of " << pokemon._winnerRate << std::endl;
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

	Fight<P1::type_traits::attack_c, P1::type_traits::hp_c, P2::type_traits::attack_c, P2::type_traits::hp_c>(p1._navn, p2._navn);
};

template<typename P1, typename P2>
struct is_first_winner
{
	inline static constexpr bool value = P1::type_traits::hp_c / P2::type_traits::attack_c > P2::type_traits::hp_c / P1::type_traits::attack_c;
};



/*Taken from slides*/
template < typename Ttrue, typename Tfalse, bool>
struct IfThenElse;

template <typename Ttrue, typename Tfalse>
struct IfThenElse < Ttrue, Tfalse, false>
{
	 typedef Tfalse Type;
};

template <typename Ttrue, typename Tfalse>
struct IfThenElse < Ttrue, Tfalse, true>
{
	typedef Ttrue Type;
};
/*End of taken from slides*/

struct FirstWinTag {};
struct SecondWinTag {};

template<typename P1, typename P2>
PokemonVariant Fight3(P1& p1, P2& p2)
{
	return Fight3Impl(p1, p2,
		typename IfThenElse<FirstWinTag, SecondWinTag, is_first_winner<P1, P2>::value>::Type());
}



PokemonVariant Fight3(PokemonVariant p1, PokemonVariant p2)
{
	return std::visit(PokeBattleVisitor(), p1, p2);
}



template<typename P1, typename P2>
P1& Fight3Impl(P1& p1, P2& p2, FirstWinTag)
{
	std::cout << "BATTLE MUSIC - First winner chicken dinner" << std::endl;
	std::cout << p1._navn << " won!" << std::endl;
	std::cout << "and " << p2._navn << " died for your entertainment" << std::endl;
	return p1;
}

template<typename P1, typename P2>
P2& Fight3Impl(P1& p1, P2& p2, SecondWinTag)
{
	std::cout << "BATTLE MUSIC - Second winner chicken dinner" << std::endl;
	std::cout << p2._navn << " won!" << std::endl;
	std::cout << "and " << p1._navn << " died for your entertainment" << std::endl;
	return p2;
}

template<typename P1, typename P2>
const Pokemon* Fight2(const P1* p1, const P2* p2) {
	std::cout << "BATTLE MUSIC" << std::endl;
	if constexpr (is_first_winner<P1,P2>::value) {
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

//By folding expression
//Does not get used as other is more specific. 
template<typename C, typename... Ps>
void ComparePokemonToAllOthers(C c, Ps... ps)
{
	(Fight3(c, ps), ...);
}

//By recursive
//Leveraging visitor pattern..Do not need to know what self is. 
void ComparePokemonToAllOthers(PokemonVariant& challenger, std::list<PokemonVariant>& pokemons)
{
	for (std::list<PokemonVariant>::const_iterator pokeIter = pokemons.begin(); pokeIter != pokemons.end(); ++pokeIter)
	{
		std::visit([](auto&& arg, auto&& arg2)
		{
			Fight3(arg, arg2);
		}, challenger,*pokeIter);
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
		//Bussiness logic  - Avoid rollover
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

	//Delegating constructor
	Pokedex(std::list<PokemonVariant> pokemons) : _pokemons(pokemons) {}

	void printAllRangeBased() {
		for (const auto& pokemon : _pokemons) {
			std::visit(PokeVisitor(), pokemon);
		}
	}

	void printAllIterator() {
		for (std::list<PokemonVariant>::const_iterator pokeIter = _pokemons.begin(); pokeIter != _pokemons.end(); ++pokeIter)
		{
			std::visit([](auto& arg) { std::cout << arg; }, * pokeIter);
		}
	}

	void printSortedByName() {
		
		std::list<std::string> nameOfPokemon{};

		std::transform(_pokemons.begin(),
			_pokemons.end(),
			std::back_inserter(nameOfPokemon),
			[](PokemonVariant pokemon) {return pokemon.get()._navn; }
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

		Fight3(Raticate, Dragonite);
		Fight3(Dragonite, Raticate);
	}

	void comparePokemontypes() {
		WeakPokemon Raticate(20, "Raticate");
		StrongPokemon Dragonite(149, "Dragonite");
		NoPokemon S�ren(899, "Soeren");
		PokemonVariant challenger = Raticate;
		ComparePokemonToAllOthers(challenger, _pokemons);
	}

	/*
	void pokemonTypeTutorial() {
		std::cout << "\nOh hello my grandson! what was your name again?" << std::endl;
		std::cout << "Nevermind. Today I will teach you about the types of the first 6 pokedex entries of this region." << std::endl;

		std::cout << "Our pokedex currently contains:" << std::endl;

		TypePokemon<WeakType, 1> bulbasaur(1, "Bulbasaur");
		printIndexAndName(bulbasaur.pokedexIndex_c, bulbasaur._navn);

		TypePokemon<WeakType, 2> ivysaur(2, "Ivysaur");
		printIndexAndName(ivysaur.pokedexIndex_c, ivysaur._navn);

		TypePokemon<WeakType, 3> venusaur(3, "Venusaur");
		printIndexAndName(venusaur.pokedexIndex_c, venusaur._navn);

		TypePokemon<StrongType, 4> charmander(4, "Charmander");
		printIndexAndName(charmander.pokedexIndex_c, charmander._navn);

		TypePokemon<WeakType, 5>  charmeleon(5, "Charmeleon");
		printIndexAndName(charmeleon.pokedexIndex_c, charmeleon._navn);

		TypePokemon<StrongType, 6> charizard(6, "Charizard");
		printIndexAndName(charizard.pokedexIndex_c, charizard._navn);

#define TYPELIST6(T1, T2, T3, T4, T5, T6) TypeList<T6, TypeList<T5, TypeList<T4, TypeList<T3, TypeList<T2, TypeList<T1, NullNodeType>>>>>>
		typedef TYPELIST6(
			decltype(bulbasaur)::type,
			decltype(ivysaur)::type,
			decltype(venusaur)::type,
			decltype(charmander)::type,
			decltype(charmeleon)::type,
			decltype(charizard)::type) PokemonTypeList;

		std::cout << "Lets examine the type of " << charmeleon._navn << " at pokedex entry #" << charmeleon._pokeIndex << std::endl;

		std::string charmeleonType = typeid(AtIndex<PokemonTypeList, decltype(charmeleon)::pokedexIndex_c>::type).name();
		std::cout << charmeleon._navn + " has type " << removeStructFromTypeName(charmeleonType);

		std::cout << "\n\nThe types of all the six are:" << std::endl;
		std::cout << Print<PokemonTypeList> << std::endl;
	}
	*/

	
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
				//pokedex.pokemonTypeTutorial();
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
