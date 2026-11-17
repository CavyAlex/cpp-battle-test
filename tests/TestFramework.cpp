#include "TestFramework.hpp"

#include <exception>
#include <iostream>

namespace sw::test
{
	std::vector<TestCase>& testCases()
	{
		static std::vector<TestCase> cases;
		return cases;
	}

	void fail(const char* file, int line, const std::string& message)
	{
		throw Failure{std::string(file) + ":" + std::to_string(line) + ": " + message};
	}
}

int main()
{
	int failed = 0;
	for (const auto& testCase : sw::test::testCases())
	{
		try
		{
			testCase.run();
		}
		catch (const sw::test::Failure& failure)
		{
			++failed;
			std::cerr << "FAIL " << testCase.name << "\n  " << failure.message << "\n";
		}
		catch (const std::exception& error)
		{
			++failed;
			std::cerr << "FAIL " << testCase.name << "\n  unexpected exception: " << error.what() << "\n";
		}
	}
	std::cerr << sw::test::testCases().size() - failed << " passed, " << failed << " failed\n";
	return failed == 0 ? 0 : 1;
}
