#include <iostream>
#include "Pokemon.h"

Pokemon::Pokemon(size_t index, std::string navn, size_t hp, size_t attack) : _pokeIndex(index),
	_navn(navn),
	_hp(hp),
	_attack(attack){
}

bool Pokemon::operator==(const Pokemon poke)
{
	return _pokeIndex == poke._pokeIndex;
}

Pokemon::Pokemon(const Pokemon& poke) : Pokemon(poke._pokeIndex, poke._navn, poke._hp, poke._attack)
{
}

Pokemon::Pokemon(Pokemon&& poke) noexcept :
	_hp(0),
	_attack(0)
{
	swap(*this, poke);
}

Pokemon& Pokemon::operator=(const Pokemon& poke)
{
	Pokemon copy(poke);
	swap(*this, copy);
	return *this;
}

Pokemon& Pokemon::operator=(Pokemon&& poke) noexcept
{
	if (this != &poke)
	{
		swap(*this, poke);
	}
	return *this;
}

void Pokemon::swap(Pokemon& left, Pokemon& rigth) const noexcept
{
	std::swap(left._pokeIndex, rigth._pokeIndex);
	std::swap(left._navn, rigth._navn);
	std::swap(left._attack, rigth._attack);
	std::swap(left._hp, rigth._hp);
}

std::ostream& operator<<(std::ostream& out, const Pokemon& c)
{
	out << "Index: " << c._pokeIndex << std::endl;
	out << "Navn: " << c._navn << std::endl;
	out << "HP: " << c._hp << std::endl;
	out << "Attack: " << c._attack << std::endl;
	return out;
}