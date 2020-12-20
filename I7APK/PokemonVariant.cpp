#include "PokemonVariant.h"
#include  <variant>


std::string PokemonVariant::name() {
	return get()._navn;
}

size_t PokemonVariant::hp() {
	return get()._hp;
}

size_t PokemonVariant::attack() {
	return get()._attack;
}

size_t PokemonVariant::pokeIndex() {
	return get()._pokeIndex;
}

const Pokemon& PokemonVariant::get() const
{
	return std::visit(PokeGetBase(), *this);
}

void PokemonVariant::print() const
{
	std::cout << get() << std::endl;
}

std::ostream& operator<<(std::ostream& out, const PokemonVariant& c)
{
	std::visit(PokeVisitor(), c);
	return out;
}