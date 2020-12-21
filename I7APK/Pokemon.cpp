#include <iostream>
#include "Pokemon.h"

Pokemon::Pokemon(size_t index, std::string navn, size_t hp, size_t attack, std::pmr::polymorphic_allocator<std::byte> alloc) : _pokeIndex(index),
	_navn(navn),
	_hp(hp),
	_attack(attack),
	_alloc(alloc) {
	std::cout << "Ordinary ctor " << _navn << "\n";
}

bool Pokemon::operator==(const Pokemon poke)
{
	return _pokeIndex == poke._pokeIndex;
}

Pokemon::Pokemon(const Pokemon& poke) : Pokemon(poke._pokeIndex, poke._navn, poke._hp, poke._attack)
{
	std::cout << "Copy ctor " << _navn << "\n";
}

Pokemon::Pokemon(const Pokemon& poke, std::pmr::polymorphic_allocator<std::byte> alloc) : Pokemon(poke._pokeIndex, poke._navn, poke._hp, poke._attack, alloc)
{
	
}

Pokemon::Pokemon(Pokemon&& poke) noexcept :
	_hp(0),
	_attack(0)
{
	swap(*this, poke);
	std::cout << "Move ctor " << _navn << "\n";
}

Pokemon& Pokemon::operator=(const Pokemon& poke)
{
	std::cout << "Assignment ctor " << _navn << "\n";
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
	std::cout << "Move Assignment ctor " << _navn << "\n";
	return *this;
}

void Pokemon::swap(Pokemon& left, Pokemon& rigth) const noexcept
{
	std::swap(left._pokeIndex, rigth._pokeIndex);
	std::swap(left._navn, rigth._navn);
	std::swap(left._attack, rigth._attack);
	std::swap(left._hp, rigth._hp);
}

std::pmr::polymorphic_allocator<std::byte> Pokemon::get_allocator() { return _alloc; }

std::ostream& operator<<(std::ostream& out, const Pokemon& c)
{
	out << "Index: " << c._pokeIndex << std::endl;
	out << "Navn: " << c._navn << std::endl;
	out << "HP: " << c._hp << std::endl;
	out << "Attack: " << c._attack << std::endl;
	return out;
}