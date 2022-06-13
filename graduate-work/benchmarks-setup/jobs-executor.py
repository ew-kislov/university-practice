import os
import subprocess

params_file_name = 'job_params.txt'

def run():
    f = open(os.path.join(os.path.dirname(os.path.abspath(__file__)), params_file_name), 'r')
    lines = f.readlines()
    
    for line in lines:
        node_count_raw, sleep_time_raw = line.split(' ')
        node_count = int(node_count_raw)
        sleep_time = int(sleep_time_raw)

        subprocess.call(['sbatch', 'batch-script-' + str(node_count), str(sleep_time) ])

run()