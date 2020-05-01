'''
Created on Fri May 01 18:27:22 2020
@author: Harshil Bhatt
'''

import dpkt
import socket

def printPcap(pcap):
    # ts-> timestamp    buf-> buffer
    for (ts, buf) in pcap: 
        try:
            eth = dpkt.ethernet.Ethernet(buf)
            ip = eth.data
            # read source ip
            src = socket.inet_ntoa(ip.src)
            # read destination ip
            dst = socket.inet_ntoa(ip.dst)
            
            print("Source: " + src + "\tDestination: " + dst)
        except:
            pass

def main():

    f = open('first.pcap','r')
    pcap = dpkt.pcap.Reader(f)
    printPcap(pcap)

if __name__ == "__main__":
    main()
