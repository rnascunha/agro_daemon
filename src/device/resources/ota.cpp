#include <vector>
#include <filesystem>
#include <fstream>

#include "../../coap_engine.hpp"
#include "../list.hpp"
#include "../../ota/ota.hpp"

#define DEFAULT_BLOCK_SIZE		512

namespace Agro{
namespace Device{
namespace Resource{

void get_ota_handler(engine::message const& request,
					engine::response& response, void*) noexcept
{
	CoAP::Message::Option::option op;
	CoAP::Message::Option::get_option(request, op, CoAP::Message::Option::code::uri_host);

	using namespace CoAP::Message;

	std::filesystem::path p{ota_path()};
	p += "/";
	p += std::string{static_cast<const char*>(request.payload), request.payload_len};

	if(!std::filesystem::exists(p) || !std::filesystem::is_regular_file(p))
	{
		CoAP::Message::Option::node uri_host{op};
		response
			.code(code::internal_server_error)
			.add_option(uri_host)
			.payload("file not found")
			.serialize();
		return;
	}

	unsigned size = std::filesystem::file_size(p);
	/**
	 * Default block2 options, if none is sent by the client
	 */
	unsigned block_size = DEFAULT_BLOCK_SIZE,
			block_num = 0,
			offset = 0,
			left = size;
	bool more = true;

	/**
	 * Checking if block2 option is present
	 */
	Option::option b2;
	if(Option::get_option(request, b2, Option::code::block2))
	{
		unsigned value = Option::parse_unsigned(b2);

		block_size = Option::block_size(value);
		block_num = Option::block_number(value);
		offset = Option::byte_offset(value);

		/**
		 * Checking if block required is valid
		 */
		if(offset > size)
		{
			CoAP::Message::Option::node uri_host{op};
			response
				.code(code::bad_request)
				.add_option(uri_host)
				.payload("out of bound")
				.serialize();
			return;
		}
		left -= offset;
		/**
		 * Setting more flag
		 */
		if(left < block_size)
			more = false;
	}
	/**
	 * Option size2
	 */
	Option::node size2_op{Option::code::size2, size};

	/**
	 * Option block2
	 */
	unsigned block2;
	Option::make_block(block2, block_num, more, block_size);
	Option::node block2_op{Option::code::block2, block2};

	std::size_t payload_len = left > block_size ? block_size : left;

	std::ifstream ifs{p, std::ios::binary};
	if(!ifs)
	{
		CoAP::Message::Option::node uri_host{op};
		response
			.code(code::internal_server_error)
			.add_option(uri_host)
			.payload("file not found")
			.serialize();
		return;
	}

	ifs.seekg(offset, ifs.beg);
	std::vector<unsigned char> buffer(static_cast<unsigned>(payload_len));

	ifs.read((char*)buffer.data(), payload_len);
	ifs.close();
	/**
	 * Sending response
	 */
	Option::node uri_host{op};
	response
		.code(code::content)
		.add_option(size2_op)
		.add_option(block2_op)
		.add_option(uri_host)
		.payload(buffer.data(), payload_len)
		.serialize();
}

}//Resource
}//Device
}//Agro
