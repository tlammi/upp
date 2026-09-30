#include <gtest/gtest.h>

#include <upp/sbool.hpp>

using upp::sbool;

static_assert(!sbool());
static_assert(sbool(true));
static_assert(!sbool(false));

static_assert(sbool() == sbool());
static_assert(sbool() == sbool(false));
static_assert(sbool() != sbool(true));

static_assert(sbool() == false);
static_assert(sbool() != true);
static_assert(sbool() < true);
static_assert(sbool(true) > false);
static_assert(sbool(true) <= true);
static_assert(sbool(true) >= false);

static_assert(!std::constructible_from<sbool, const char*>,
              "sbool accepts string literal");
static_assert(!std::constructible_from<sbool, float>, "sbool accepts float");
static_assert(!std::constructible_from<sbool, double>, "sbool accepts float");
static_assert(!std::constructible_from<sbool, decltype(0)>,
              "sbool accepts integer");
