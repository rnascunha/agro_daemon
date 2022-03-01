#include "jason.hpp"
#include <iostream>

constexpr const char* str = "[1,-1,3,\"string\",7.4,-589,[1, \"teset\", 23, 2.3],{\"a\":1}]";

int main()
{
	jason::document base;
	if(jason::parse(base, str) || !jason::is_array(base))
	{
		std::cerr << "Error parsing...\n";
		return 1;
	}

	std::cout << "Parsed...\n";

	jason::array_t doc{std::move(base)};

	using namespace jason::literals;
	std::cout << "i: " << doc[0_u].value_or(0) << "\n";
	std::cout << "u: " << doc[1_i].value_or(-10) << "\n";
	std::cout << "s: " << doc[3_s].value_or("not a string") << "\n";
	std::cout << "d: " << doc[4_d].value_or(0.0) << "\n";

	auto arr = jason::get_array(doc[6]); //doc[6_a]
	if(arr)
	{
		for(auto  const& a : *arr)
		{
			if(jason::is_int(a))
			{
				std::cout << "arri: " << *jason::get_int(a) << "\n";
				continue;
			}
			if(jason::is_string(a))
			{
				std::cout << "arrs: " << *jason::get_string(a) << "\n";
				continue;
			}
			if(jason::is_double(a))
			{
				std::cout << "arrd: " << *jason::get_double(a) << "\n";
				continue;
			}
		}
	}

	auto obj = doc[7_o];
	if(obj)
	{
		std::cout << "obj: " << jason::stringify<true /* pretty output */>(*obj) << "\n";
	}
}
