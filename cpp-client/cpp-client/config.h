#pragma once
#include <cstring>
#include <vector>
#include <string>


namespace client
{

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

	inline bool delete_file(const char* filename)
	{
		if (std::remove(filename) == 0)
		{
			std::cout << "File '" << filename << "' deleted successfully." << std::endl;
			return true;
		}
		else
		{
			std::cerr << "Error: Unable to delete file '" << filename << "'." << std::endl;
			return false;
		}
	}

	namespace config
	{
		uint8_t login_tries = 0, file_seneding_tries = 0;
		constexpr uint8_t max_login_tries = 3, max_file_send_tries = 3;

		inline bool is_login_attempt_available()
		{
			login_tries++;
			return login_tries <= max_login_tries;
		}

		inline bool is_file_send_attempt_available()
		{
			file_seneding_tries++;
			return file_seneding_tries <= max_file_send_tries;
		}

		client::identifier identyfier;
		client::transfer transfer;
		std::string name;

		constexpr uint8_t client_version{ 3 };
		constexpr uint8_t client_id_size = 16;
		char client_id[client_id_size]{};
		inline void set_client_id(const char* new_client_id)
		{
			std::memcpy(&client_id, new_client_id, client_id_size);
		}

		constexpr int security_asymmetric_key_size{ 1024 };
		constexpr int security_symmetric_key_size{ 128 };	//generated in the server

		constexpr char rsa_private_key_file[100] = { "priv.key" };
		constexpr char transfer_info_file_name[100]{ "transfer.info" };
		constexpr char me_info_file_name[100]{ "me.info" };

		uint32_t prv_file_content_size{};
		uint32_t prv_file_checksum{};


		void config_reset()
		{
			login_tries = 0;
			file_seneding_tries = 0;
			identyfier= {};
			transfer = {};
			name = "";
			prv_file_content_size = 0;
			prv_file_checksum = 0;
			set_client_id("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");

		}
	}
}