#pragma once
#include <iostream>
#include "CompilePokemon.h"

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

	std::cout << "Lets examine the type of " << charmeleon.name << " at pokedex entry #" << charmeleon.pokedexIndex << std::endl;

	std::string charmeleonType = typeid(AtIndex<PokemonTypeList, decltype(charmeleon)::pokedexIndex>::type).name();
	std::cout << charmeleon.name + " has type " << removeStructFromTypeName(charmeleonType);

	std::cout << "\n\nThe types of all the six are:" << std::endl;
	std::cout << Print<PokemonTypeList> << std::endl;
}