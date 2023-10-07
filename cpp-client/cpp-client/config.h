#pragma once
#include <cstring>

namespace client::config
{


	const uint8_t client_version{3};
	char client_id[16]{};
	const char transfer_info_file_name[100]{ "transfer.info" };
	const char me_info_file_name[100]{ "me.info" };
	std::string name;

	void set_client_id(const char* new_client_id)
	{
		strncpy_s(client_id, new_client_id, sizeof(client_id));
		//also copy to file
	}
}