#pragma once

#include <vector>
#include <variant>
#include <string>
#include <map>

namespace upp{
namespace property_tree{
namespace detail{

}

class Node;

using ValueBase = std::variant<int, double, std::string>;

class Value: public ValueBase {
public:

	using ValueBase::ValueBase;

	template<class T>
	T& get(){
		return std::get<T>(*this);
	}

	template<class T>
	const T& get() const {
		return std::get<T>(*this);
	}

	template<class T>
	T cast() const;


	bool operator==(int i) const {
		if(std::holds_alternative<int>(*this))
			return std::get<int>(*this) == i;
		return false;
	}

	bool operator==(std::string_view str) const {
		if(std::holds_alternative<std::string>(*this))
			return std::get<std::string>(*this) == str;
		return false;
	}
};


template<>
inline int Value::cast<int>() const {
	if(std::holds_alternative<int>(*this))
		return std::get<int>(*this);
	if(std::holds_alternative<double>(*this))
		return std::get<double>(*this);
	if(std::holds_alternative<std::string>(*this))
		return std::stoi(std::get<std::string>(*this));
	throw std::runtime_error("Unknown conversion");
}

template<>
inline double Value::cast<double>() const {
	if(std::holds_alternative<int>(*this))
		return std::get<int>(*this);
	if(std::holds_alternative<double>(*this))
		return std::get<double>(*this);
	if(std::holds_alternative<std::string>(*this))
		return std::stod(std::get<std::string>(*this));
	throw std::runtime_error("Unknown conversion");
}

template<>
inline std::string Value::cast<std::string>() const {
	if(std::holds_alternative<int>(*this))
		return std::to_string(std::get<int>(*this));
	if(std::holds_alternative<double>(*this))
		return std::to_string(std::get<double>(*this));
	if(std::holds_alternative<std::string>(*this))
		return std::get<std::string>(*this);
	throw std::runtime_error("Unknown conversion");
}

using List = std::vector<Node>;
using Dict = std::map<std::string, Node, std::less<>>;

using NodeBase = std::variant<Value, List, Dict>;

class Node: public NodeBase{
public:

	enum class Type{
		Value,
		List,
		Dict,
	};

	using NodeBase::NodeBase;

	constexpr Type type() const {
		return static_cast<Type>(index());
	}

	Node& operator[](size_t idx){
		return std::get<List>(*this).at(idx);
	}

	const Node& operator[](size_t idx) const {
		return std::get<List>(*this).at(idx);
	}

	Node& operator[](std::string_view key){
		auto& dict = std::get<Dict>(*this);
		auto match = dict.find(key);
		if(match == dict.end()){
			auto [iter, success] = dict.insert(Dict::value_type(key, Node{}));
			(void)success;
			return iter->second;
		}
		return match->second;
	}

	Node& at(std::string_view key){
		auto& dict = std::get<Dict>(*this);
		auto match = dict.find(key);
		if(match != dict.end())
			return match->second;
		throw std::runtime_error("Node::at()");
	}

	const Node& at(std::string_view key) const {
		const auto& dict = std::get<Dict>(*this);
		auto match = dict.find(key);
		if(match != dict.end())
			return match->second;
		throw std::runtime_error("Node::at()");
	}

	Value& operator*(){
		return std::get<Value>(*this);
	}

	const Value& operator*() const {
		return std::get<Value>(*this);
	}

	Value* operator->(){
		return &std::get<Value>(*this);
	}

	const Value* operator->() const {
		return &std::get<Value>(*this);
	}


	List& list() {
		return std::get<List>(*this);
	}


	const List& list() const {
		return std::get<List>(*this);
	}

	Dict& dict() {
		return std::get<Dict>(*this);
	}

	const Dict& dict() const {
		return std::get<Dict>(*this);
	}

};

}
}
