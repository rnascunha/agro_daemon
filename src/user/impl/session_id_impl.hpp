#ifndef AGRO_DAEMON_USER_SESSION_ID_IMPL_HPP__
#define AGRO_DAEMON_USER_SESSION_ID_IMPL_HPP__

#include "../session_id.hpp"
#include <random>
#include <sstream>

namespace Agro{

//https://stackoverflow.com/a/48592524
template<unsigned Size>
std::string generate_session_id() noexcept
{
    std::random_device seeder;
    std::mt19937 rng(seeder());
    std::uniform_int_distribution<char> gen('!', '~');	//All printable range
    std::stringstream ss;
    for(unsigned i = 0; i < Size; ++i)
    {
    	ss << gen(rng);
    }

   return ss.str();
}

//https://www.boost.org/doc/libs/1_77_0/libs/random/example/password.cpp

//#include <boost/random/random_device.hpp>
//#include <boost/random/uniform_int_distribution.hpp>

//template<unsigned Size>
//std::string create_session_id()
//{
//    /*<< We first define the characters that we're going
//         to allow.  This is pretty much just the characters
//         on a standard keyboard.
//    >>*/
//    std::string chars(
//        "abcdefghijklmnopqrstuvwxyz"
//        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
//        "1234567890"
//        "!@#$%^&*()"
//        "`~-_=+[{]}\\|;:'\",<.>/? ");
//    /*<< We use __random_device as a source of entropy, since we want
//         passwords that are not predictable.
//    >>*/
//    boost::random::random_device rng;
//    /*<< Finally we select 8 random characters from the
//         string and print them to cout.
//    >>*/
//    boost::random::uniform_int_distribution<> index_dist(0, chars.size() - 1);
//    std::stringstream ss;
//    for(unsigned i = 0; i < Size; ++i)
//    {
//        ss << chars[index_dist(rng)];
//    }
//    return ss.str();
//}

}//Agro

#endif /* AGRO_DAEMON_USER_SESSION_ID_IMPL_HPP__ */
