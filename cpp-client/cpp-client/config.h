#pragma once
#include <cstring>
#include <vector>
#include <string>

namespace client
{
	//TODO: set sizes
	struct _transfer
	{
		//bool is_available;
		std::string ip_address{};
		unsigned short int port{};
		std::string name{};
		std::string file_address{};
	} transfer;

	struct _identyfier
	{
		bool is_available{};
		std::string name{};
		std::string id{};	//unique//ascii hex
		std::string private_key{};
	} ;

	namespace config
	{
		client::_identyfier identyfier;
		client::_transfer transfer;
		std::string name;

		const uint8_t client_version{ 3 };
		char client_id[16]{};
		constexpr int security_asymmetric_key_size{ 1024 };
		constexpr int security_symmetric_key_size{ 128 };
		std::vector<unsigned char> private_key{};
		const char transfer_info_file_name[100]{ "transfer.info" };
		const char me_info_file_name[100]{ "me.info" };


		void set_client_id(const char* new_client_id)
		{
			strncpy_s(client_id, new_client_id, sizeof(client_id));
			//also copy to file
		}
	}
}