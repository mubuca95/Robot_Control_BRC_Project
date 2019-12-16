
import random
import socket
import string

from sys import path         
path.append('liblsl-Python\\')
from pylsl import StreamOutlet, StreamInfo

tcpport = int(raw_input('TCP Port: '))
tcpaddress = raw_input('TCP Address (default: 127.0.0.1): ') or '127.0.0.1'

# opening socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect((tcpaddress, tcpport))

# opening stream outlet
randomid = ''.join(random.choice(string.ascii_uppercase + string.digits) for _ in range(10))
info = StreamInfo('TCP Marker Stream', 'Markers', 1, 0, 'string', randomid)
outlet = StreamOutlet(info)

print("waiting for input...")
while True:
    # waiting for data
    data = sock.recv(1)
    
    # sending marker
    if data:
        outlet.push_sample([data])
        print "marker: ", data