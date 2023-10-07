#pragma once
#include <string>	//try to remove the string
#include <vector>
#include <variant>
#include <typeindex>

namespace client
{

	struct transfer
	{
		//bool is_available;
		std::string url{};
		unsigned int port{};
		std::string sender_name{};
		std::string file_address{};
	};

	struct identyfier
	{
		bool is_available{};
		std::string name{};
		std::string unique{};	//ascii hex
		std::string private_key{};
	};





	// Request			all request codes
	//Request structs

	enum class request_code : uint16_t
	{
		request_error = 0,
		registration = 1025,
		sending_public_key = 1026,
		reconnect_request = 1027,
		file_transfer = 1028,
		crc_request = 1029,
		crc_wrorng_resending = 1030,
		crc_wrorng_forth_time_stop = 1031,
	};

#pragma pack(push, 1)
	template <class Request_Class>
	struct request_payload : Request_Class
	{
	};

	struct request1025 
	{
		char name[255]{};
	};
	struct request1026
	{
		char name[255]{};
		char public_key[160]{};
	};
	struct request1027 
	{
		char name[255]{}; 
	};
	struct request1028
	{
		uint32_t content_size{};
		std::vector<char> message_content{};
	};
	struct request1029 
	{
		char file_name[255]{}; 
	};
	struct request1030 
	{ 
		char file_name[255]{};
	};
	struct request1031 
	{
		char file_name[255]{};
	};


	const std::map<std::type_index, request_code> request_code_map = {
	{typeid(request1025), request_code::registration},
	{typeid(request1026), request_code::sending_public_key},
	{typeid(request1027), request_code::reconnect_request},
	{typeid(request1028), request_code::file_transfer},
	{typeid(request1029), request_code::crc_request},
	{typeid(request1030), request_code::crc_wrorng_resending},
	{typeid(request1031), request_code::crc_wrorng_forth_time_stop},
	};



	// Define the updated header structure
	template <class Request_Class>
	struct reques_header
	{
		char client_id[16]{};
		const uint8_t version{ client::config::client_version };
		const request_code code{ htons(request_code_map[typeid(Request_Class)]) };
		uint32_t payload_size{};
	};

	template <class Request_Class>
	struct request
	{
		reques_header<Request_Class> header{};
		request_payload<Request_Class> payload{};
	};
#pragma pack(pop)





	enum class response_code : short int
	{
		response_error = 0,
		register_success = 2100,
		register_fail = 2101,
		public_key_received_sending_aes = 2102,
		file_received_successfully_with_crc = 2103,
		approval_message_receiving = 2104,	//response to request  1029 / 1031
		approval_reconnection_request_send_crypted_aes = 2105,
		denined_reconnection_request_client_should_register_again = 2106,	//the client not registered or without public key
		global_server_error = 2107,
	};




	// Response			all response codes
#pragma pack(push, 1)
	template <class Response_Class>
	struct response_payload : Response_Class 
	{
	};

	struct response2100 
	{
		char client_id[16]{}; 
	};
	struct response2101 
	{
	};
	struct response2102
	{
		char client_id[16]{};
		std::string aes_key{};
	};
	struct response2103
	{
		char client_id[16]{};
		unsigned int content_size{};	//after encryption
		char file_name[255]{};
		char cksum[4]{}; //crc
	};
	struct response2104 {
		char client_id[16]{}; 
	};
	struct response2105 {
		char client_id[16]{}; 
	};
	struct response2106 
	{
		char client_id[16]{}; 
	};
	struct response2107
	{
	};
	struct responseError 
	{
	};


	struct response_header
	{
		uint8_t version{};
		response_code code{ response_code::response_error };
		uint32_t payload_size{};
	};



	struct response
	{
		response_header header{};
		std::vector<char> payload{};
		//response_payload<Response_Class> payload{};
	};

#pragma pack(pop)



}