#!/usr/bin/env python3
"""
 
"""
import pygetwindow
import subprocess
import time

width = 605
height = 460
p1 = subprocess.Popen(fr'code', fr'/home/ksc/work/ser4/solar_car_control_system_v3/AC/SER4_v3_AC.code-workspace', shell=True)
#time.slepp(3)
p2 = subprocess.Popen(fr'code', fr'/home/ksc/work/ser4/solar_car_control_system_v3/AC/SER4_v3_DC.code-workspace', shell=True)
#time.sleep(3)

while len(pygetwindow.getWindowsWithTitle('Visual Studio Code')) < 2:
    print('.', end='')
wins = pygetwindow.getWindowsWithTitle('Visual Studio Code')
for i, win in enumerate(wins):
    win.resizeTo(width, height)
    if i == 0:
        win.moveTo(1753, win.top)
    elif i == 1:
        win.moveTo(1753-width, win.top)

#----------------------------------------------

