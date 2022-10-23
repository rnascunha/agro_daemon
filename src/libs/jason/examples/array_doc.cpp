#include "jason.hpp"

#include <iostream>

int main()
{
	using namespace jason;
	array_doc doc;

	doc.push_back(1, "teste", 1.3);
	doc << "jason" << -13 << "44";

	std::cout << stringify(doc) << "\n";

	return EXIT_SUCCESS;
}
