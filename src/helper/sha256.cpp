#include "sha256.hpp"
#include <iomanip>
#include <sstream>
#include <string>

#include <filesystem>
#include <fstream>

std::string sha256_string(sha256_hash const data) noexcept
{
	std::ostringstream hash;
    for (int i = 0; i < 32; ++i)
    {
    	hash << std::hex
    				<< std::setfill('0')
    				<< std::setw(2)
    				<< (int)data[i];
    }
    return hash.str();
}

bool sha256_file(std::filesystem::path const& path, sha256_hash hash) noexcept
{
	std::ifstream ifs{path, std::ios::binary};
	if(!ifs) return false;

	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	const int buf_size = 32768;
	std::uint8_t buffer[buf_size];

	while(!ifs.eof())
	{
		ifs.read((char*)buffer, buf_size);
		SHA256_Update(&sha256, buffer, ifs.gcount());
	}
	SHA256_Final(hash, &sha256);

	return true;
}

std::size_t sha256_file(std::filesystem::path const& path, std::size_t max_bytes, sha256_hash hash) noexcept
{
	std::ifstream ifs{path, std::ios::binary};
	if(!ifs) return 0;

	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	const int buf_size = 32768;
	std::uint8_t buffer[buf_size];
	std::size_t readed = 0, temp;
	while(!ifs.eof())
	{
		ifs.read((char*)buffer, buf_size);
		temp = ifs.gcount();
		if((readed + temp) > max_bytes)
		{
			temp = max_bytes - readed;
		}

		SHA256_Update(&sha256, buffer, temp);
		readed += temp;
		if(readed >= max_bytes) break;
	}
	SHA256_Final(hash, &sha256);

	return readed;
}
