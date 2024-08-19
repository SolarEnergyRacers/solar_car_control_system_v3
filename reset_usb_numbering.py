#!/usr/bin/env python
"""
Run it like this:

$ sudo python reset__numbering.py driver_name

Where driver_name is the output from

$ lsmod | grep usbserial
"""
import os
import sys
from subprocess import Popen, PIPE
import fcntl

if len(sys.argv) != 2 or len(sys.argv[-1]) < 1:
    print("Usage:\n\treset_usb_numbering.py driver name")
    sys.exit()
driver = sys.argv[-1]
print(f"Resetting driver: '{driver}'...")
USBDEVFS_RESET= 21780

try:
    lsusb_out = Popen("lsusb | grep -i %s"%driver, shell=True, bufsize=64, stdin=PIPE, stdout=PIPE, close_fds=True).stdout.read().strip().split()
    if len(lsusb_out) < 4:
        print(f"[ERR] Driver '{driver}' not found.")
        sys.exit()
    bus = lsusb_out[1].decode("utf-8")
    device = lsusb_out[3][:-1].decode("utf-8")
    f = open(f"/dev/bus/usb/{bus}/{device}", 'w', os.O_WRONLY)
    fcntl.ioctl(f, USBDEVFS_RESET, 0)
except Exception as ex:
    print(f"Failed to reset device. Exception: {ex}")
    
