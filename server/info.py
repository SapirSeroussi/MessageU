import datetime
import struct
import threading
import sqlite3

from constants import *

class ClientsManager:
    def __init__(self, db_manager):
        self._clients = {}  # Maps client_id to ClientInfo
        self._name_to_client_id = {}  # Maps name to client_id
        
        self._db_manager = db_manager
        self._last_saved_index = 0  # Track where we left off saving messages
        self.reload_data()

    def get_client_by_id(self, client_id):
        return self._clients.get(client_id)
    
    def get_clients_list(self):
        return self._clients.values()
    
    def client_exists(self, client_id):
        return client_id in self._clients
    

    def _add_client_from_db(self, client_id, name, public_key, last_seen=None):
        if name in self._name_to_client_id:
            return None
        client_info = ClientInfo(client_id, name, public_key, last_seen)
        self._clients[client_id] = client_info
        self._name_to_client_id[name] = client_id
        return client_info
    
    def add_client(self, client_id, name, public_key):        
        if len(client_id) != ID_SIZE:
            print(f"Error: Invalid client ID '{client_id}'.")
            return False
    
        if name == '' or len(name) > NAME_SIZE:
            print(f"Error: Invalid name '{name}'.")
            return False

        if public_key == '' or len(public_key) != PUBLIC_KEY_SIZE:
            print(f"Error: Invalid public key '{public_key}'.")
            return False
        
        if self._add_client_from_db(client_id, name, public_key):
            self._db_manager.execute_query(
                "INSERT OR REPLACE INTO clients (ClientID, userName, PublicKey, LastSeen) VALUES (?, ?, ?, ?)",
                (client_id, name, public_key, None)
            )
            return True
        
        print(f"Error: Name '{name}' already exists.") 
        return False

    
    def update_last_seen(self, client_id):
        if client_id not in self._clients:
            return False
        self._clients[client_id].update_last_seen()
        
    def add_message(self, client_id, message):
        if client_id not in self._clients:
            return False
        self._clients[client_id].add_message(message)
    
    def add_message(self, client_id, message):
        if client_id not in self._clients:
            return False
        self._clients[client_id].add_message(message)
        self._db_manager.execute_query(
            "INSERT INTO messages (ToClient, FromClient, Type, Content) VALUES (?, ?, ?, ?)",
            (message.get_to_client(), message.get_from_client(), message.get_type(), message.get_content())
        )
        return True

    
    # return the list as bytes and reset it
    def get_bytes_messages_list(self, client_id):
        if client_id not in self._clients:
            return False
        messages = self._clients[client_id].get_bytes_messages_list()
        self._db_manager.execute_query("DELETE FROM messages WHERE ToClient = ?", (client_id,))
        return messages
    
    def reload_data(self):
        clients = self._db_manager.fetch_all("SELECT ClientID, userName, PublicKey, LastSeen FROM clients")
        for client_id, name, public_key, last_seen in clients:
            self._add_client_from_db(client_id, name, public_key, last_seen)

        messages = self._db_manager.fetch_all("SELECT ToClient, FromClient, Type, Content FROM messages")
        for to_client, from_client, msg_type, content in messages:
            if to_client in self._clients:
                message = MessageInfo(to_client, from_client, msg_type, content)
                self._clients[to_client].add_message(message)

    

class DatabaseManager:
    def __init__(self, db_name="defensive.db"):
        self.db_name = db_name
        self._db_lock = threading.Lock()
        self._init_db()

    def _init_db(self):
        with self._db_lock:
            self.conn = sqlite3.connect(self.db_name, check_same_thread=False)
            self.conn.row_factory = sqlite3.Row
            self.cursor = self.conn.cursor()
            self.cursor.execute("PRAGMA foreign_keys = ON;")

            self.cursor.execute('''
                CREATE TABLE IF NOT EXISTS clients (
                    ClientID BLOB PRIMARY KEY,
                    userName TEXT UNIQUE,
                    PublicKey TEXT,
                    LastSeen TEXT
                )
            ''')
            self.cursor.execute('''
                CREATE TABLE IF NOT EXISTS messages (
                    MessageId INTEGER PRIMARY KEY AUTOINCREMENT,
                    ToClient BLOB,
                    FromClient BLOB,
                    Type INTEGER,
                    Content BLOB,
                    FOREIGN KEY(ToClient) REFERENCES clients(ClientID),
                    FOREIGN KEY(FromClient) REFERENCES clients(ClientID)
                )
            ''')
            self.conn.commit()

    def execute_query(self, query, params=()):
        with self._db_lock:
            self.cursor.execute(query, params)
            self.conn.commit()

    def fetch_all(self, query, params=()):
        with self._db_lock:
            self.cursor.execute(query, params)
            return self.cursor.fetchall()

    def close(self):
        self.conn.close()




class ClientInfo:
    def __init__(self, client_id, name, public_key, last_seen=None):
        self._client_id = client_id  
        self._name = name        
        self._public_key = public_key

        self._last_seen = last_seen if last_seen else datetime.datetime.now() 

        self._messages_list = []
        
    
    def get_client_id(self):
        return self._client_id
    
    def get_name(self):
        return self._name
    
    def get_public_key(self):
        return self._public_key
    
    def get_last_seen(self):
        return self._last_seen

    def update_last_seen(self):
        self._last_seen = datetime.datetime.now()
    
    def add_message(self, message):
        self._messages_list.append(message)
    
    # return the list as bytes and reset it
    def get_bytes_messages_list(self):
        messages_bytes = [message.to_byte_stream() for message in self._messages_list]
        self._messages_list = []  # Reset the list
        return messages_bytes


class MessageInfo:
    _message_counter = 1

    def __init__(self, to_client, from_client, type, content):
        self._message_id = MessageInfo._message_counter
        MessageInfo._message_counter += 1

        self._to_client = to_client
        self._from_client = from_client
        self._type = type
        self._content = content

    def get_message_id(self):
        return self._message_id
    
    def get_to_client(self):
        return self._to_client
    
    def get_from_client(self):
        return self._from_client
    
    def get_type(self):
        return self._type
    
    def get_content(self):
        return self._content

    def to_byte_stream(self):
        client_id = self._from_client
        message_id = struct.pack('<I', self._message_id) 
        message_type = struct.pack('<B', self._type)  
        message_size = struct.pack('<I', len(self._content))  
        content = self._content.ljust(len(self._content), b'\x00')  

        return client_id + message_id + message_type + message_size + content
