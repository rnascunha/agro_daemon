#include "html/swriter.hpp"
#include <iostream>

int main()
{
	std::cout << "HTML String Test\n";

	HTML::swriter os;

	std::cout << "Writing to string stream...\n";
	os.init()
		.row("col1", "col2", "col3")
		.column(1)
		.column(2.3)
		.column('a')
		.nl()
		.row(1, 2, 2.3)
		.row(5, 7, 9)
		.column("t\"es,te")
		.column("teste2")
		.column(HTML::io::break_line, 1, "teste3", HTML::io::break_line)
		.end();

	std::cout << "Printing string...\n\n";

	std::cout << os.str();

	return 0;
}
