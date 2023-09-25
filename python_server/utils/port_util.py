if __name__ != "__main__":
    import os

    def get_port_number_from_file(file_name):
        if os.path.exists(file_name):
            with open(file_name, "r") as file:
                port_number_string: str = file.read().strip()
                if port_number_string.isdigit():
                    port_number = int(port_number_string)
                    return port_number
        return None
