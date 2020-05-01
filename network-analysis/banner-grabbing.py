'''
Created on Fri May 01 09:27:22 2020
@author: Harshil Bhatt
'''


import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

t_host = str(input("Enter Host name: "))
t_port = int(input("Enter Port: "))

sock.connect((t_host, t_port))
sock.send('GET HTTP/1.1 \r\n'.encode())

ret = sock.recv(1024)
print("[+] " + str(ret.decode('utf-8')))