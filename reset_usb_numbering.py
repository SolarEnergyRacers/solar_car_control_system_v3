#!/usr/bin/env python
"""
Run it like this:

$ sudo python reset__numbering.py driver_name

Where driver_name is the output from

$ lsmod | grep usbserial
"""
import os
import subprocess
import sys
from subprocess import Popen, PIPE
import fcntl

def list_usb():
    """
    List all usb devices
    """
    try:
        # Execute the command and capture the output
        result = subprocess.run("lsmod", shell=True, check=True, stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE, text=True)
        usb_drivers = [s for s in result.stdout.strip().split("\n") if "usbserial" in s ]
        # Return the standard output from the command
        return usb_drivers
    except subprocess.CalledProcessError as e:
    # If the command fails, return the error output
        return f"Error: {e.stderr.strip()}"   
    return ""

if len(sys.argv) != 2 or len(sys.argv[-1]) < 1:
    print("Usage:\n\treset_usb_numbering.py driver name")
    usb_devices = '\n'.join(list_usb())
    print(f"\tFound Drivers (lsmod | grep usbserial): \n\t\t{usb_devices}")
    sys.exit()
driver = sys.argv[-1]
print(f"Resetting driver: '{driver}'...")
USBDEVFS_RESET= 21780

try:
    lsusb_out = Popen(f"lsusb | grep -i {driver}", shell=True, bufsize=64,
                      stdin=PIPE, stdout=PIPE, close_fds=True).stdout.read().strip().split()
    if len(lsusb_out) < 4:
        print(f"[ERR] Driver '{driver}' not found.")
        sys.exit()
    bus = lsusb_out[1].decode("utf-8")
    device = lsusb_out[3][:-1].decode("utf-8")
    f = open(f"/dev/bus/usb/{bus}/{device}", 'w', os.O_WRONLY)
    fcntl.ioctl(f, USBDEVFS_RESET, 0)
except Exception as ex:
    print(f"Failed to reset device. Exception: {ex}")
