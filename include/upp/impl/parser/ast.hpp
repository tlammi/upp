#pragma once

#include <memory>

#include "upp/impl/parser/result.hpp"
#include "upp/impl/parser/matcher.hpp"
#include "upp/impl/parser/joined.hpp"

namespace upp{
namespace parser{

template<class Iter, class M, class OnMatch=void>
class Ast{
public:
	using iterator = Iter;

	Ast(M&& m, OnMatch&& on_match): matcher_{std::move(m)}, on_match_{std::move(on_match)}{}

	Result<Iter> match(Iter begin, Iter end) const {
		auto res = matcher_.match(begin, end);
		if(res) on_match_(begin, res.iter);
		return res;
	}

private:
	M matcher_;
	OnMatch on_match_;
};

template<class Iter, class M>
class Ast<Iter, M, void>{
public:
	using iterator = Iter;

	Ast(M&& m): matcher_{std::move(m)}{}

	Result<Iter> match(Iter begin, Iter end) const {
		return  matcher_.match(begin, end);
	}

private:
	M matcher_;
};


template<class Iter, class OnMatch=void>
class DynAst{
public:
	using iterator = Iter;

	DynAst(Matcher<Iter>* ptr, OnMatch&& on_match): matcher_{ptr}, on_match_{std::move(on_match)}{}


	Result<Iter> match(Iter begin, Iter end) const {
		auto res = matcher_->match(begin, end);
		if(res) on_match_(begin, res.iter);
		return res;
	}

private:
	std::unique_ptr<Matcher<Iter>> matcher_;
	OnMatch on_match_;
};

template<class Iter>
class DynAst<Iter, void>{
public:
	using iterator = Iter;

	DynAst(Matcher<Iter>* ptr): matcher_{ptr}{}


	Result<Iter> match(Iter begin, Iter end) const {
		return matcher_->match(begin, end);
	}

private:
	std::unique_ptr<Matcher<Iter>> matcher_;
};

}
}
