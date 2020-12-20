#pragma once
#include  <variant>
#include "TypePokemon.h"
#include <iostream>


#include <list>
#include <iterator>
#include <thread>
#include <algorithm>
#include <mutex>
#include "boost/signals2.hpp"
#include "CompilePokemon.h"
#include "TypeTraits.h"
#include "Pokemon.h"

struct PokemonVariant : std::variant<WeakPokemon, StrongPokemon, NoPokemon>
{
	//PokemonVariant() = default;

	template<typename T>
	PokemonVariant(const T& x);

	std::string name();

	size_t hp();

	size_t attack();

	size_t pokeIndex();

	const Pokemon& get() const;

	void print() const;
};

std::ostream& operator<<(std::ostream& out, const PokemonVariant& c);

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