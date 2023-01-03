#!/usr/bin/env python3
"""
    https://askubuntu.com/questions/613973/how-can-i-start-up-an-application-with-a-pre-defined-window-size-and-position
    
    precondition:
        sudo apt-get install wmctrl
        sudo apt-get install xdotool   
"""
import pyautogui
import subprocess
import time


def get(x):
    return subprocess.check_output(["/bin/bash", "-c", x]).decode("utf-8")


def start(command="code", args=".", pos_left=10, pos_right=10, pos_width=40, pos_height=50):
    commandStr = f"{command} {args}"

    ws1 = get("wmctrl -lp")
    subprocess.Popen(["/bin/bash", "-c", commandStr])

    for try_count in range(30):
        ws2 = [w.split()[0:3]
               for w in get("wmctrl -lp").splitlines() if not w in ws1]
        procs = [[(p, w[0]) for p in get("ps -e ww").splitlines()
                  if command in p and w[2] in p] for w in ws2]
        if len(procs) > 0:
            w_id = procs[0][0][1]
            cmd1 = f"wmctrl -ir {w_id} -b remove,maximized_horz"
            cmd2 = f"wmctrl -ir {w_id} -b remove,maximized_vert"
            cmd3 = f"xdotool windowmove {w_id} {pos_left} {pos_right}"
            # cmd4 = f"xdotool windowsize --sync {w_id} {pos_width}% {pos_height}%"
            cmd4 = f"xdotool windowsize --sync {w_id} {pos_width} {pos_height}"
            for cmd in [cmd1, cmd2, cmd3, cmd4]:
                subprocess.call(["/bin/bash", "-c", cmd])
            break
        time.sleep(0.5)


width, height = pyautogui.size()

start('code', '/home/ksc/work/ser4/solar_car_control_system_v3/AC/SER4_v3.code-workspace',
      10, 10, width//2-16, 1400)
time.sleep(1)
start('code', '/home/ksc/work/ser4/solar_car_control_system_v3/DC/SER4_v3.code-workspace',
      width//2, 10, width//2-20, 1400)

# print('fiin.')
# time.sleep(5)
# input("Press Enter to continue... ")
