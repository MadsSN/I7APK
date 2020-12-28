#pragma once
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
		T& p1 = *first++;
		T& p2 = p1;
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
	typedef boost::signals2::signal<PokemonVariant(PokemonVariant&, PokemonVariant&), PercentageWin<PokemonVariant>> PokeSignal;
	PokeSignal sig;

	void connect(const PokeSignal::slot_function_type& slot)
	{
		sig.connect(slot);
	}

	void fight(PokemonVariant p1, PokemonVariant p2) const
	{
		std::cout << "Pre signal step" << std::endl;
		PokeSignal::result_type pokemon = sig(p1, p2);
		std::cout << "Post signal step" << std::endl;
		std::cout << "Winner between: " << p1.name() << " and "
			<< p2.name() << " is: "
			<< pokemon._pokemon.name() << " with ratio of " << pokemon._winnerRate << std::endl;
		std::cout << "Post printout step" << std::endl;
	}
};