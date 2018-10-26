#!/usr/bin/python

import sys
import random
import string
import time


sys.path.append('/usr/lib/python2.7/dist-packages/scapy')
from scapy.all import *
from Queue import Queue
from threading import Thread


#SRC_IP = '192.168.1.13'
#DST_IP = '192.168.1.13'

SRC_IP = '192.168.1.3'
DST_IP = '192.168.1.2'

SRC_PORT = 5588
DST_PORT = 5555

IFACE = 'enp0s3'

tcp_queue = Queue()

def rand_str(size):
    return ''.join([random.choice(string.ascii_letters) for i in xrange(size)])#List Comprehension列表推导式

def sniff_callback(packet):
    if packet[TCP].sport == DST_PORT or packet[TCP].dport == SRC_PORT:
        print packet.sprintf("%.time% %-s,IP.src%:%TCP.sport% --> %-s,IP.dst%:%TCP.dport% [%TCP.flags%]")
        tcp_queue.put(packet)

def sniff_run():
    return sniff(iface = IFACE, filter = 'tcp', prn = sniff_callback)

def scapy_do_run():

    
    sniff_worker = Thread(target=sniff_run)
    sniff_worker.daemon = True
    sniff_worker.start()
    
    PAYLOAD = rand_str(100)
    #print PAYLOAD

    print "send SYN"
    SYN = IP(src = SRC_IP,dst = DST_IP) / TCP(sport = SRC_PORT,dport = DST_PORT, seq = 100, flags = "S")
    send(SYN)

    print "recv SYNACK"
    SYNACK = tcp_queue.get(timeout = 10)
    tcp_queue.task_done()
    print "Unexpected flags:" + SYNACK.sprintf("%TCP.flags%")
    assert(SYNACK[TCP].flags == 0x12)

    print "send ACK"
    ACK = IP(src = SRC_IP,dst = DST_IP) / TCP(sport = SRC_PORT,dport = DST_PORT, seq = SYNACK.ack, ack = SYNACK.seq + 1, flags = "A")
    send(ACK)


    time.sleep(10)

    print "send local-FINACK"
    LFINACK = IP(src=SRC_IP,dst=DST_IP) / TCP(sport=SRC_PORT,dport=DST_PORT, seq = ACK.seq, ack = ACK.ack, flags="FA")
    send(LFINACK)

    print "recv peer-ACK or peer-FINACK"
    PACK = tcp_queue.get(timeout = 10)
    tcp_queue.task_done()
    if PACK[TCP].flags == 0x10:
        print "==>try to recv peer-FINACK"
        PFINACK = tcp_queue.get(timeout = 10)
        assert(PFINACK[TCP].flags == 0x11)
        PACK = PFINACK
    elif PACK[TCP].flags == 0x11:
        print "==>get a peer-FINACK"
    else:
        print "Unexpected flags:" + PACK.sprintf("%TCP.flags%")

    print "send local-ACK"
    LACK = IP(src=SRC_IP,dst=DST_IP) / TCP(sport=SRC_PORT,dport=DST_PORT, seq = PACK.ack, ack = PACK.seq + 1, flags="A")
    send(LACK)

if __name__ == '__main__':
    scapy_do_run()
