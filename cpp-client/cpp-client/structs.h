#pragma once
#include <string>	//try to remove the string
#include <vector>
#include <variant>

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



	template <class T1, class T2>
	bool is_the_same()
	{
		//return typeid(T1) == typeid(T2);
		return std::is_same<T1, T2>::value;
	}




	// Request			all request codes
#pragma pack(push, 1)	//Request structs

	enum request_code : uint16_t
	{
		registration = 1025,
		sending_public_key = 1026,
		reconnect_request = 1027,
		file_transfer = 1028,
		crc_request = 1029,
		crc_wrorng_resending = 1030,
		crc_wrorng_forth_time_stop = 1031,
	};

	template <class Request_Class>
	struct request_payload : Request_Class { };

	struct request1025 { char name[255]{}; };
	struct request1026
	{
		char name[255]{};
		char public_key[160]{};
	};
	struct request1027 { char name[255]{}; };
	struct request1028
	{
		uint32_t content_size{};
		std::vector<char> message_content{};
	};
	struct request1029 { char file_name[255]{}; };
	struct request1030 { char file_name[255]{}; };
	struct request1031 { char file_name[255]{}; };



	// Define the updated header structure


	template <class Request_Class>
 // Ensure no padding is added to the struct
	struct reques_header
	{
		char client_id[16]{};
		const uint8_t version{ client::config::client_version };
		const uint16_t code{ htons(
			is_the_same<request1025, Request_Class>() ? registration
			: is_the_same<request1026, Request_Class>() ? sending_public_key
			: is_the_same<request1027, Request_Class>() ? reconnect_request
			: is_the_same<request1028, Request_Class>() ? file_transfer
			: is_the_same<request1029, Request_Class>() ? crc_request
			: is_the_same<request1030, Request_Class>() ? crc_wrorng_resending
			: is_the_same<request1031, Request_Class>() ? crc_wrorng_forth_time_stop
			: 0
		)};
		uint32_t payload_size{};
	};

	template <class Request_Class>
	struct request
	{
		reques_header<Request_Class> header{};
		request_payload<Request_Class> payload{};
	};
















	enum response_code : short int
	{
		register_success = 2100,
		register_fail = 2101,
		public_key_received_sending_aes = 2102,
		file_received_successfully_with_crc = 2103,
		approval_message_receiving = 2104,	//response to request  1029 / 1031
		approval_reconnection_request_send_crypted_aes = 2105,
		denined_reconnection_request_client_should_register_again = 2106,	//the client not registered or without public key
		global_server_error = 2107
	};




	// Response			all response codes

	template <class Response_Class>
	struct response_payload : Response_Class { };

	struct response2100 { char client_id[16]{}; };
	struct response2101 { };
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
	struct response2104 { char client_id[16]{}; };
	struct response2105 { char client_id[16]{}; };
	struct response2106 { char client_id[16]{}; };
	struct response2107 { };
	struct responseError { };


	struct response_header
	{
		uint8_t version{};
		uint16_t code{};
		uint32_t payload_size{};
	};




	struct response
	{
		response_header header{};
		std::vector<char> payload{};
		//response_payload<Response_Class> payload{};
	};

	//using Response = std::variant<response<responseError>, response<response2100>, response<response2101>, response<response2102>, response<response2103>, response<response2104>, response<response2105>, response<response2106>, response<response2107>>;





#pragma pack(pop) //Request structs end
}