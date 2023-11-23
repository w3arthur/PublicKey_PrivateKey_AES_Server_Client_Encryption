#pragma once
namespace client
{

	enum class request_code : uint16_t
	{
		request_error = 0,
		registration = 1025,
		sending_public_key = 1026,
		reconnect_request = 1027,
		file_transfer = 1028,
		crc_request = 1029,
		crc_wrong_resending = 1030,
		crc_wrong_forth_time_stop = 1031,
	};

	enum class response_code : uint16_t
	{
		register_success = 2100,
		register_fail = 2101,
		public_key_received_sending_aes = 2102,
		file_received_successfully_with_crc = 2103,
		approval_message_receiving = 2104,	//response to request  1029 / 1031
		approval_reconnection_request_send_crypted_aes = 2105,
		denied_reconnection_request_client_should_register_again = 2106,	//the client not registered or without public key
		global_server_error = 2107,
	};

	inline void set_response_code(response_code& m_response_code,  const uint16_t response_code_value)
	{
		switch (ntohs(response_code_value))
		{
		case 2100: m_response_code = response_code::register_success; break;
		case 2101: m_response_code = response_code::register_fail; break;
		case 2102: m_response_code = response_code::public_key_received_sending_aes; break;
		case 2103: m_response_code = response_code::file_received_successfully_with_crc; break;
		case 2104: m_response_code = response_code::approval_message_receiving; break;
		case 2105: m_response_code = response_code::approval_reconnection_request_send_crypted_aes; break;
		case 2106: m_response_code = response_code::denied_reconnection_request_client_should_register_again; break;
		case 2107: /*fall through+*/
		default:  m_response_code =  response_code::global_server_error;
		}
	}

}