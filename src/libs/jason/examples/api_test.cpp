//#include "rapidjson/document.h"
#include "jason.hpp"
#include <iostream>

#include <optional>
#include <string>

const char* str = "{\"a\":1,\"b\":\"teste\",\"c\":null,\"d\":1.1,\"e\":-1,\"f\":[1,2,3],\"g\":{\"h\":1}}";

int main()
{
	Jason::document doc;

	if(Jason::parse(doc, str) || !Jason::is_object(doc))
	{
		std::cerr << "Error parsing...\n";
		return 1;
	}

	std::cout << "Parsed...\n";

	using namespace Jason::literals;
	std::cout << std::boolalpha;

	//Verify all true
	std::cout << "\nAll should succeded:\n";
	std::cout << "au: " << Jason::verify(doc, "a"_u) << "\n";
	std::cout << "ai: " << Jason::verify(doc, "a"_i) << "\n";
	std::cout << "an: " << Jason::verify(doc, "a"_n) << "\n";
	std::cout << "b : " << Jason::verify(doc, "b"_s) << "\n";
	std::cout << "c : " << Jason::verify(doc, "c"_null) << "\n";
	std::cout << "df: " << Jason::verify(doc, "d"_f) << "\n";
	std::cout << "dd: " << Jason::verify(doc, "d"_d) << "\n";
	std::cout << "dn: " << Jason::verify(doc, "d"_n) << "\n";
	std::cout << "ei: " << Jason::verify(doc, "e"_i) << "\n";
	std::cout << "en: " << Jason::verify(doc, "e"_n) << "\n";
	std::cout << "f : " << Jason::verify(doc, "f"_a) << "\n";
	std::cout << "g : " << Jason::verify(doc, "g"_o) << "\n";

	std::cout << "\nAll should fail:\n";
	std::cout << "aa: " << Jason::verify(doc, "a"_a) << "\n";
	std::cout << "af: " << Jason::verify(doc, "a"_f) << "\n";
	std::cout << "ad: " << Jason::verify(doc, "a"_d) << "\n";
	std::cout << "bo: " << Jason::verify(doc, "b"_o) << "\n";
	std::cout << "ci: " << Jason::verify(doc, "c"_i) << "\n";
	std::cout << "di: " << Jason::verify(doc, "d"_i) << "\n";
	std::cout << "du: " << Jason::verify(doc, "d"_u) << "\n";
	std::cout << "do: " << Jason::verify(doc, "d"_o) << "\n";
	std::cout << "eu: " << Jason::verify(doc, "e"_u) << "\n";
	std::cout << "fo: " << Jason::verify(doc, "f"_o) << "\n";
	std::cout << "ga: " << Jason::verify(doc, "g"_a) << "\n";

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
		auto v = Jason::get<Jason::string_t>(doc["b"]);
		std::cout << "b-s: " << (v ? *v : "no value") << "\n";
	}
	{
		auto v = Jason::get_string(doc["b"]);
		std::cout << "b-s: " << (v ? *v : "no value") << "\n";
	}
	{
		auto v = Jason::get_int(doc["a"]);

		std::cout << "a-i: ";
		if(v) std::cout << *v;
		else std::cout << "no value";
		std::cout << "\n";
	}
	{
		auto v = Jason::get(doc, "a"_i);

		std::cout << "a-i: ";
		if(v) std::cout << *v;
		else std::cout << "no value";
		std::cout << "\n";
	}
	{
		auto v = Jason::get_double(doc["d"]);

		std::cout << "d-d: ";
		if(v) std::cout << *v;
		else std::cout << "no value";
		std::cout << "\n";
	}
	{
		auto v = Jason::get(doc, "e"_u);

		std::cout << "e-u: ";
		if(v) std::cout << *v;
		else std::cout << "no value";
		std::cout << "\n";
	}
	{
		auto const& v = Jason::get_array(doc["f"]);

		std::cout << "f-a: ";
		if(v)
		{
			std::cout << "[";
			for(auto const& a : *v)
			{
				auto e = Jason::get_int(a);
				if(e) std::cout << *e << ", ";
			}
			std::cout << "]";
		}
		else std::cout << "no value";
		std::cout << "\n";
	}
	{
		auto const& v = Jason::get(doc, "g"_o);

		std::cout << "g-o: ";
		if(v)
		{
			std::cout << "doc[g][h]: ";
			auto const& t = Jason::get_int((*v)["h"]);//Jason::get(*v, "h"_i);
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
	 * Setting
	 */
	Jason::set(doc, "i", "teste");
	Jason::set(doc, "j", "teste1", 6);
	Jason::set(doc, "k", std::string{"teste2"});
	Jason::set(doc, "l", 10);
	Jason::set(doc, "m", -13);
	Jason::set(doc, "n", 1.34);

	Jason::object_t::type obj;
	obj.SetObject();

	Jason::set(obj, "aa", 1, doc.GetAllocator());
	Jason::set(obj, "bb", -1.33, doc.GetAllocator());
	Jason::set(obj, "cc", "obj_in_teste", doc.GetAllocator());

	Jason::set(doc, "o", obj.GetObject());

	Jason::array_t arr{doc};

	Jason::push_back(arr, 1);
	Jason::push_back(arr, "My array");
	Jason::set(doc, "p", arr.get());

	std::cout << "Stringifying:\n";
	std::cout << Jason::stringify<true /* pretty outpyt */ >(doc) << "\n";

	return 0;
}
