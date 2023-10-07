def main():
    from utils.port_util import get_port_number_from_file
    from utils.init_sql_lite import create_clients_table, create_files_table
    import config
    from utils.init_server import start_server

    print('python server')

    # sql lite init
    create_clients_table()
    create_files_table()

    client_port_number: int = None
    try:
        client_port_number = get_port_number_from_file(
            config.client_port_info_file_name)
    except Exception as e:
        print("\033[91m", f"Port Exception: {e}", "\033[0m")
        client_port_number = config.client_default_port_if_the_file_not_exist

    start_server('localhost', client_port_number)


if __name__ == "__main__":
    main()
