#include <gtest/gtest.h>
#include "jason.hpp"

class ObjectTest : public ::testing::Test{
	protected:
		void SetUp() override
		{
			constexpr const char* str = "{\"a\":1,\"b\":\"teste\",\"c\":null,\"d\":1.1,\"e\":-1,\"f\":[1,2,3],\"g\":{\"h\":1}}";
			EXPECT_TRUE(jason::parse(obj_.doc(), str));

			obj_.swap(obj_.doc());
		}

		jason::object_doc obj_;
};

class ArrayTest : public ::testing::Test{
	protected:
		void SetUp() override
		{
			constexpr const char* str = "[1,-1,3,\"string\", null, 7.4,-589,[1, \"teset\", 23, 2.3],{\"a\":1}]";
			EXPECT_TRUE(jason::parse(arr_.doc(), str));

			arr_.swap(arr_.doc());
		}

		jason::array_doc arr_;
};

TEST(JasonTest, ParseTrue) {
	//Parsing a object
	constexpr const char* str_obj = "{\"a\":1,\"b\":\"teste\",\"c\":null,\"d\":1.1,\"e\":-1,\"f\":[1,2,3],\"g\":{\"h\":1}}";
	jason::document base_obj;
	EXPECT_TRUE(jason::parse(base_obj, str_obj));

	//Parsing array
	constexpr const char* str_arr = "[1,-1,3,\"string\",7.4,-589,[1, \"teset\", 23, 2.3],{\"a\":1}]";
	jason::document base_arr;
	EXPECT_TRUE(jason::parse(base_arr, str_arr));

	//Parsing interger
	constexpr const char* str_int = "1";
	jason::document base_int;
	EXPECT_TRUE(jason::parse(base_int, str_int));
}

TEST(JasonTest, ParseFail) {
	//Parsing a object
	constexpr const char* str_obj = "\"a\":1,\"b\":\"teste\",\"c\":null,\"d\":1.1,\"e\":-1,\"f\":[1,2,3],\"g\":{\"h\":1}}";
	jason::document base_obj;
	EXPECT_FALSE(jason::parse(base_obj, str_obj));

	//Parsing array
	constexpr const char* str_arr = "[1,-1,3,\"string\",7.4,-589,[1, \"teset\", 23, 2.3],{\"a\":1}";
	jason::document base_arr;
	EXPECT_FALSE(jason::parse(base_arr, str_arr));

	//Parsing interger
	constexpr const char* str_int = "/sd1";
	jason::document base_int;
	EXPECT_FALSE(jason::parse(base_int, str_int));
}

TEST_F(ObjectTest, CheckKeysTrue){
	EXPECT_TRUE(obj_.has("a"));
	EXPECT_TRUE(obj_.has("b"));
	EXPECT_TRUE(obj_.has("c"));
	EXPECT_TRUE(obj_.has("d"));
	EXPECT_TRUE(obj_.has("e"));
	EXPECT_TRUE(obj_.has("f"));
	EXPECT_TRUE(obj_.has("g"));
}

TEST_F(ObjectTest, CheckKeysFalse){
	EXPECT_FALSE(obj_.has("h"));
	EXPECT_FALSE(obj_.has("i"));
	EXPECT_FALSE(obj_.has("j"));
	EXPECT_FALSE(obj_.has("l"));
	EXPECT_FALSE(obj_.has("m"));
	EXPECT_FALSE(obj_.has("n"));
	EXPECT_FALSE(obj_.has("o"));
}

TEST_F(ObjectTest, CheckKeysTypesTrue){
	using namespace jason::literals;

	EXPECT_TRUE(jason::verify(obj_, "a"_u));
	EXPECT_TRUE(jason::verify(obj_, "b"_s));
	EXPECT_TRUE(jason::verify(obj_, "c"_null));
	EXPECT_TRUE(jason::verify(obj_, "d"_d));
	EXPECT_TRUE(jason::verify(obj_, "e"_i));
	EXPECT_TRUE(jason::verify(obj_, "f"_a));
	EXPECT_TRUE(jason::verify(obj_, "g"_o));
	//Vefiring all at same time
	EXPECT_TRUE(jason::verify(obj_, "a"_u, "b"_s, "c"_null, "d"_d, "e"_i, "f"_a, "g"_o));
}

