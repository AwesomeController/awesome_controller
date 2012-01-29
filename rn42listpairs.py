#!/usr/bin/python
import sys
import time
from datetime import datetime, timedelta
import serial
from serial.tools import list_ports

RN42_VIA_USB_STRING = 'usbserial'
RN42_VIA_BLUETOOTH_STRING = 'RN42'
BLUESMIRF_INTERFACE_STRING = RN42_VIA_BLUETOOTH_STRING

# consider using list_ports.grep() instead
def find_serial_interface():
    ports = list_ports.comports()
    print 'Ports found:'
    for port in ports:
        print port[0]
    print '----------'
    for port in ports:
        if port[0].count(BLUESMIRF_INTERFACE_STRING) == 1:
            print 'Using serial interface: %s' % port[0]
            return port[0]
    return None

def connect_to_serial(serial_interface, tries_remaining=5):
    try:
        connection = serial.Serial(serial_interface, 115200, timeout=10, writeTimeout=10)
        print 'Connected.'
        return connection
    except serial.serialutil.SerialException:
        tries_remaining -= 1
        if tries_remaining == 0:
            raise
        print 'Could not connect, retrying (%s retries remaining)' % tries_remaining
        return connect_to_serial(serial_interface, tries_remaining)

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

def enter_command_mode(serial_conn):
    send_command(serial_conn, "$$$", include_newline=False)

def print_debug1(serial_conn):
    send_command(serial_conn, "D")

def print_debug2(serial_conn):
    send_command(serial_conn, "E")

def set_infinite_command_over_bluetooth(serial_conn):
    send_command(serial_conn, "ST,255")

def reset_rn42(serial_conn):
    send_command(serial_conn, "R,1")

def perform_inquiry(serial_conn, timeout):
    send_command(serial_conn, "I,%s" % timeout)
    begin_time = datetime.now()
    print 'Waiting for timeout...'
    max_timedelta = timedelta(seconds=timeout)
    time_difference = datetime.now() - begin_time
    while time_difference < max_timedelta:
        if serial_conn.inWaiting() > 0:
            sys.stdout.write(serial_conn.readline())
        time.sleep(0.1)
        time_difference = datetime.now() - begin_time
    print 'Timed out'
    time.sleep(2)
    if serial_conn.inWaiting() > 0:
        sys.stdout.write(serial_conn.readline())
    else:
        print 'no end result'

def main(*args):
    serial_interface = find_serial_interface()
    if not serial_interface:
        print 'Serial interface not found, exiting'
        return 1

    print 'Connecting to it...'

    serial_conn = connect_to_serial(serial_interface)

    enter_command_mode(serial_conn)
    print_debug1(serial_conn)
    print_debug2(serial_conn)

    set_infinite_command_over_bluetooth(serial_conn)
    reset_rn42(serial_conn)
    serial_conn.close()

    time.sleep(1)

    print 'Reconnecting to it...'
    serial_conn = connect_to_serial(serial_interface)

    enter_command_mode(serial_conn)
    print_debug1(serial_conn)
    print_debug2(serial_conn)

    time.sleep(10)

    perform_inquiry(serial_conn, timeout=10)

    send_command(serial_conn, "---")
    serial_conn.close()
    return 0

if __name__ == '__main__':
    sys.exit(main(*sys.argv))
