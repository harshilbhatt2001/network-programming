'''
Created on Fri May 01 08:21:22 2020
@author: Harshil Bhatt
'''


import socket

# For UDP, type is SOCK_DGRAM
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

udp_host = socket.gethostname()
udp_port = 12345

sock.bind((udp_host, udp_port))

while True:
    print("Waiting for client")
    data, addr = sock.recvfrom(1024)
    print("Recieved: ", data, "from IP: ", addr)
