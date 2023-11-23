if __name__ != "__main__":
    import threading
    import socket
    import struct
    import uuid
    import config  # as config
    import utils.classes as classes
    import utils.file_util as file_util
    import utils.sql_lite_util as sql_lite_util
    import utils.ram_data_util as ram_data_util
    import utils.encrypting_util as encrypting_util
    import utils.responses as responses
    import utils.string_util as string_util

    def start_server(url, port):
        # Function to start the server
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.bind((url, port))
        server_socket.listen(config.maximum_number_of_queued_connections)

        config.green_print(f"Server is listening on port {port}")

        while True:
            client_socket, client_address = server_socket.accept()
            client_thread = threading.Thread(
                target=handle_client_request, args=(client_socket, client_address))
            client_thread.start()

    def handle_client_request(client_socket, client_address):
        print("Accepted connection from", client_address)
        try:
            # Function to handle the client's request
            received_request_header_format: str = "<16sBHI"
            header_size = struct.calcsize(received_request_header_format)
            header_data = client_socket.recv(header_size)
            if len(header_data) == header_size:
                # Unpack the header fields
                client_id, version, code, payload_size = struct.unpack(
                    received_request_header_format, header_data)

                # update last visited date if available
                client_id_str = string_util.uuid_to_string(client_id)
                sql_lite_util.update_last_seen_by_id(client_id_str)
                ram_data_util.update_last_seen_by_id(
                    config.client_ram_data, client_id_str)
                code_handlers = {
                    1025: handle_code_1025,
                    1026: handle_code_1026,
                    1027: handle_code_1027,
                    1028: handle_code_1028,
                    1029: handle_code_1029,
                    1030: handle_code_1030,
                    1031: handle_code_1031,
                }

                if code in code_handlers:
                    code_handlers[code](
                        client_socket, payload_size, client_id)
        except Exception as ex:
            config.red_print(f"{ex}")
        finally:
            client_socket.close()

    def handle_code_1025(client_socket: object, payload_size,
                         client_id):
        config.blue_print('1025 Register request')
        # Receive the payload data based on payload_size
        payload_data = client_socket.recv(payload_size)
        name: str = string_util.get_string(payload_data, "<255s")
        if (sql_lite_util.check_client(name) or ram_data_util.check_client(config.client_ram_data, name)):  # is a client
            client_socket.send(responses.response2106(client_id))
            return
        # else:
        unique_id = uuid.uuid4()
        client_id = sql_lite_util.register_client(unique_id, name)
        # client_id =
        ram_data_util.register_client(
            config.client_ram_data, str(unique_id), name)  # store data to ram
        client_socket.send(responses.response2100(name, client_id))

    def handle_code_1026(client_socket: object, payload_size, client_id):
        config.blue_print('1026 AESKey send')
        # Receive the payload data based on payload_size
        payload_data = client_socket.recv(payload_size)
        request1026_format: str = "<255s160s"
        _name, public_key_bytes = struct.unpack(
            request1026_format, payload_data)
        name: str = string_util.get_string(_name, "255s")
        decoded_aes_key, aes_key = encrypting_util.generate_aes_key()

        client_id_str = string_util.uuid_to_string(client_id)
        sql_lite_util.update_keys_in_clients_table(
            client_id_str, public_key_bytes, aes_key)
        ram_data_util.update_keys_in_clients_table(
            config.client_ram_data, client_id_str, string_util.decode_byte(public_key_bytes), decoded_aes_key)

        encrypted_aes_key, encrypted_aes_key64 = encrypting_util.encrypt_aes_key(
            public_key_bytes, aes_key)

        client_socket.send(responses.response2102(
            client_id, encrypted_aes_key))

    def handle_code_1027(client_socket: object, payload_size, client_id):
        config.blue_print('1027 Reconnect available user and send him aes key')
        payload_data = client_socket.recv(payload_size)
        name: str = string_util.get_string(payload_data, "<255s")

        client_id_str = string_util.uuid_to_string(client_id)
        name, public_key_bytes, byte_aes_key = sql_lite_util.get_client_info_by_id(
            client_id_str)
        # name, public_key_bytes, byte_aes_key = ram_data_util.get_client_info_by_id(
        #    config.client_ram_data, client_id_str) #stored user in ram
        if name is None or not public_key_bytes or not byte_aes_key:
            config.red_print('No user in data base')
            client_socket.send(responses.response2106(client_id))
            return

        encrypted_aes_key, encrypted_aes_key64 = encrypting_util.encrypt_aes_key(
            public_key_bytes, byte_aes_key)

        client_socket.send(responses.response2102(
            client_id, encrypted_aes_key))

    def handle_code_1028(client_socket: object, payload_size, client_id):
        # Receive the payload data based on payload_size
        config.blue_print('1028 Receive file and check sum')

        client_id_str = string_util.uuid_to_string(client_id)
        name, public_key_bytes, byte_aes_key = sql_lite_util.get_client_info_by_id(
            client_id_str)
        # name, public_key_bytes, byte_aes_key = ram_data_util.get_client_info_by_id(
        #    config.client_ram_data, client_id_str)
        if name is None or not public_key_bytes or not byte_aes_key:
            config.red_print('No user in data base')
            client_socket.send(responses.response2106(client_id))
            return

        request1028_format_package_size = "<I"
        uint32_t_size = struct.calcsize(request1028_format_package_size)

        _package_size_value = client_socket.recv(uint32_t_size)

        package_size_value, = struct.unpack(
            request1028_format_package_size, _package_size_value)

        request1028_format_package_message = f"<{str(int(package_size_value))}s"
        package_message_file_size = struct.calcsize(
            request1028_format_package_message)
        _package_message_aes_file_value = client_socket.recv(
            package_message_file_size)

        package_aes_file_value, = struct.unpack(
            request1028_format_package_message, _package_message_aes_file_value)

        decrypted_file, decrypted_binary = encrypting_util.decrypt_with_aes(
            string_util.decode_byte(byte_aes_key), package_aes_file_value.decode('utf-8'))  #

        request1028_format_filename = f"<255s"
        request1028_format_filename_size = struct.calcsize(
            request1028_format_filename)
        # payload_size - uint32_t_size - package_message_file_size:

        _filename = client_socket.recv(request1028_format_filename_size)
        filename = string_util.get_string(
            _filename, request1028_format_filename)

        user_id_str = string_util.uuid_to_string(client_id)

        content_size: int = len(package_aes_file_value)
        cksum: int = encrypting_util.calculate_numeric_checksum(decrypted_file)
        path: str = f'{config.received_files_folder_name}/{user_id_str}'

        file_util.write_to_file(path, filename, decrypted_binary)

        sql_lite_util.insert_file_data(user_id_str, filename, path)
        ram_data_util.insert_file_data(
            config.file_ram_data, user_id_str, filename, path)
        client_socket.send(responses.response2103(
            client_id, content_size, filename, cksum))

    def handle_code_1029(client_socket: object, payload_size, client_id):
        config.green_print('1029 File accept, return Thanks.')

        request1029_format_filename = f"<255s"
        request1029_format_filename_size = struct.calcsize(
            request1029_format_filename)

        _filename = client_socket.recv(request1029_format_filename_size)
        filename = string_util.get_string(
            _filename, request1029_format_filename)

        user_id_str = string_util.uuid_to_string(client_id)
        sql_lite_util.update_verification_to_true(user_id_str, filename)
        ram_data_util.update_verification_to_true(
            config.file_ram_data, user_id_str, filename)
        client_socket.send(responses.response2014_accept(client_id))

    def handle_code_1030(client_socket: object, payload_size, client_id):
        config.green_print('1030 Try send CRC again, retry up to 3 times')
        pass  # applied in client

    def handle_code_1031(client_socket: object, payload_size, client_id):
        config.red_print('1031 CRC wrong for 4th time.')
        client_socket.send(responses.response2014_accept(client_id))

else:
    print('wrong page run')
