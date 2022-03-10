#include "jason.hpp"

#include <iostream>

int main()
{
	using namespace jason;
	object_doc doc;

	doc.set("key", 1);

	std::cout << stringify(doc) << "\n";

	return EXIT_SUCCESS;
}
