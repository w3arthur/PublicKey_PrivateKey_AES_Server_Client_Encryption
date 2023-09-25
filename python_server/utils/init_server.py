if __name__ != "__main__":
    import socket
    import struct
    import hashlib
    import os

    from config import maximum_number_of_queued_connections

    def start_server(url, port):
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.bind((url, port))
        server_socket.listen(maximum_number_of_queued_connections)

        print(f"Server is listening on port {port}")

        while True:
            client_socket, client_address = server_socket.accept()
            try:
                handle_client_request(client_socket)
            except Exception as ex:
                print(f"Error handling request: {ex}")
            client_socket.close()

    def handle_client_request(client_socket):
        # read the message from client
        request = client_socket.recv(1024).decode()

        print(f"Received: {request}")

        response = "hello"
        client_socket.send(response.encode())

    def handle_registration(client_socket):
        # טיפול בבקשה לרישום משתמש
        pass

    def handle_send_public_key(client_socket):
        # טיפול בבקשה לשליחת מפתח ציבורי
        pass

    def handle_send_encrypted_file(client_socket):
        # טיפול בבקשה לשליחת קובץ מוצפן
        pass
