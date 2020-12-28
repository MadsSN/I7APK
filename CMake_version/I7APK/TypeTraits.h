#pragma once
template <typename T>
class TypeTraits;

struct WeakType {};
struct StrongType {};
struct NoType {};

template<>
struct TypeTraits<WeakType> {
	static const int hp_c = 51;
	static const int attack_c = 10;
	typedef WeakType type;
};

template<>
struct TypeTraits<StrongType> {
	static const int hp_c = 50;
	static const int attack_c = 5;
	typedef StrongType type;
};

template<>
struct TypeTraits<NoType> {
	static const int hp_c = 100;
	static const int attack_c = 13;
	typedef NoType type;
};
