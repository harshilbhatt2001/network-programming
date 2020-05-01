'''
Created on Fri May 01 08:21:22 2020
@author: Harshil Bhatt
'''

import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

udp_host = socket.gethostname()
udp_port = 12345

msg = "hello"
print("UDP Target IP: ", udp_host)
print("UDP Target PORT: ", udp_port)

sock.sendto(msg.encode(), (udp_host, udp_port))