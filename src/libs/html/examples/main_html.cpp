#include "html/writer.hpp"
#include <iostream>
#include <fstream>

#define FILE_NAME "test.html"

int main()
{
	std::cout << "HTML Test\n";

	std::ofstream osf;
	std::filebuf * fb = osf.rdbuf();

	fb->open (FILE_NAME,std::ios::out);
	if(!fb->is_open())
	{
		std::cerr << "Error opening file\n";
		return 1;
	}

	std::cout << "Opened file\n";

	HTML::writer os{fb};
	const std::string t{"teste\"ter'sd"};

	std::cout << "Writing to file...\n";
	os.init()
		.hrow(std::string{"co>l1"}, "co<l2", "col3")
		.column(1)
		.column(2.3)
		.column('a')
		.nl()
		.row(1, 2, 2.3)
		.row(5, 7, 9)
		.row(t, 2, 3.3)
		.column(std::string{"t\"es,te"}, 1)
		.column("te<ste2")
		.column(HTML::io::break_line, 1, "teste3", HTML::io::break_line)
		.end();

	std::cout << "Closing file...\n";
	fb->close();

	return 0;
}
