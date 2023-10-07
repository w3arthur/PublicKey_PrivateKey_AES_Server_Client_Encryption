#pragma once
#include <cstring>
#include <vector>

namespace client::config
{


	const uint8_t client_version{ 3 };
	char client_id[16]{};
	constexpr int security_asymmetric_key_size{ 1024 };
	constexpr int security_symmetric_key_size{ 128 };
	std::vector<char> private_key(security_asymmetric_key_size);
	const char transfer_info_file_name[100]{ "transfer.info" };
	const char me_info_file_name[100]{ "me.info" };
	std::string name;

	void set_client_id(const char* new_client_id)
	{
		strncpy_s(client_id, new_client_id, sizeof(client_id));
		//also copy to file
	}
}