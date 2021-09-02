#ifndef AGRO_DAEMON_MESSAGE_BINARY_DATA_HPP__
#define AGRO_DAEMON_MESSAGE_BINARY_DATA_HPP__

#include <cstdlib>

template<typename Stream>
void get_file(Stream& stream, const char* first_block, std::size_t block_size) noexcept;

#include "impl/binary_data_impl.hpp"

#endif /* AGRO_DAEMON_MESSAGE_BINARY_DATA_HPP__ */
