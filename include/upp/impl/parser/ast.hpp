#pragma once

#include <memory>

#include "upp/impl/parser/result.hpp"
#include "upp/impl/parser/matcher.hpp"
#include "upp/impl/parser/joined.hpp"

namespace upp{
namespace parser{

template<class Iter>
class Factory;

template<class Iter, class OnMatch>
class DynAst;

template<class Iter, class M, class OnMatch=void>
class Ast{
	template<class, class>
	friend class DynAst;
	friend class Factory<Iter>;
public:

	Ast(M&& m, OnMatch&& on_match): matcher_{std::move(m)}, on_match_{std::move(on_match)}{}
	Ast(const M& m, OnMatch&& on_match): matcher_{m}, on_match_{std::move(on_match)}{} 

	Result<Iter> match(Iter begin, Iter end, Iter(*skipper)(Iter, Iter)=nullptr) const {
		if(skipper) begin = skipper(begin, end);
		auto res = matcher_.match(begin, end, skipper);
		if(res) on_match_(begin, res.iter);
		return res;
	}

private:
	M matcher_;
	OnMatch on_match_;
};

template<class Iter, class M>
class Ast<Iter, M, void>{
	template<class, class>
	friend class DynAst;
	friend class Factory<Iter>;
public:

	Ast(M&& m): matcher_{std::move(m)}{}
	Ast(const M& m): matcher_{m}{} 

	Result<Iter> match(Iter begin, Iter end, Iter(*skipper)(Iter, Iter)=nullptr) const {
		if(skipper) begin = skipper(begin, end);
		return matcher_.match(begin, end, skipper);
	}

private:
	M matcher_;
};


template<class Iter, class OnMatch=void>
class DynAst{
	friend class Factory<Iter>;
public:

	DynAst(Matcher<Iter>* ptr, OnMatch&& on_match): matcher_{ptr}, on_match_{std::move(on_match)}{}

	template<class M, class OnMatch2>
	DynAst(Ast<Iter, M, OnMatch2>&& other): matcher_{std::make_unique<M>(std::move(other.matcher_))}{}

	template<class M, class OnMatch2>
	DynAst& operator=(Ast<Iter, M, OnMatch2>&& other){
		matcher_ = std::make_unique<M>(std::move(other.matcher_));
		return *this;
	}

	Result<Iter> match(Iter begin, Iter end, Iter(*skipper)(Iter, Iter)=nullptr) const {
		if(skipper) begin = skipper(begin, end);
		auto res = matcher_->match(begin, end, skipper);
		if(res) on_match_(begin, res.iter);
		return res;
	}

private:
	std::unique_ptr<Matcher<Iter>> matcher_;
	OnMatch on_match_;
};

template<class Iter>
class DynAst<Iter, void>{
	friend class Factory<Iter>;
public:

	DynAst(Matcher<Iter>* ptr): matcher_{ptr}{}

	template<class M, class OnMatch2>
	DynAst(Ast<Iter, M, OnMatch2>&& other): matcher_{std::make_unique<M>(std::move(other.matcher_))}{}

	template<class M, class OnMatch2>
	DynAst& operator=(Ast<Iter, M, OnMatch2>&& other){
		matcher_ = std::make_unique<M>(std::move(other.matcher_));
		return *this;
	}

	Result<Iter> match(Iter begin, Iter end, Iter(*skipper)(Iter, Iter)=nullptr) const {
		if(skipper) begin = skipper(begin, end);
		return matcher_->match(begin, end, skipper);
	}

private:
	std::unique_ptr<Matcher<Iter>> matcher_;
};

}
}

