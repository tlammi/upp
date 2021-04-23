#include "fixture.hpp"

TEST_F(ParserFixture, FactoryDynAst){
	auto dynast0 = factory.dyn_ast();
	auto dynast1 = factory.dyn_ast([](auto, auto){});
}

TEST_F(ParserFixture, FactoryRegex){
	auto re0 = factory.regex(R"(.*)");
	auto re1 = factory.regex(R"(.*\.)", [](auto, auto){});
	auto re2 = factory.ast(re0, [](auto, auto){});
}

TEST_F(ParserFixture, FactoryCharLiteral){
	auto c0 = factory.lit('!');
	auto c1 = factory.lit('?', [](auto, auto){});
	auto c2 = factory.ast(c0, [](auto, auto){});
	auto c3 = factory.ast(factory.lit('0'), [](auto, auto){});
}

TEST_F(ParserFixture, FactoryStrLiteral){
	auto s0 = factory.lit("!");
	auto s1 = factory.lit("?", [](auto, auto){});
	auto s2 = factory.ast(s0, [](auto, auto){});
	auto s3 = factory.ast(factory.lit("0"), [](auto, auto){});
}

TEST_F(ParserFixture, FactoryJoined){
	auto c = factory.lit('!');
	auto s = factory.lit("asdf");
	auto j0 = factory.ast((c, s), [](auto, auto){});
	auto j2 = factory.ast(j0);
}

TEST_F(ParserFixture, FactoryOr){
	auto c = factory.lit('"');
	auto s = factory.lit("str");
	auto or0 = factory.ast((c | s), [](auto, auto){});
	auto or1 = factory.ast(or0);
	auto or2 = factory.ast((factory.lit('a') | factory.lit('b')));
}

TEST_F(ParserFixture, FactoryOptional){
	auto c = factory.lit('c');
	auto opt0 = factory.ast(-c, [](auto, auto){});
	auto opt1 = factory.ast(opt0);
	auto opt2 = factory.ast(-factory.lit('d'));
}

TEST_F(ParserFixture, FactoryKleeneStar){
	auto c = factory.lit('d');
	auto star0 = factory.ast(*c, [](auto, auto){});
	auto star1 = factory.ast(star0, [](auto, auto){});
	auto star2 = factory.ast(*factory.lit("string"), [](auto, auto){});
}

TEST_F(ParserFixture, FactoryKleenePlus){
	auto c = factory.lit('e');
	auto p0 = factory.ast(+c, [](auto, auto){});
	auto p1 = factory.ast(p0);
	auto p2 = factory.ast(+factory.regex("aaaaa"));
}

