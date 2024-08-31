#!/usr/bin/env python3
"""
copy all *.log from cwd into log_folder_name
"""
import os
import sys
import subprocess

CODE = "/usr/share/code/code"
WORKSPACES = ["AC/ser5_v5_AC.code-workspace", "DC/ser5_v5_DC.code-workspace"]


def start_ac_dc(working_dir):
    """
    Start workspaces for code
    """
    for workspace in WORKSPACES:
        workspace_path = os.path.join(working_dir, workspace)
        cwd = os.path.join(working_dir,workspace.rsplit('/', 1)[0])
        print(f"\t- {workspace} in {cwd}")
        subprocess.Popen([CODE, workspace_path], cwd=cwd)


if __name__ == "__main__":
    print(f"Start multiple instances of code {sys.argv}:")
    pwd = sys.argv[1] if len(sys.argv) > 1 else os.getcwd()
    start_ac_dc(pwd)
    input("Finish with Enter key...")
