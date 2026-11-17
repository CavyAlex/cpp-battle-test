#pragma once

#include <string>
#include <vector>

// Минимальный тестовый каркас без сторонних библиотек.
// CHECK работает и в Release: в отличие от assert, его не отключает NDEBUG.
namespace sw::test
{
	struct TestCase
	{
		const char* name;
		void (*run)();
	};

	std::vector<TestCase>& testCases();

	struct Failure
	{
		std::string message;
	};

	[[noreturn]]
	void fail(const char* file, int line, const std::string& message);
}

#define SW_CONCAT_IMPL(a, b) a##b
#define SW_CONCAT(a, b) SW_CONCAT_IMPL(a, b)

#define TEST(name)                                                                                                     \
	static void name();                                                                                                \
	static const bool SW_CONCAT(name, _registered) = (sw::test::testCases().push_back({#name, &name}), true);          \
	static void name()

#define CHECK(...)                                                                                                     \
	do                                                                                                                 \
	{                                                                                                                  \
		if (!(__VA_ARGS__))                                                                                            \
		{                                                                                                              \
			sw::test::fail(__FILE__, __LINE__, "CHECK(" #__VA_ARGS__ ")");                                             \
		}                                                                                                              \
	}                                                                                                                  \
	while (false)

#define CHECK_THROWS(expression, ExceptionType)                                                                        \
	do                                                                                                                 \
	{                                                                                                                  \
		bool thrown = false;                                                                                           \
		try                                                                                                            \
		{                                                                                                              \
			expression;                                                                                                \
		}                                                                                                              \
		catch (const ExceptionType&)                                                                                   \
		{                                                                                                              \
			thrown = true;                                                                                             \
		}                                                                                                              \
		if (!thrown)                                                                                                   \
		{                                                                                                              \
			sw::test::fail(__FILE__, __LINE__, "expected " #ExceptionType " from " #expression);                       \
		}                                                                                                              \
	}                                                                                                                  \
	while (false)
