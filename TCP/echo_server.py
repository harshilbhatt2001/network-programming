'''
Created on Wed Apr 29 00:04:22 2020
@author: Harshil Bhatt
'''

import socket

HOST = ''   # empty string: the server will accept connections on all available IPv4 interfaces
PORT = 65432

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen(5)     # max of 5 sockets allowed 
    conn, addr = s.accept()
    with conn:
        print("Connected by ", addr)
        while True:
            data = conn.recv(1024)
            if not data:
                break
            print ("Echoed by server\nData Lost: ", conn.sendall(data))