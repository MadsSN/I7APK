#pragma once
#include <ostream>
//#include "PokemonStatsResource.h"
#include <memory_resource>

class Pokemon {
public:
	using allocator_type = std::pmr::polymorphic_allocator<std::byte>;

	Pokemon(size_t index, std::string navn, size_t hp, size_t attack, allocator_type alloc = {});

	size_t _pokeIndex;
	std::string _navn;
	size_t _hp;
	size_t _attack;
	allocator_type _alloc;

	bool operator==(const Pokemon poke);

	Pokemon(const Pokemon& poke);

	Pokemon(const Pokemon& poke, allocator_type alloc);

	Pokemon(Pokemon&& poke) noexcept;

	Pokemon& operator=(const Pokemon& poke);

	Pokemon& operator=(Pokemon&& poke) noexcept;

	void swap(Pokemon& left, Pokemon& rigth) const noexcept;

	allocator_type get_allocator();
};

std::ostream& operator<<(std::ostream& out, const Pokemon& c);