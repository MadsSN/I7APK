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
		for (; first != last; ++first) {
			results++;
			auto result = *first;
			if (p1.index() == result.index()) {
				p1Win++;
			}
			else if (p1.index() == p2.index())
			{
				p2 = result;
			}
		}

		int winProcent = (100 * p1Win) / results;
		if (winProcent > 50)
		{
			return result_type(p1, winProcent);
		}
		else
		{
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
		std::cout << "Winner between: " << std::visit(PokeGetBase(), p1)._navn << " and "
			<< std::visit(PokeGetBase(), p2)._navn << " is: "
			<< std::visit(PokeGetBase(), pokemon._pokemon)._navn << " with ratio of " << pokemon._winnerRate << std::endl;

			/*workaround?*/


		std::cout << "fight finished" << std::endl;
	}
};