TEST_F(ObjectTest, CheckKeysTypesFalse){
	using namespace jason::literals;

	EXPECT_FALSE(jason::verify(obj_, "a"_s));
	EXPECT_FALSE(jason::verify(obj_, "b"_u));
	EXPECT_FALSE(jason::verify(obj_, "c"_u64));
	EXPECT_FALSE(jason::verify(obj_, "d"_i));
	EXPECT_FALSE(jason::verify(obj_, "e"_s));
	EXPECT_FALSE(jason::verify(obj_, "f"_o));
	EXPECT_FALSE(jason::verify(obj_, "g"_a));
	//"c" is null
	EXPECT_FALSE(jason::verify(obj_, "a"_u, "b"_s, "c"_u, "d"_d, "e"_i, "f"_a, "g"_o));
}

TEST_F(ObjectTest, GetBasicTypesTrue){
	using namespace jason::literals;

	//All this return a optional
	EXPECT_EQ(jason::get_uint(obj_["a"]).value_or(0), 1);
	EXPECT_EQ(obj_["a"_u].value_or(0), 1);
	EXPECT_STREQ(obj_["b"_s].value_or("erro"), "teste");
	EXPECT_EQ(obj_["d"_d].value_or(0.0), 1.1);
	EXPECT_EQ(obj_["e"_i].value_or(0), -1);
}

TEST_F(ObjectTest, GetBasicTypesFail){
	using namespace jason::literals;

	//All this return a optional
	EXPECT_EQ(obj_["h"_u].value_or(0), 0);					//"h" doesn't exists
	EXPECT_STREQ(obj_["a"_s].value_or("error"), "error");	//"a" is not a string
	EXPECT_EQ(obj_["b"_i].value_or(0), 0);					//"b" is not integer
	EXPECT_EQ(obj_["d"_u].value_or(0), 0);					//"d" is not unsigned
	EXPECT_EQ(obj_["e"_u64].value_or(0), 0);				//"e" is not unsigned64
}

TEST_F(ObjectTest, ObjectAddKeyValue){
	using namespace jason::literals;

	obj_.set("h", "my literal string");
	EXPECT_STREQ(obj_["h"_s].value_or("error"), "my literal string");

	obj_.set("i", std::string{"my string"});
	EXPECT_STREQ(obj_["i"_s].value_or("error"), "my string");

	obj_.set("j", 10);
	EXPECT_EQ(obj_["j"_u].value_or(0), 10);

	obj_.set("k", -10);
	EXPECT_EQ(obj_["k"_i].value_or(0), -10);

	obj_.set("l", 1.334);
	EXPECT_EQ(obj_["l"_d].value_or(0.0), 1.334);
}

TEST_F(ArrayTest, CheckArray){
	EXPECT_EQ(arr_.size(), 9);

	using namespace jason::literals;
	EXPECT_EQ(arr_[0_u].value_or(0), 1);
	EXPECT_EQ(arr_[1_i].value_or(0), -1);
	EXPECT_STREQ(arr_[3_s].value_or("error"), "string");
	EXPECT_TRUE(jason::is_null(arr_[4]));
	EXPECT_EQ(arr_[5_d].value_or(0.0), 7.4);
	EXPECT_TRUE(jason::is_array(arr_[7]));
	EXPECT_TRUE(jason::is_object(arr_[8]));
}

TEST_F(ArrayTest, ArrayAddData){
	EXPECT_EQ(arr_.size(), 9);
	arr_ << 1 << "teste" << 1.1;
	EXPECT_EQ(arr_.size(), 12);
	arr_.push_back(30, -32, 1.23, "teste");
	EXPECT_EQ(arr_.size(), 16);
	std::vector<int> vec{1,2,3,4,5};

	arr_.push(vec);
	EXPECT_EQ(arr_.size(), 21);
	arr_.push(vec.begin(), vec.end());
	EXPECT_EQ(arr_.size(), 26);
}

