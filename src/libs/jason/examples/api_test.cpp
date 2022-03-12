#include "jason.hpp"
#include <iostream>

#include <optional>
#include <string>

constexpr const char* str = "{\"a\":1,\"b\":\"teste\",\"c\":null,\"d\":1.1,\"e\":-1,\"f\":[1,2,3],\"g\":{\"h\":1}}";

int main()
{
	jason::document base;

	if(!jason::parse(base, str) || !jason::is_object(base))
	{
		std::cerr << "Error parsing...\n";
		return 1;
	}

	std::cout << "Parsed...\n";

	jason::object_t doc{std::move(base)};

	using namespace jason::literals;
	std::cout << std::boolalpha;

	//Verify all true
	std::cout << "\nAll should succeded:\n";
	std::cout << "au: " << jason::verify(doc, "a"_u) << "\n";
	std::cout << "ai: " << jason::verify(doc, "a"_i) << "\n";
	std::cout << "an: " << jason::verify(doc, "a"_n) << "\n";
	std::cout << "b : " << jason::verify(doc, "b"_s) << "\n";
	std::cout << "c : " << jason::verify(doc, "c"_null) << "\n";
	std::cout << "df: " << jason::verify(doc, "d"_f) << "\n";
	std::cout << "dd: " << jason::verify(doc, "d"_d) << "\n";
	std::cout << "dn: " << jason::verify(doc, "d"_n) << "\n";
	std::cout << "ei: " << jason::verify(doc, "e"_i) << "\n";
	std::cout << "en: " << jason::verify(doc, "e"_n) << "\n";
	std::cout << "f : " << jason::verify(doc, "f"_a) << "\n";
	std::cout << "g : " << jason::verify(doc, "g"_o) << "\n";

	std::cout << "\nAll should fail:\n";
	std::cout << "aa: " << jason::verify(doc, "a"_a) << "\n";
	std::cout << "af: " << jason::verify(doc, "a"_f) << "\n";
	std::cout << "ad: " << jason::verify(doc, "a"_d) << "\n";
	std::cout << "bo: " << jason::verify(doc, "b"_o) << "\n";
	std::cout << "ci: " << jason::verify(doc, "c"_i) << "\n";
	std::cout << "di: " << jason::verify(doc, "d"_i) << "\n";
	std::cout << "du: " << jason::verify(doc, "d"_u) << "\n";
	std::cout << "do: " << jason::verify(doc, "d"_o) << "\n";
	std::cout << "eu: " << jason::verify(doc, "e"_u) << "\n";
	std::cout << "fo: " << jason::verify(doc, "f"_o) << "\n";
	std::cout << "ga: " << jason::verify(doc, "g"_a) << "\n";

	std::cout << "\nVerifying all at same time (should succeded):\n";
	std::cout << "All fields: " << verify(doc, "a"_u, "b"_s, "c"_null,
												"d"_d, "e"_i, "f"_a, "g"_o) << "\n";

	std::cout << "\nVerifying all at same time (should fail):\n";
	std::cout << "All fields: " << verify(doc, "a"_u, "b"_o /* wrong */, "c"_null,
												"d"_d, "e"_i, "f"_a, "g"_o) << "\n";

	std::cout << "\nVerifying using operator 'and (&&)' (should succeded):\n";
	std::cout << "au: " << (doc && "a"_u) << "\n";

	std::cout << "\nVerifying using operator 'and (&&)' (should fail):\n";
	std::cout << "aa: " << (doc && "a"_a) << "\n";

	std::cout << "\nPrint values:\n";
	{
		auto v = jason::get<jason::string_t>(doc["b"]);
		std::cout << "b-s: " << (v ? *v : "no value") << "\n";
	}
	{
		auto v = jason::get_string(doc["b"]);
		std::cout << "b-s: " << (v ? *v : "no value") << "\n";
	}
	{
		auto v = jason::get_int(doc["a"]);

		std::cout << "a-i: ";
		if(v) std::cout << *v;
		else std::cout << "no value";
		std::cout << "\n";
	}
	{
		auto v = jason::get(doc, "a"_i);

		std::cout << "a-i: ";
		if(v) std::cout << *v;
		else std::cout << "no value";
		std::cout << "\n";
	}
	{
		auto v = jason::get_double(doc["d"]);

		std::cout << "d-d: ";
		if(v) std::cout << *v;
		else std::cout << "no value";
		std::cout << "\n";
	}
	{
		auto v = jason::get(doc, "e"_u);

		std::cout << "e-u: ";
		if(v) std::cout << *v;
		else std::cout << "no value";
		std::cout << "\n";
	}
	{
		auto const& v = jason::get_array(doc["f"]);

		std::cout << "f-a: ";
		if(v)
		{
			std::cout << "[";
			for(auto const& a : *v)
			{
				auto e = jason::get_int(a);
				if(e) std::cout << *e << ", ";
			}
			std::cout << "]";
		}
		else std::cout << "no value";
		std::cout << "\n";
	}
	{
		auto const& v = jason::get(doc, "g"_o);

		std::cout << "g-o: ";
		if(v)
		{
			std::cout << "doc[g][h]: ";
			auto const& t = jason::get_int((*v)["h"]);//jason::get(*v, "h"_i);
			if(t)
			{
				std::cout << *t;
			}
			else
			{
				std::cout << "no value";
			}
		}
		else std::cout << "no value";
		std::cout << "\n";
	}

	/**
	 * Array access
	 */
	auto const& v = jason::get_array(doc["f"]);
	if(v)
	{
		std::cout << "\nAccessing array[" << (*v).size() << "]:\n";
		std::cout << (*v)[0_i].value_or(-1) << " | ";
		std::cout << (*v)[0_s].value_or("no a string") << " | ";
		std::cout << (*v)[1_u].value_or(-2) << " | ";
		std::cout << (*v)[2_i].value_or(-3) << " | ";
		std::cout << (*v)[3_s].value_or("out of bound") << "\n";
	}

	/**
	 * Creating array
	 */
	std::cout << "\nCreating and retriving array:\n";
//	jason::array_t narr{doc};
	jason::array_t narr{doc.allocator()};
	narr << 1 << 3 << -1 << 1.3 << "arr_str";
	narr.push_back(11);
	narr.push_back("1", 913, 13, -1);
	std::vector<unsigned> vec{100, 99, 98, 97, 96}, vec2{95, 94, 93, 92};
	narr.push(vec.begin(), vec.end());
	narr.push(vec2);

	for(auto const& a : narr)
	{
		if(jason::is_int(a))
		{
			auto i = jason::get_int(a);
			std::cout << "i: " << *i << " | ";
			continue;
		}
		if(jason::is_double(a))
		{
			auto i = jason::get_double(a);
			std::cout << "d: " << *i << " | ";
			continue;
		}
		if(jason::is_string(a))
		{
			auto i = jason::get_string(a);
			std::cout << "s: " << *i << " | ";
			continue;
		}
	}
	std::cout << "\n";


	/**
	 * Setting
	 */
	doc.set("i", "teste");
	doc.set("j", "teste1", 6);
	doc.set("k", std::string{"teste2"});
	doc.set("l", 10);
	doc.set("m", -13);
	doc.set("n", 1.34);

	jason::object_t obj{doc.allocator()};

	doc.set("aa", 1);
	doc.set("bb", -1.33);
	doc.set("cc", "obj_in_teste");

	doc.set("o", obj);

	jason::array_t arr{doc.allocator()};

	arr.push_back(1);
	arr.push_back("My array");
	arr.push_back(3);

	std::cout << "\nFor loop array[" << arr.size() << "]\n";
	for(auto const& a : arr)
	{
		if(jason::is_int(a))
			std::cout << "i: " << jason::get_int(a).value_or(0) << "\n";
		if(jason::is_string(a))
			std::cout << "s: " << jason::get_string(a).value_or("no value") << "\n";
	}

	doc.set("p", arr);
	doc.set("q", 10);
	doc.set("r", "jjj");

	jason::array_t arr2{doc.allocator()};
	arr2 << "1" << 2 << 1.3;
	doc.set("s", arr2);

	std::cout << "\nIterate object:\n";
	for(auto const& m : doc)
	{
		std::cout << *jason::get_string(m.name) << ": ";
		if(jason::is_int(m.value))
		{
			std::cout << *jason::get_int(m.value);
		}
		else if(jason::is_string(m.value))
		{
			std::cout << *jason::get_string(m.value);
		}
		else if(jason::is_double(m.value))
		{
			std::cout << *jason::get_double(m.value);
		}
		else std::cout << "other type";
		std::cout << "\n";
	}

	std::cout << "\nStringifying:\n";
	std::cout << jason::stringify<true /* pretty outpyt */ >(doc) << "\n";

	return 0;
}
