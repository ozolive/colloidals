#!/usr/bin/python
import time
import serial
import time
import csv
import os.path
import struct

process_log = 'process_log.csv'


BAUD_RATE=9600
#BAUD_RATE=115200

# configure the serial connections (the parameters differs on the device you are connecting to)
ser=None

def open_port():
    global ser
    ser = serial.Serial(
        port=None,
        baudrate=BAUD_RATE,
        parity=serial.PARITY_ODD,
        stopbits=serial.STOPBITS_TWO,
        bytesize=serial.SEVENBITS,
        timeout=1,
    )

    for port_prefix in ('COM','/dev/ttyUSB','/dev/ttyACM'):
        for port_num in range(0,20):
            port_name = port_prefix + str(port_num)
            if ((port_prefix == 'COM') or (os.path.exists(port_name))):
                ser.port = port_name
                try:
                    ser.open()
                except serial.serialutil.SerialException:
                    pass
                if ser.isOpen():
                    print "Opened port",ser.port
                    break;
        else:
            continue
        break

    if ser.isOpen():   
#        ser.setDTR(False)
#        time.sleep(2)
#        ser.setDTR(True)
        while(ser.inWaiting() > 0):
            junk = ser.read(1)

        ser.write("U=" + str(int(time.time())) + '\n')
        return True
    else:
        print "Couldnt open any port..."
        return False


def reset_port():
    try:
        if ser.isOpen():
            ser.close()
    except:
        pass
    opened=False
    while not opened:
        opened = open_port()
        time.sleep(5)

    
outbuff = ''
def get_reply():
    global outbuff
    try:
      while((ser.inWaiting() > 0)):
        while ser.inWaiting() > 0:
            c = ser.read(1)
            if c == "\n":
              outbuff = outbuff.replace("\n","") 
              write_log(process_log,outbuff)
              outbuff=''
              break
            outbuff += c
        time.sleep(16/BAUD_RATE)
    except IOError as e:
        print "Reading", outbuff
        print "ERROR", "IO",e
        reset_port()
    except OSError as e:
        print "Reading", outbuff
        print "ERROR", "OS",e
        reset_port()


def write_log(fname,outstr):
#    print "Writing", outstr,"To",fname
    
    if os.path.isfile(fname):
        fd = open(fname, "ab")
    else:
        fd = open(fname, "wb")
        fd.write("time,ma_min,ma_avg,ma_max,ma_var_avg,ma_cur,ma_pwm_avg,ma_pwm_var_avg,outv_avg,ma,ppm\n")
    fd.write(str(int(time.time())) + ',' + outstr + "\n")
    fd.close()

open_port()
while 1 :
        get_reply()
        time.sleep(0.2)
