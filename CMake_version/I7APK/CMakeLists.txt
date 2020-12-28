# CMakeList.txt : CMake project for I7APK, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


project ("I7APK")

find_package(Boost 1.66.0)

# Add source to this project's executable.
include_directories(${Boost_INCLUDE_DIR})
add_executable (I7APK "I7APK.cpp" "Pokemon.cpp" "Pokemon.h" "CompilePokemon.h" "FightPokemon.h" "FightPokemonAsync.h" "FightRecursivePokemon.h" "IfThenElse.h" "Pokedex.h" "PokemonFightCalculator" "PokemonStatsResource.h" "PokemonVariant.h" "TypeList.h" "TypePokemon.h" "TypeTraits.h")

#target_link_libraries(anyExecutable Boost::signals2)
# TODO: Add tests and install targets if needed.
