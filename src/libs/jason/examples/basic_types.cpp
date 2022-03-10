#include "jason.hpp"

#include <string_view>
#include <iostream>

int main()
{
	std::string_view json_str{"{\"a\":1,\"b\":\"teste\",\"c\":null,\"d\":1.1,\"e\":-1,\"f\":[1,2,3],\"g\":{\"h\":1}}"};

	using namespace jason;

	document doc;
	if(!parse(doc, json_str.data(), json_str.size())
			&& !jason::is_object(doc))
	{
		std::cerr << "Error parsing data... Quitting!\n";
		return EXIT_FAILURE;
	}

	object_t data{std::move(doc)};

	/**
	 * Checking keys
	 */
	if(!data.has("a") || !data.has("b") || !data.has("c")
		|| !data.has("d") || !data.has("e") || !data.has("f")
		|| !data.has("g"))
	{
		std::cerr << "Missing field! Quitting!\n";
		return EXIT_FAILURE;
	}

	/**
	 * Checking types
	 */
	if(!is_uint(data["a"]) || !is_string(data["b"]) || !is_null(data["c"])
		|| !is_double(data["d"]) || !is_int(data["e"]) || !is_array(data["f"])
		|| !is_object(data["g"]))
	{
		std::cerr << "Wrong type! Quitting!\n";
		return EXIT_FAILURE;
	}

	/**
	 * As now we know data keys a-g exits, and there types, we can safely use
	 * the following functions.
	 *
	 * Differently from the jason::get_<type> functions, this doesn't return a
	 * optional, but the type.
	 */
	auto a = unsigned_t::get(data["a"]);	//a is unsigned
	auto b = string_t::get(data["b"]);		//b is const char*
	auto d = double_t::get(data["d"]);		//d is double
	auto e = integer_t::get(data["e"]);		//e is int
	auto f = array_t::get(data["f"]);		//f is array_ref_t
	auto g = object_t::get(data["g"]);		//g is object_ref_t

	std::cout << "a: " << a << "\nb: " << b << "\nc: null"
			<< "\nd: " << d << "\ne: " << e << "\n";

	std::cout << "f[size=" << f.size() << "]: [";
	for(auto const& n : f)
	{
		if(is_int(n)) std::cout << integer_t::get(n) << ", ";
	}
	std::cout << "]\n";

	std::cout << "g: {";
	for(auto const& n : g)
	{
		std::cout << string_t::get(n.name) << ": ";
		if(is_int(n.value)) std::cout << integer_t::get(n.value);
	}
	std::cout << "}\n";

	return EXIT_SUCCESS;
}
