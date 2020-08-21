#pragma once
#include <vector>
namespace upp {
namespace cli {

enum class TokenType {
		ShortFlag,
		LongFlag,
		Other,
};
struct Token {
		TokenType type;
		const char* ptr;

		Token(TokenType type_, const char* ptr_) : type{type_}, ptr{ptr_} {}
		bool operator==(const Token& rhs) const {
				return type == rhs.type && ptr == rhs.ptr;
		}
};

inline std::vector<Token> tokenize(const char* arg) {
		auto is_flag = [](const char* str) {
				return str[0] == '-' && str[1] != '\0';
		};
		auto is_long_flag = [](const char* str) {
				return str[1] == '-' && str[2] != '\0';
		};

		if (is_flag(arg)) {
				if (is_long_flag(arg)) {
						return {Token{TokenType::LongFlag, arg + 2}};
				}
				const char* c = &arg[1];
				std::vector<Token> out;
				while (*c != '\0') {
						out.emplace_back(TokenType::ShortFlag, c);
						++c;
				}
				return out;
		}
		return {Token{TokenType::Other, arg}};
}
}  // namespace cli
}  // namespace upp