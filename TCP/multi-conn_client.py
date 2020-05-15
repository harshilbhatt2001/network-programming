'''
Created on Wed Apr 29 16:21:22 2020
@author: Harshil Bhatt
'''


import socket
import selectors
import types

messages = [b'Message 1 from client.', b'Message 2 from client.']

def start_connections(host, port, num_conns):
    server_addr = (host, port)
    
    for i in range(num_conns):
        connID = i + 1
        print("Starting connection ", connID, " to ", server_addr)
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.setblocking(False)
        sock.connect_ex(server_addr)
        events = selectors.EVENT_READ | selectors.EVENT_WRITE
        data = types.SimpleNamespace(connID=connID,
                                     msg_total = sum(len(m) for m in messages),
                                     recv_total = 0,
                                     messages = list(messages),
                                     outb = b'')
        sel.register(sock, events, data=data)

def service_connection(key, mask):
    sock = key.fileobj
    