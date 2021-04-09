#pragma once

#include "upp/impl/parser/result.hpp"

namespace upp{
namespace parser{

template<class Iter>
class Matcher{
public:
	virtual Result<Iter> match(Iter begin, Iter end, Iter(*skipper)(Iter, Iter)) const;
};

}
}
