#pragma once
#include "PokemonVariant.h"

#include "boost/signals2.hpp"

template<typename T>
class PercentageWinner
{
public:
	PercentageWinner(const T& pokemon, int winnerRate) : _pokemon(pokemon), _winnerRate(winnerRate)
	{
		std::cout << "Percentage winner ctor " << std::endl;
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
		std::cout << "Percentage step 1 " << std::endl;
		T& p1 = *first++;
		std::cout << "Percentage step 1.1 " << std::endl;
		T& p2 = p1;
		std::cout << "Percentage step 1.2 " << std::endl;
		int results = 1;
		int p1Win = 1;
		for (; first != last; ++first) {
			results++;
			std::cout << "Percentage step 2 " << std::endl;
			auto result = *first;
			std::cout << "Percentage step 2 result" << std::endl;
			if (p1.index() == result.index()) {
				p1Win++;
			}
			else if (p1.index() == p2.index())
			{
				p2 = result;
			}
			std::cout << "Percentage step 3 " << std::endl;
		}
		int winProcent = (100 * p1Win) / results;
		std::cout << "Percentage step 4 " << std::endl;
		if (winProcent > 50)
		{
			std::cout << "Percentage step 5.1 " << std::endl;
			return result_type(p1, winProcent);
		}
		else
		{
			std::cout << "Percentage step 5.2 " << std::endl;
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