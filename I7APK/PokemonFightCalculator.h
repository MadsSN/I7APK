#pragma once
#include <utility>


#include "PokemonVariant.h"

#include "boost/signals2.hpp"

template<typename T>
class PercentageWinner
{
public:
	PercentageWinner(const T& pokemon, int winnerRate) : _pokemon(pokemon), _winnerRate(winnerRate)
	{
		std::cout << std::visit(PokeGetBase(), pokemon)._navn << std::endl;
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
		T p1 = *first++;
		T p2 = p1;
		int results = 1;
		int p1Win = 1;
		std::cout << "p1 type start: " << typeid(p1).name() << std::endl;
		std::cout << "p2 type start: " << typeid(p2).name() << std::endl;
		for (; first != last; ++first) {
			std::cout << "p1.index(): " << p1.index() << '\n';
			std::cout << "p2.index(): " << p2.index() << '\n';
			results++;
			auto result = *first;
			std::cout << "result.index() before first if: " << result.index() << '\n';
			if (p1.index() == result.index()) {
				std::cout << "result.index() first if: " << result.index() << '\n';
				p1Win++;
			}
			else if (p1.index() == p2.index())
			{
				std::cout << "result.index() else if: " << result.index() << '\n';
				p2 = result;
			}
		}
		std::cout << "results after for loop: " << results << '\n';

		int winProcent = (100 * p1Win) / results;
		if (winProcent > 50)
		{
			std::cout << "winProcent > 50 if" << '\n'; //std::cout << "result_type winProcent > 50 if: " << decltype(result_type(p1, winProcent))::type << '\n';
			std::cout << "p1 type: " << typeid(p1).name() << std::endl;
			std::cout << "result_type type: " << typeid(result_type(p1, winProcent)).name() << std::endl;
			return result_type(p1, winProcent);
		}
		else
		{
			std::cout << "winProcent > 50 else" << '\n'; 	//std::cout << "result_type winProcent > 50 else: " << decltype(result_type(p2, 100 - winProcent))::type << '\n';
			std::cout << "p2 type: " << typeid(p2).name() << std::endl;
			return result_type(p2, 100 - winProcent);
		}

	}
};

class PokemonFightCalculator
{
public:
	typedef boost::signals2::signal<std::variant<WeakPokemon, StrongPokemon, NoPokemon>(std::variant<WeakPokemon, StrongPokemon, NoPokemon>&, std::variant<WeakPokemon, StrongPokemon, NoPokemon>&), PercentageWin<std::variant<WeakPokemon, StrongPokemon, NoPokemon>>> PokeSignal;
	PokeSignal sig;

	void connect(const PokeSignal::slot_function_type& slot)
	{
		sig.connect(slot);
	}

	void fight(std::variant<WeakPokemon, StrongPokemon, NoPokemon> p1, std::variant<WeakPokemon, StrongPokemon, NoPokemon> p2) const
	{
		PokeSignal::result_type pokemon = sig(p1, p2);
		std::cout << "PokeSignal::result_type: " << typeid(pokemon).name() << std::endl;
		std::cout << "typeid(std::visit(PokeGetBase(), p1)).name() " <<  typeid(std::visit(PokeGetBase(), p1)).name() << std::endl;
		std::cout << "has _navn " <<  std::visit(PokeGetBase(), p1)._navn << std::endl;
		std::cout << "typeid(std::visit(PokeGetBase(), p2)).name() " <<  typeid(std::visit(PokeGetBase(), p2)).name() << std::endl;
		std::cout << "has _navn " <<  std::visit(PokeGetBase(), p2)._navn << std::endl;
		std::cout << "typeid(std::visit(PokeGetBase(), pokemon._pokemon)).name() " <<  typeid(std::visit(PokeGetBase(), pokemon._pokemon)).name() << std::endl;
		std::cout << "has _navn " <<  std::visit(PokeGetBase(), pokemon._pokemon)._navn << std::endl;

		std::cout << "Winner between: " << std::visit(PokeGetBase(), p1)._navn << " and "
			<< std::visit(PokeGetBase(), p2)._navn << " is: "
			<< std::visit(PokeGetBase(), pokemon._pokemon)._navn << " with ratio of " << pokemon._winnerRate << std::endl;

			/*workaround?*/


		std::cout << "fight finished" << std::endl;
	}
};
