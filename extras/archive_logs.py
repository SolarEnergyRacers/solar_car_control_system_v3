#!/usr/bin/env python3
"""
copy all *.log from cwd into log_folder_name
"""
import os
import re
import sys

log_folder_name = 'logs_archive'

def find_unique_name(file_path_with_ext):
    index = 1
    unique_file_path = file_path_with_ext
    file_path, file_ext = tuple(os.path.splitext(file_path_with_ext))
    while os.path.exists(unique_file_path):
        unique_file_path = f"{file_path}--{index:03}{file_ext}"
        index += 1
    return unique_file_path


def move_all_log_files(working_dir):
    log_folder_path = os.path.join(working_dir, log_folder_name)
    if not os.path.exists(log_folder_path):
        os.mkdir(log_folder_path)

    log_files = [f for f in os.listdir(working_dir) if re.match(r"[a-zA-Z_0-9$-]+.*(\.log)", f)]
    for file_name in log_files:
        log_file_path = find_unique_name(os.path.join(log_folder_path, file_name))
        os.rename(file_name, log_file_path)
        print(f"{file_name:30s} → {log_file_path}")


if __name__ == "__main__":
    print(f'Copy *.log files into {log_folder_name} folder. Add index if already exists') 
    working_dir = sys.argv[1] if len(sys.argv) > 1 else os.getcwd()
    move_all_log_files(working_dir)
    input('Finish with Enter key...')

