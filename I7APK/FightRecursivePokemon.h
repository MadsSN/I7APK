#pragma once
#include <iostream>


template<int health, int dmg>
struct Attack {
	inline static const int hp = (health - dmg) < 0 ? 0 : (health - dmg);
};

template<const int p1_dmg, const int p1_hp, const int p2_dmg, const int p2_hp>
void FightRecursivePokemon(const std::string& p1_name, const std::string& p2_name) {
	std::cout << p1_name << " has " << p1_hp << " hp" << std::endl;

	if constexpr (p1_hp == 0) {
		std::cout << p1_name << " died for your entertainment" << std::endl;
	}
	else {
		// give dmg
		std::cout << p1_name << " survived and attacks " << p2_name << " for " << p1_dmg << " hp" << std::endl;
		// next round
		FightRecursivePokemon<p2_dmg, Attack<p2_hp, p1_dmg>::hp, p1_dmg, p1_hp>(p2_name, p1_name);
	}
};

template<typename P1, typename P2>
void FightRecursivePokemon(const P1& p1, const P2& p2) {
	std::cout << "BATTLE MUSIC" << std::endl;

	FightRecursivePokemon<P1::type_traits::attack_c, P1::type_traits::hp_c, P2::type_traits::attack_c, P2::type_traits::hp_c>(p1._navn, p2._navn);
};