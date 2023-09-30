def main():
    from utils.port_util import get_port_number_from_file
    from utils.init_sql_lite import create_clients_table, create_files_table
    import config
    from utils.init_server import start_server

    print('python server')

    # sql lite init
    create_clients_table()
    create_files_table()

    client_port_number: int = get_port_number_from_file(
        config.client_port_info_file_name)

    if client_port_number == None:
        client_port_number = config.client_default_port_if_the_file_not_exist

    start_server('localhost', client_port_number)


if __name__ == "__main__":
    main()
