from constants import *
import struct

def get_port(file_name = PORT_FILE):
    try:
        with open(file_name, "r") as file:
            port = file.read().strip()
            return int(port) if port.isdigit() else DEFAULT_PORT
    except (FileNotFoundError, ValueError):
        print("Error: myport.info not found. Using default port: ", DEFAULT_PORT)
        return DEFAULT_PORT
    

def unpack_header(data):
    header_format = f'<{ID_SIZE}s B H I'  
    client_id, version, code, payload_size = struct.unpack(header_format, data[:REQUEST_HEADER_SIZE])
    payload = data[REQUEST_HEADER_SIZE : REQUEST_HEADER_SIZE + payload_size]
    return client_id, version, code, payload_size, payload


def check_type(message_type, message_size):
    if message_type not in {MESSAGE_TYPE_GET_SYMMETRIC_KEY, MESSAGE_TYPE_SEND_SYMMETRIC_KEY, 
                    MESSAGE_TYPE_SEND_MESSAGE, MESSAGE_TYPE_SEND_FILE}:
        return False
    if message_type != MESSAGE_TYPE_GET_SYMMETRIC_KEY and message_size == 0:
        return False
    if message_type == MESSAGE_TYPE_SEND_SYMMETRIC_KEY and message_size != RSA_ENCRYPTED_MESSAGE_LENGTH:
        return False
    if message_type == MESSAGE_TYPE_SEND_MESSAGE or message_type == MESSAGE_TYPE_SEND_FILE:
        if message_size % 16 != 0:  # message encrypted with AES-BCB encryption always divide by 16
            return False
    return True

