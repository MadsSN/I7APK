#include "Traits.h"



template<>
struct TypeTraits<WeakType> {
	static size_t hp() { return 50; };
	static size_t attack() { return 10; };
};

template<>
struct TypeTraits<StrongType> {
	static size_t hp() { return 100; };
	static size_t attack() { return 5; };
};