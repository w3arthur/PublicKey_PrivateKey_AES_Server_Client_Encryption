from utils.port_util import get_port_number_from_file
import utils.sql_lite_init as sql_lite_init
import utils.file_util as file_util
import config  # as config
import utils.server as server

print('python server')

# reset data from db and folder
file_util.delete_database_file_name()
file_util.reset_stored_files_folder()

# sql lite init
sql_lite_init.create_clients_table()
sql_lite_init.create_files_table()

client_port_number: int = None
try:
    client_port_number = get_port_number_from_file(
        config.client_port_info_file_name)
except Exception as e:
    config.red_print(f"Port Exception: {e}")
    client_port_number = config.client_default_port_if_the_file_not_exist

server.start_server('localhost', client_port_number)
