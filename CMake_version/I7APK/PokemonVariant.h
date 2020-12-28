#pragma once

#include "TypePokemon.h"
#include <iostream>
#include <variant>


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

	//Purpose choose not to use template argument
	PokemonVariant(const WeakPokemon& x) : std::variant<WeakPokemon, StrongPokemon, NoPokemon>(x)
	{
	};

	PokemonVariant(const StrongPokemon& x) : std::variant<WeakPokemon, StrongPokemon, NoPokemon>(x)
	{
	};

	PokemonVariant(const NoPokemon& x) : std::variant<WeakPokemon, StrongPokemon, NoPokemon>(x)
	{
	};


	std::string name() const {
		return get()._navn;
	}

	size_t hp() const {
		return get()._hp;
	}

	size_t attack() const {
		return get()._attack;
	}

	size_t pokeIndex() const {
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
