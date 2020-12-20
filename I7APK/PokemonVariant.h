#pragma once

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
#include  <variant>
#include "Pokemon.h"

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
	PokemonVariant(const T& x) : std::variant<WeakPokemon, StrongPokemon, NoPokemon>(x)
	{
		
	};

	std::string name() {
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
	std::visit(PokeVisitor(), c);
	return out;
}
