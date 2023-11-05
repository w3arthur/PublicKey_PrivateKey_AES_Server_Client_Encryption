if __name__ != "__main__":
    import socket
    import struct

    import config  # as config
    import utils.classes as classes
    import utils.sql_lite_util as sql_lite_util
    import utils.encrypting_util as encrypting_util

    # Define the updated header structure
    HEADER_SIZE = struct.calcsize(config.received_request_header_format)

    def start_server(url, port):
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.bind((url, port))
        server_socket.listen(config.maximum_number_of_queued_connections)

        config.green_print(f"Server is listening on port {port}")

        while True:
            client_socket, client_address = server_socket.accept()
            print("Accepted connection from", client_address)
            is_fail = False
            try:
                handle_client_request(client_socket)
            except Exception as ex:
                config.red_print(f"{ex}")
                is_fail = True
            client_socket.close()
            if is_fail is False:
                config.green_print("ok")

    def handle_client_request(client_socket):
        header_data = client_socket.recv(HEADER_SIZE)
        if len(header_data) == HEADER_SIZE:
            # Unpack the header fields
            client_id, version, code, payload_size = struct.unpack(
                config.received_request_header_format, header_data)

            # update last visited date if available
            client_id_str = sql_lite_util.uuid_to_string(client_id)
            sql_lite_util.update_last_seen_by_id(client_id_str)

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

        # print(f"Received: {request}")

    def null_terminator_crop(string: str) -> str:
        name_end_index: int = string.find(b'\0')
        if name_end_index != -1:
            return string[:name_end_index]
        return string

    def get_string(payload_data, request_format: str = "<255s") -> str:
        str1,  = struct.unpack(request_format, payload_data)
        return null_terminator_crop(str1).decode('utf-8')

    def handle_code_1025(client_socket: object, payload_size,
                         client_id):
        config.blue_print('1025 Register request')
        # Receive the payload data based on payload_size
        payload_data = client_socket.recv(payload_size)
        name: str = get_string(payload_data, "<255s")
        # if (sql_lite_util.check_client(name)):  # is a client
        #     code: int = 2101
        #     header = classes.Header(code, 0)
        #     header_bytes = header.serialize()
        #     message = header_bytes
        #     client_socket.send(message)
        #     return
        code: int = 2100
        client_id = sql_lite_util.register_client(name)
        payload = classes.Payload2100(client_id)
        header = classes.Header(code, payload.size())

        header_bytes = header.serialize()
        payload_bytes = payload.serialize()
        message = header_bytes + payload_bytes
        client_socket.send(message)

    def handle_code_1026(client_socket: object, payload_size, client_id):
        config.blue_print('1026 AESKey send')
        # Receive the payload data based on payload_size
        payload_data = client_socket.recv(payload_size)
        request1026_format: str = "<255s160s"
        _name, public_key_bytes = struct.unpack(
            request1026_format, payload_data)

        name: str = get_string(_name, "255s")
        encrypted_aes_key, aes_key_db = encrypting_util.generate_aes_key()
        # public_key = public_key_bytes.hex()  # continue
        client_id_str = sql_lite_util.uuid_to_string(client_id)
        sql_lite_util.update_keys_in_clients_table(
            client_id_str, public_key_bytes, aes_key_db)

        code: int = 2102
        payload = classes.Payload2102(client_id, encrypted_aes_key)
        header = classes.Header(code, payload.size())
        header_bytes = header.serialize()
        payload_bytes = payload.serialize()
        message = header_bytes + payload_bytes
        client_socket.send(message)

    def handle_code_1027(client_socket: object, payload_size, client_id):
        config.blue_print('1027 Try login again')
        pass

    def handle_code_1028(client_socket: object, payload_size, client_id):
        # Receive the payload data based on payload_size
        config.blue_print('1028 Receive file and check sum')

        client_id_str = sql_lite_util.uuid_to_string(client_id)
        name, public_key_bytes, byte_aes_key = sql_lite_util.get_client_info_by_id(
            client_id_str)
        if name is None or not public_key_bytes or not byte_aes_key:
            pass

        request1028_format_package_size = "<I"
        uint32_t_size = struct.calcsize(request1028_format_package_size)
        if uint32_t_size < payload_size:
            pass  # TODO:
        _package_size_value = client_socket.recv(uint32_t_size)

        package_size_value, = struct.unpack(
            request1028_format_package_size, _package_size_value)

        request1028_format_package = f"<{str(int(package_size_value))}s"  # <
        package_file_size = struct.calcsize(request1028_format_package)
        _package_aes_file_value = client_socket.recv(package_file_size)
        if package_file_size < payload_size - uint32_t_size:
            pass  # TODO:

        package_aes_file_value, = struct.unpack(
            request1028_format_package, _package_aes_file_value)

        decrypted_file = encrypting_util.decrypt_with_aes(
            byte_aes_key, package_aes_file_value.decode('utf-8'))

        print('file content:')
        print(decrypted_file)
        print(':end')

        content_size: int = len(package_aes_file_value)
        filename = config.filename.encode('utf-8')
        cksum: int = encrypting_util.calculate_numeric_checksum(decrypted_file)

        code: int = 2103
        payload = classes.Payload2103(client_id, content_size, filename, cksum)
        header = classes.Header(code, payload.size())
        header_bytes = header.serialize()
        payload_bytes = payload.serialize()
        message = header_bytes + payload_bytes
        client_socket.send(message)

    def handle_code_1029(client_socket: object, payload_size, client_id):
        config.blue_print('1029 Try send CRC again')
        pass

    def handle_code_1030(client_socket: object, payload_size, client_id):
        config.blue_print('1030 File accept, return Thanks.')
        pass

    def handle_code_1031(client_socket: object, payload_size, client_id):
        config.blue_print('1031 CRC wrong for 4th time.')
        pass
