#pragma once
#include <string>
#include <map>
#include <vector>
#include <string>
#include <variant>
#include <typeindex>
#include <winsock2.h>
#include <cstring>
#include "config.h"
#include "enums.h"

namespace client
{
#pragma pack(push, 1)


	//request

	template <class Request_Class>
	struct request_header;

	template <class Request_Class>
	struct request_payload : Request_Class
	{
	};

	template <class Request_Class>
	struct request
	{
		request_header<Request_Class> header{};
		request_payload<Request_Class> payload{};
		std::string serialized_payload_data{};
	};

	struct request1025 
	{
		void set_name(const std::string& name)
		{
			strncpy_s(this->name, name.c_str(), sizeof(this->name));
			this->name[name.size()] = '\0';
		}
		char name[255]{};
	};

	struct request1026 : request1025
	{
		void set_public_key(const  unsigned char* _public_key)
		{
			memcpy_s(this->public_key, sizeof(this->public_key), _public_key, sizeof(this->public_key));
		}
		//char name[255]{};
		unsigned char public_key[160]{};
	}; 

	struct request1027 : request1025
	{
		//char name[255]{}; 
	}; 


	struct filename
	{
		void set_file_name(const std::string& name)
		{
			strncpy_s(this->file_name, name.c_str(), sizeof(this->file_name));
			this->file_name[name.size()] = '\0';
		}
		char file_name[255]{};
	};

	struct request1028 : filename
	{
		//char file_name[255]{};
		uint32_t content_size{};
		std::string message_content;
	};

	struct request1029 : filename
	{
		// char file_name[255]{};
	};	

	struct request1030 : filename
	{
		//char file_name[255]{};
	}; 

	struct request1031 : filename
	{
		//char file_name[255]{};
	}; 


	const std::map<std::type_index, request_code> request_code_map = {
	{typeid(request1025), request_code::registration},
	{typeid(request1026), request_code::sending_public_key},
	{typeid(request1027), request_code::reconnect_request},
	{typeid(request1028), request_code::file_transfer},
	{typeid(request1029), request_code::crc_request},
	{typeid(request1030), request_code::crc_wrong_resending},
	{typeid(request1031), request_code::crc_wrong_forth_time_stop},
	};

	template <class Request_Class>
	struct request_header
	{
		request_header()
		{
			memcpy(&this->client_id, &config::client_id, sizeof(this->client_id));
		}
		char client_id[config::client_id_size]{};	//[16]
		const uint8_t version{ config::client_version };
		const uint16_t code{ static_cast<uint16_t>(request_code_map.at(typeid(Request_Class))) };
		uint32_t payload_size{};
	};




	// response

	struct response_header
	{
		response_header() = default;
		response_header(const std::vector<char>& data)
		{
			std::memcpy(this, data.data(), sizeof(this));
			this->payload_size = ntohl(this->payload_size);
		}
		uint8_t version{};
		short int response_code_value{};//0
		uint32_t payload_size{};

	};

	struct response
	{
		response() = default;
		response(const std::vector<char>& data) : header(data), payload()
		{
			payload.reserve(header.payload_size);
			set_response_code(header.response_code_value);
		}
		response_code get_response_code() const
		{
			return m_response_code;
		}

		void set_response_code(uint16_t response_code_value)
		{
			client::set_response_code(m_response_code, response_code_value);
		}

		void set_response_code(response_code response_code)
		{
			m_response_code = response_code;
		}


		response_header header{};
		std::vector<char> payload{};

	private:

		response_code m_response_code{ response_code::global_server_error };
	};



	template <class Response_Class>
	struct response_payload : Response_Class 
	{
	};

	struct response2100	// register_success
	{
		char client_id[config::client_id_size]{}; //[16]
	};

	struct response2101 // register_fail
	{
	};

	struct response2102 : response2100	// public_key_received_sending_aes
	{ 
	};//char client_id[16]{}; // in addition set std::string aes_key;

	struct response2103// : response2100	// file_received_successfully_with_crc
	{
		char client_id[16]{};
		uint32_t content_size{};	//after encryption
		char file_name[255]{};
		uint32_t cksum{}; //crc
	};

	struct response2104 : response2100	// approval_message_receiving
	{
	}; //char client_id[16]{}; 

	struct response2105 : response2100	// approval_reconnection_request_send_crypted_aes
	{
	}; //char client_id[16]{}; 

	struct response2106 : response2100 // denied_reconnection_request_client_should_register_again
	{
	}; //char client_id[16]{}; 

	struct response2107
	{
	}; // Error


	inline void serialize_request1028(request<request1028>& req, uint32_t& serialized_request_size)
	{
		req.payload.content_size = static_cast<uint32_t>(req.payload.message_content.size());
		req.serialized_payload_data.append(std::string(reinterpret_cast<char*>(&req.payload.content_size), sizeof(req.payload.content_size)));
		req.serialized_payload_data.append(req.payload.message_content);
		req.serialized_payload_data.append(req.payload.file_name, sizeof(req.payload.file_name));
		serialized_request_size = req.payload.content_size;
	}


#pragma pack(pop)
}