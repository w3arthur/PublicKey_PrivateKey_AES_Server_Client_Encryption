def main():
    from utils.port_util import get_port_number_from_file
    from utils.init_sql_lite import create_clients_table, create_files_table
    from config import client_port_info_file_name, client_default_port_if_the_file_not_exist
    from utils.init_server import start_server

    print('python server')

    # sql lite init
    create_clients_table()
    create_files_table()

    client_port_number: int = None

    client_port_number = get_port_number_from_file(client_port_info_file_name)

    if client_port_number == None:
        client_port_number = client_default_port_if_the_file_not_exist

    start_server('localhost', client_port_number)


if __name__ == "__main__":
    main()
