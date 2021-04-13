#pragma once

#include "upp/impl/parser/result.hpp"
#include "upp/impl/parser/ctx.hpp"

namespace upp{
namespace parser{

template<class Iter>
class Matcher{
public:
	virtual ~Matcher(){}
	virtual bool match(Ctx<Iter>& ctx) const;
};

}
}
