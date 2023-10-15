if __name__ != "__main__":
    import socket
    import struct
    import hashlib
    import os

    import config
    from utils.classes import Header, Payload
    from utils.sql_lite_util import check_client, register_client
    from utils.encrypting_util import generate_and_encrypt_aes_key

    # Define the updated header structure
    HEADER_SIZE = struct.calcsize(config.received_request_header_format)

    def start_server(url, port):
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.bind((url, port))
        server_socket.listen(config.maximum_number_of_queued_connections)

        print("\033[92m", f"Server is listening on port {port}", "\033[0m")

        while True:
            client_socket, client_address = server_socket.accept()
            print("Accepted connection from", client_address)

            try:
                handle_client_request(client_socket)
            except Exception as ex:
                print(f"Error handling request: {ex}")
            client_socket.close()

    def handle_client_request(client_socket):
        header_data = client_socket.recv(HEADER_SIZE)
        if len(header_data) == HEADER_SIZE:
            # Unpack the header fields
            client_id, version, code, payload_size = struct.unpack(
                config.received_request_header_format, header_data)
            print('client id', client_id)
            print('version', version)
            print('code', code)
            print('payload_size', payload_size)

            # Receive the payload data based on payload_size
            payload_data = client_socket.recv(payload_size)

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
                code_handlers[code](client_socket, payload_data)

        # print(f"Received: {request}")

    def null_terminator_crop(string: str) -> str:
        name_end_index: int = string.find(b'\0')
        if name_end_index != -1:
            return string[:name_end_index]
        return string

    def get_string(payload_data, request_format: str = "<255s") -> str:
        str1,  = struct.unpack(request_format, payload_data)
        return null_terminator_crop(str1).decode('utf-8')

    def handle_code_1025(client_socket: object, payload_data: str):
        name: str = get_string(payload_data, "<255s")
        # if (check_client(name)):  # is a client
        #     code: int = 2101
        #     header = Header(code, 0)
        #     header_bytes = header.serialize()
        #     message = header_bytes
        #     client_socket.send(message)
        #     return
        code: int = 2100
        client_id: str = register_client(name)
        header = Header(code, len(client_id))
        payload = Payload(client_id)
        header_bytes = header.serialize()
        payload_bytes = payload.serialize()
        message = header_bytes + payload_bytes
        client_socket.send(message)

    def handle_code_1026(client_socket: object, payload_data: str):
        request1026_format: str = "<255s160s"
        _name, public_key_bytes = struct.unpack(
            request1026_format, payload_data)
        name: str = get_string(_name, "255s")
        encrypted_aes_key, aes_key = generate_and_encrypt_aes_key(
            public_key_bytes)
        pass

    def handle_code_1027(client_socket: object, payload_data: str):
        pass

    def handle_code_1028(client_socket: object, payload_data: str):
        request1028_format_package_size = "<I"
        pass

    def handle_code_1029(client_socket: object, payload_data: str):

        pass

    def handle_code_1030(client_socket: object, payload_data: str):
        pass

    def handle_code_1031(client_socket: object, payload_data: str):
        pass

    def handle_registration(client_socket):
        pass

    def handle_send_public_key(client_socket):
        pass

    def handle_send_encrypted_file(client_socket):
        pass
