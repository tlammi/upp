#include "upp/parser.hpp"

#include <iterator>
#include <iostream>
#include <fstream>
#include <string_view>
#include <map>
#include <vector>
#include <variant>

struct Value;

using Obj = std::map<std::string, Value>;

// Deque to avoid relocation on assignment
using List = std::deque<Value>;

using ValueBase = std::variant<std::monostate, int, std::string, Obj, List>;

struct Value: public ValueBase {
	using ValueBase::ValueBase;
};


struct ObjFactory{
	ObjFactory(){}

	void push_value(Value&& val){
		if(current_key == ""){
			std::get<List>(*stack.back()).push_back(std::move(val));
		} else {
			std::get<Obj>(*stack.back())[current_key] = val;
			current_key = "";
		}
	}

	void push_container(Value&& container){
		if(!stack.size()){
			root = std::move(container);
			stack.push_back(&root);
		} else if(current_key != ""){
			auto& obj = std::get<Obj>(*stack.back());
			obj[current_key] = std::move(container);
			stack.push_back(&obj.at(current_key));
			current_key = "";
		} else {
			auto& list = std::get<List>(*stack.back());
			list.push_back(std::move(container));
			stack.push_back(&list.back());
		}

	}

	void push_key(std::string&& str){
		current_key = str;
	}

	void move_up(){
		stack.pop_back();
	}

	Value root{};
	std::deque<Value*> stack{};
	std::string current_key{""};
};

namespace p = upp::parser;

int main(int argc, char** argv){
	if(argc != 2)
		throw std::runtime_error("Input JSON file required");

	std::ifstream fs{argv[1]};
	std::string str{std::istreambuf_iterator<char>(fs), std::istreambuf_iterator<char>()};

	auto json = ObjFactory();

	// Utility object so <Iter> does not need to be specified each time
	auto factory = p::Factory<std::string::const_iterator>();

	// Matches literal '{' and invokes callback on match
	auto obj_begin = factory.lit('{', [&](auto begin, auto end){
		(void)begin;
		(void)end;
		json.push_container(Obj());
	});

	auto obj_end = factory.lit('}', [&](auto begin, auto end){
		(void)begin;
		(void)end;
		json.move_up();
	});

	auto list_begin = factory.lit('[', [&](auto begin, auto end){
		(void)begin;
		(void)end;
		std::cerr << "list begin\n";
		json.push_container(List());
	});

	auto list_end = factory.lit(']', [&](auto begin, auto end){
		(void)begin;
		(void)end;
		std::cerr << "list end\n";
		json.move_up();
	});

	// Regex for matching quoted strings
	auto quoted = factory.regex(R"(".*?[^\\]")", "quoted string", [&](auto begin, auto end){
		json.push_value(std::string(begin+1, end-1));
	});

	// Copy matcher from quoted but override the callback
	auto key = factory.ast(quoted, [&](auto begin, auto end){
		json.push_key(std::string(begin+1, end-1));
	});

	// Match integer
	auto integer = factory.regex(R"(0|[1-9][0-9]*)", "integer", [&](auto begin, auto end){
		int i = std::stoi(std::string(begin, end));
		json.push_value(i);
	});

	// Forward declarations to allow recursive matching
	auto obj = factory.dyn_ast();
	auto list = factory.dyn_ast([](auto begin, auto end){
		std::cerr << "list: " << std::string(begin, end) << '\n';
	});

	// Match any of the alternatives
	auto value = (quoted | integer | obj | list);

	// Match "quoted str": <value>
	auto key_value = (key, factory.lit(':'), value);

	// Declaration of a JSON object
	// - -> zero or one
	// * -> zero to inf
	obj = (obj_begin, -(key_value), *(factory.lit(','), key_value), obj_end);
	list = (list_begin, -(value), *(factory.lit(','), value), list_end);
	
	auto obj_or_list = (obj | list);

	auto result = p::parse(str.cbegin(), str.cend(), obj_or_list, p::whitespaces);

	if(result)
		std::cerr << "Successfull parse\n";
	else{
		std::cerr << "failed to parse\n";
		std::cerr << argv[1] << ":" << p::error_msg(str.cbegin(), str.cend(), result) << '\n';
	}
}
