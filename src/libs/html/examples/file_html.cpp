#include "html/fwriter.hpp"
#include <iostream>

#define FILE_NAME "file2.html"

int main()
{
	std::cout << "HTML File Test\n";

	HTML::fwriter os(FILE_NAME);

	if(!os.is_open())
	{
		std::cerr << "Error opening file\n";
		return 1;
	}

	std::cout << "Opened file\n";

	std::cout << "Writing to file...\n";
	os.init()
		.hrow("col1", std::string{"col2"}, "col3")
		.column(1)
		.column(2.3)
		.column('a')
		.nl()
		.row(1, 2, 2.3)
		.row(5, 7, 9)
		.column("t\"es,te")
		.column("teste2")
		.column(HTML::io::break_line, 1, "teste3", HTML::io::break_line);

	std::cout << "Closing file...\n";

	os.close();

	return 0;
}
