#include "csv.hpp"
#include <iostream>
#include <fstream>

#define FILE_NAME "test.txt"

int main()
{
	std::cout << "CSV Test\n";

	std::ofstream osf;
	std::filebuf * fb = osf.rdbuf();

	fb->open (FILE_NAME,std::ios::out);
	if(!fb->is_open())
	{
		std::cerr << "Error opening file\n";
		return 1;
	}

	std::cout << "Opened file\n";

	CSV::writer os{fb};

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
	fb->close();

	return 0;
}
