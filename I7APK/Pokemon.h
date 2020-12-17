#include <ostream>
#include "Pokemon.cpp"

std::ostream& operator<<(std::ostream& out, const Pokemon& c);


template <typename T>
class TypeTraits;

class Pokemon {
public:
	Pokemon(size_t index, std::string navn, size_t hp, size_t attack) : _pokeIndex(index),
		_navn(navn),
		_hp(hp),
		_attack(attack) {

	}

	size_t _hp;
	size_t _attack;
	size_t _pokeIndex;
	std::string _navn;
};