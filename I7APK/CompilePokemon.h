#include "TypeTraits.h"
#pragma once
template<typename TType, size_t PokedexIndex = 0>
class CompilePokemon {
	BOOST_STATIC_ASSERT(
		boost::is_same<TType, WeakType>::value ||
		boost::is_same<TType, StrongType>::value ||
		boost::is_same<TType, NoType>::value);
public:
	CompilePokemon(std::string pokemonName) : name(pokemonName) {}

	std::string name;

	static const size_t pokedexIndex = PokedexIndex;

	typedef TypeTraits<TType> type_traits;
	typedef typename TypeTraits<TType>::type type;
};