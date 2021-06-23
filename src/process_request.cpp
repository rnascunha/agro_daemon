#include "process_request.hpp"
#include "coap_engine.hpp"
#include "rapidjson/document.h"

void process_request(engine&, std::string&& data) noexcept
{
	rapidjson::Document d;

	if (d.Parse(data.c_str()).HasParseError() || !d.IsObject())
	{
		//Not valid message;
		return;
	}


}
