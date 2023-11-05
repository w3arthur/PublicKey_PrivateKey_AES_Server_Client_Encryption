#pragma once
#include <cstring>
#include <vector>
#include <string>

namespace client
{
	//TODO: set sizes
	struct transfer
	{
		//bool is_available;
		std::string ip_address{};
		unsigned short int port{};
		std::string name{};
		std::string file_address{};
		transfer& operator=(const transfer& other)
		{
			if (this != &other) {
				ip_address = other.ip_address;
				port = other.port;
				name = other.name;
				file_address = other.file_address;
			}
			return *this;
		}

	};

	struct identifier
	{
		bool is_available{};	//false
		std::string name{};
		std::string id{};	//unique//ascii hex
		std::string private_key{};
		identifier& operator=(const identifier& other)
		{
			if (this != &other) {
				is_available = other.is_available;
				name = other.name;
				id = other.id;
				private_key = other.private_key;
			}
			return *this;
		}
	} ;

	namespace config
	{
		uint8_t login_tries = 0, file_seneding_tries = 0;
		constexpr uint8_t max_login_tries = 3, max_file_send_tries = 3;

		bool is_login_attempt_available()
		{
			login_tries++;
			return login_tries <= max_login_tries;
		}

		bool is_file_send_attempt_available()
		{
			file_seneding_tries++;
			return file_seneding_tries <= max_file_send_tries;
		}

		client::identifier identyfier;
		client::transfer transfer;
		std::string name;

		constexpr char filename[255] = "file.txt";

		constexpr uint8_t client_version{ 3 };
		constexpr uint8_t client_id_size = 16;
		char client_id[client_id_size]{};
		inline void set_client_id(const char* new_client_id)
		{
			std::memcpy(&client_id, new_client_id, client_id_size);
		}

		constexpr int security_asymmetric_key_size{ 1024 };
		constexpr int security_symmetric_key_size{ 128 };
		std::vector<unsigned char> private_key{};

		constexpr char transfer_info_file_name[100]{ "transfer.info" };
		constexpr char me_info_file_name[100]{ "me.info" };
		constexpr char read_from_file_name[100] = "file.txt";

		uint32_t file_content_size{};
		uint32_t file_checksum{};

		void config_reset()
		{
			login_tries = 0;
			file_seneding_tries = 0;
			identyfier= {};
			transfer = {};
			name = "";
			private_key.clear();
			file_content_size = 0;
			file_checksum = 0;
			set_client_id("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
			//TODO: remove me.info content
		}
	}
}