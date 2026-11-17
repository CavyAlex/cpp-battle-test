#pragma once

#include <charconv>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace sw
{
	class CommandParserVisitor
	{
	private:
		std::istream& _stream;
		std::string _commandName;

	public:
		CommandParserVisitor(std::istream& stream, std::string commandName) :
				_stream(stream),
				_commandName(std::move(commandName))
		{}

		// Поле — неотрицательное целое, целиком помещающееся в свой тип: без знака, лишних символов и переполнения
		template <class TField>
		void visit(const char* name, TField& field)
		{
			static_assert(std::is_unsigned_v<TField>, "Command fields are expected to be unsigned integers");

			std::string token;
			_stream >> token;
			const auto [end, error] = std::from_chars(token.data(), token.data() + token.size(), field);
			if (token.empty() || error != std::errc{} || end != token.data() + token.size())
			{
				throw std::runtime_error(_commandName + ": invalid or missing value for '" + name + "'");
			}
		}
	};
}
