if __name__ != "__main__":
    server_version: float = 3.0

    client_port_info_file_name: str = "port.info"
    client_default_port_if_the_file_not_exist: int = 1357
    client_url = 'localhost'  # "0.0.0.0"

    maximum_number_of_queued_connections: int = 5

    database_name: str = "`defensive.db`"
    clients_table_name: str = "clients"
    files_table_name: str = "files"
