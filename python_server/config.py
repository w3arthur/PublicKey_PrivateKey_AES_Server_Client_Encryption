if __name__ != "__main__":
    server_version: int = 3

    client_port_info_file_name: str = "port.info"
    client_default_port_if_the_file_not_exist: int = 1357
    client_url: str = 'localhost'  # "127.0.0.1" "0.0.0.0"

    received_request_header_format: str = "<16sBHI"
    maximum_number_of_queued_connections: int = 5

    database_file_name: str = "defensive.db"
    clients_table_name: str = "clients"
    files_table_name: str = "files"

    filename: str = "file.txt"

    def red_print(string: str):
        print("\033[91m", string, "\033[0m")

    def green_print(string: str):
        print("\033[92m", string, "\033[0m")

    def blue_print(string: str):
        print("\033[94m", string, "\033[0m")
