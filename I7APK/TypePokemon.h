#pragma once
#include "TypeTraits.h"
#include "Pokemon.h"
#include "boost/signals2.hpp"

template<typename TType>
class TypePokemon : public Pokemon {
	BOOST_STATIC_ASSERT(
		boost::is_same<TType, WeakType>::value ||
		boost::is_same<TType, StrongType>::value ||
		boost::is_same<TType, NoType>::value);
public:
	TypePokemon() : TypePokemon(0, "")
	{};

	TypePokemon(size_t index, std::string navn) : Pokemon(index, navn, TypeTraits<TType>::hp_c, TypeTraits<TType>::attack_c)
	{};

	typedef TypeTraits<TType> type_traits;
};

using WeakPokemon = TypePokemon<WeakType>;
using StrongPokemon = TypePokemon<StrongType>;
using NoPokemon = TypePokemon<NoType>;