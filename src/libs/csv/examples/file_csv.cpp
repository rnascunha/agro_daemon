#include "fcsv.hpp"
#include <iostream>
#include <fstream>

#define FILE_NAME "file.txt"

int main()
{
	std::cout << "CSV File Test\n";

	CSV::fwriter os(FILE_NAME);

	if(!os.is_open())
	{
		std::cerr << "Error opening file\n";
		return 1;
	}

	std::cout << "Opened file\n";

	std::cout << "Writing to file...\n";
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

	std::cout << "Closing file...\n";

	os.close();

	return 0;
}
