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
	const Pokemon& operator()(const T& arg) const 
	{
		return arg;
	}
};
//using std::variant<WeakPokemon, StrongPokemon, NoPokemon> = std::variant<WeakPokemon, StrongPokemon, NoPokemon>;
/*
struct std::variant<WeakPokemon, StrongPokemon, NoPokemon> : std::variant<WeakPokemon, StrongPokemon, NoPokemon>
{
	std::variant<WeakPokemon, StrongPokemon, NoPokemon>() = default;

	//Purpose choose not to use template argument
	std::variant<WeakPokemon, StrongPokemon, NoPokemon>(const WeakPokemon& x) : std::variant<WeakPokemon, StrongPokemon, NoPokemon>(x)
	{
	};

	std::variant<WeakPokemon, StrongPokemon, NoPokemon>(const StrongPokemon& x) : std::variant<WeakPokemon, StrongPokemon, NoPokemon>(x)
	{
	};

	std::variant<WeakPokemon, StrongPokemon, NoPokemon>(const NoPokemon& x) : std::variant<WeakPokemon, StrongPokemon, NoPokemon>(x)
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

	const Pokemon& get()
	{
		auto p = std::visit(PokeGetBase(), *this);
		return p;
	}

	void print()
	{
		std::cout << get() << std::endl;
	}
};
*/

std::ostream& operator<<(std::ostream& out, const std::variant<WeakPokemon, StrongPokemon, NoPokemon>& c)
{
	std::visit(PokeVisitor(), c);
	return out;
}
