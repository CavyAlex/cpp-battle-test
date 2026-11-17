cmake_minimum_required(VERSION 3.20)
project(sw_battle_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-Wall -Wextra -Wpedantic)

# Всё, кроме main, — в библиотеку: её используют и программа, и юнит-тесты
file(GLOB_RECURSE LIBRARY_SOURCES CONFIGURE_DEPENDS src/Core/*.?pp src/Features/*.?pp src/IO/*.?pp)
add_library(sw_battle STATIC ${LIBRARY_SOURCES})
target_include_directories(sw_battle PUBLIC src/)

add_executable(sw_battle_test src/main.cpp)
target_link_libraries(sw_battle_test PRIVATE sw_battle)

enable_testing()

# Сценарии: tests/**/*.txt, ожидания записаны в самом файле (см. tests/run_scenario.sh)
file(GLOB_RECURSE SCENARIOS CONFIGURE_DEPENDS tests/*.txt)
foreach(SCENARIO ${SCENARIOS})
	file(RELATIVE_PATH TEST_NAME ${CMAKE_CURRENT_SOURCE_DIR}/tests ${SCENARIO})
	string(REGEX REPLACE "\\.txt$" "" TEST_NAME ${TEST_NAME})
	add_test(NAME ${TEST_NAME}
		COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/tests/run_scenario.sh $<TARGET_FILE:sw_battle_test> ${SCENARIO})
endforeach()

# Юнит-тесты: tests/**/*.cpp
file(GLOB_RECURSE UNIT_TEST_SOURCES CONFIGURE_DEPENDS tests/*.cpp tests/*.hpp)
add_executable(unit_tests ${UNIT_TEST_SOURCES})
target_include_directories(unit_tests PRIVATE tests/)
# Тесты задают только нужные поля StubOptions, остальные намеренно берутся по умолчанию;
# начиная с clang 18 -Wextra предупреждает о таком в именованных инициализаторах
target_compile_options(unit_tests PRIVATE -Wno-missing-field-initializers)
target_link_libraries(unit_tests PRIVATE sw_battle)
add_test(NAME unit_tests COMMAND unit_tests)
