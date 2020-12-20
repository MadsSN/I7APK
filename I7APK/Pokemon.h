#pragma once
#include <ostream>

class Pokemon {
public:
	Pokemon(size_t index, std::string navn, size_t hp, size_t attack);

	size_t _pokeIndex;
	std::string _navn;
	size_t _hp;
	size_t _attack;

	bool operator==(const Pokemon poke);

	Pokemon(const Pokemon& poke);

	Pokemon(Pokemon&& poke) noexcept;

	Pokemon& operator=(const Pokemon& poke);

	Pokemon& operator=(Pokemon&& poke) noexcept;

	void swap(Pokemon& left, Pokemon& rigth) const noexcept;
};

std::ostream& operator<<(std::ostream& out, const Pokemon& c);