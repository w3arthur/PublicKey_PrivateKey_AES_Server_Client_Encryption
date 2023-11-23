#pragma once
#include <cstdio>
#include <sstream>
#include <random>
#include <iomanip>
#include <cstring>

#include <iostream>

namespace client
{


	inline std::string convert_uuid_bytes_to_string(const char* uuid_bytes)
	{
		std::stringstream ss;
		for (int i = 0; i < 16; i++) {
			ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(static_cast<unsigned char>(uuid_bytes[i]));
			if (i == 3 || i == 5 || i == 7 || i == 9)
			{
				ss << "-";
			}
		}

		return ss.str();
	}



	inline void generate_uuid_bytes(char* uuid_bytes)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<unsigned int> dis(0, 255);

		for (int i = 0; i < 16; i++)
		{
			uuid_bytes[i] = static_cast<char>(dis(gen));
		}

		// Set the UUID version (e.g., version 4)
		uuid_bytes[6] = (uuid_bytes[6] & 0x0F) | 0x40;

		// Set the UUID variant (e.g., variant 2)
		uuid_bytes[8] = (uuid_bytes[8] & 0x3F) | 0x80;
	}

}
