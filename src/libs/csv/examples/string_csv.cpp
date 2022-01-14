#include "csv/swriter.hpp"
#include <iostream>

int main()
{
	std::cout << "CSV String Test\n";

	CSV::swriter os;

	std::cout << "Writing to string stream...\n";
	os.row("col1", "col2", "col3")
		.column(1)
		.column(2.3)
		.column('a')
		.nl()
		.row(1, 2, 2.3)
		.row(5, 7, 9)
		.column("t\"es,te")
		.column("teste2")
		.column(CSV::io::break_line, 1, "teste3", CSV::io::break_line);

	std::cout << "Printing string...\n\n";

	std::cout << os.str();

	return 0;
}
