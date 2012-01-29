#!/usr/bin/python
import sys
import time
import serial
from serial.tools import list_ports

BLUESMIRF_INTERFACE_STRING = 'RN42'

# consider using list_ports.grep() instead
def find_serial_interface():
    ports = list_ports.comports()
    for port in ports:
        if port[0].count(BLUESMIRF_INTERFACE_STRING) == 1:
            print 'Found serial interface: %s' % port[0]
            return port[0]
    return None

def connect_to_serial(serial_interface):
    connection = serial.Serial(serial_interface, 115200, timeout=1000)
    print 'Connected.'
    return connection

def send_command(serial_conn, command, include_newline=True):
    print 'Sending %s' % command
    if include_newline:
        command += "\n"
    serial_conn.write(command)

    print 'Response:'
    sys.stdout.write(serial_conn.readline())
    time.sleep(0.05)
    while serial_conn.inWaiting() > 0:
        sys.stdout.write(serial_conn.readline())
        time.sleep(0.05)
    print ''

def main(*args):
    serial_interface = find_serial_interface()
    if not serial_interface:
        print 'USB port not found, exiting'
        return 1

    print 'Connecting to it...'

    serial_conn = connect_to_serial(serial_interface)

    send_command(serial_conn, "$$$", include_newline=False)
    send_command(serial_conn, "D")
    send_command(serial_conn, "E")

    serial_conn.close()
    return 0

if __name__ == '__main__':
    sys.exit(main(*sys.argv))
