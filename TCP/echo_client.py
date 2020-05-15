'''
Created on Wed Apr 29 00:04:22 2020
@author: Harshil Bhatt
'''

import socket

HOST = ''   # empty string: the server will accept connections on all available IPv4 interfaces
PORT = 65432

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    s.sendall(b'Hello World')
    data = s.recv(1024)

print('Received ', repr(data))