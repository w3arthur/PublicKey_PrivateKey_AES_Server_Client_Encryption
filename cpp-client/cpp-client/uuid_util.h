#pragma once
#include <cstdio>
#include <sstream>
#include <random>
#include <iomanip>




namespace client
{


	inline std::string convert_uuid_bytes_to_string(const char* uuidBytes)
	{
		std::stringstream ss;
		for (int i = 0; i < 16; i++) {
			ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(static_cast<unsigned char>(uuidBytes[i]));
			if (i == 3 || i == 5 || i == 7 || i == 9)
			{
				ss << "-";
			}
		}

		return ss.str();
	}



	inline bool convert_uuid_string_to_bytes(const std::string& uuidString, char* uuidBytes)
	{
		if (uuidString.size() != 36) 
		{
			return false; // Invalid UUID string format
		}

		// Create a format string to parse the UUID string
		static const char* format = "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx";

		// Use sscanf to parse the UUID string
		if (sscanf_s(uuidString.c_str(), format,
			&uuidBytes[0], &uuidBytes[1], &uuidBytes[2], &uuidBytes[3],
			&uuidBytes[4], &uuidBytes[5], &uuidBytes[6], &uuidBytes[7],
			&uuidBytes[8], &uuidBytes[9], &uuidBytes[10], &uuidBytes[11],
			&uuidBytes[12], &uuidBytes[13], &uuidBytes[14], &uuidBytes[15]) != 16) 
		{
			return false; // Failed to parse the UUID string
		}

		return true; // Successfully converted the UUID string to bytes
	}


	inline void generate_uuid_bytes(char* uuidBytes)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<unsigned int> dis(0, 255);

		for (int i = 0; i < 16; i++)
		{
			uuidBytes[i] = static_cast<char>(dis(gen));
		}

		// Set the UUID version (e.g., version 4)
		uuidBytes[6] = (uuidBytes[6] & 0x0F) | 0x40;

		// Set the UUID variant (e.g., variant 2)
		uuidBytes[8] = (uuidBytes[8] & 0x3F) | 0x80;
	}

}
