#include "jason.hpp"

namespace Jason{

bool parse(document& doc, const char* str) noexcept
{
	return doc.Parse(str).HasParseError();
}

bool parse(document& doc, const char* str, std::size_t size) noexcept
{
	return doc.Parse(str, size).HasParseError();
}

bool parse(document& doc, std::string const& str) noexcept
{
	return doc.Parse(str.data(), str.size()).HasParseError();
}

}//Jason
