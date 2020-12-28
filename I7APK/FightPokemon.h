#pragma once
#include "PokemonVariant.h"
#include "IfThenElse.h"
struct FirstWinTag {};
struct SecondWinTag {};

template<typename P1, typename P2>
struct is_first_winner
{
	inline static constexpr bool value = P1::type_traits::hp_c / P2::type_traits::attack_c > P2::type_traits::hp_c / P1::type_traits::attack_c;
};

template<typename P1, typename P2>
std::variant<WeakPokemon, StrongPokemon, NoPokemon> FightWithTags(P1& p1, P2& p2)
{
	return FightWithTagsImpl(p1, p2,
		typename IfThenElse<FirstWinTag, SecondWinTag, is_first_winner<P1, P2>::value>::Type());
}

template<typename P1, typename P2>
P1& FightWithTagsImpl(P1& p1, P2& p2, FirstWinTag)
{
	std::cout << "BATTLE MUSIC - First winner chicken dinner" << std::endl;
	std::cout << p1._navn << " won!" << std::endl;
	std::cout << "and " << p2._navn << " died for your entertainment" << std::endl;
	return p1;
}

template<typename P1, typename P2>
P2& FightWithTagsImpl(P1& p1, P2& p2, SecondWinTag)
{
	std::cout << "BATTLE MUSIC - Second winner chicken dinner" << std::endl;
	std::cout << p2._navn << " won!" << std::endl;
	std::cout << "and " << p1._navn << " died for your entertainment" << std::endl;
	return p2;
}

template<typename P1, typename P2>
void FightWithConstexpr(P1&& p1, P2&& p2) {
	std::cout << "BATTLE MUSIC" << std::endl;
	if constexpr (is_first_winner<P1, P2>::value) {
		std::cout << p1._navn << " won!" << std::endl;
		std::cout << "and " << p2._navn << " died for your entertainment" << std::endl;
	}
	else {
		std::cout << p1._navn << " died for your entertainment" << std::endl;
		std::cout << "and " << p2._navn << " won!" << std::endl;
	}
};

template<typename P1, typename P2>
std::variant<WeakPokemon, StrongPokemon, NoPokemon> FightWithConstexpr(P1& p1, P2& p2) {
	std::cout << "BATTLE MUSIC" << std::endl;
	if constexpr (is_first_winner<P1, P2>::value) {
		std::cout << p1._navn << " won!" << std::endl;
		std::cout << "and " << p2._navn << " died for your entertainment" << std::endl;
		return p1;
	}
	else {
		std::cout << p1._navn << " died for your entertainment" << std::endl;
		std::cout << "and " << p2._navn << " won!" << std::endl;
		return p2;
	}
};

struct PokeBattleVisitor
{
	template<typename P1, typename P2>
	std::variant<WeakPokemon, StrongPokemon, NoPokemon> operator()(const P1& arg, const P2& challenger)
	{
		return FightWithConstexpr(challenger, arg);
	}
};