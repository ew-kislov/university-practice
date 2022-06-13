import os
import subprocess
from string import zfill
import time
from datetime import datetime

params_file_name = 'job_params_target.txt'
sbatch_file_name = 'dynamic-sbatch-script'
jobs_interval_sec = 2
max_jobs = 500

def run():
    f = open(os.path.join(os.path.dirname(os.path.abspath(__file__)), params_file_name), 'r')
    lines = f.readlines()

    i = 0
    
    for line in lines:
        if max_jobs == i:
            print('Reached max jobs count: ' + str(max_jobs))
            break

        current_time = datetime.now()

        sleep_time_raw, node_count_raw = line.split(' ')
        node_count = int(node_count_raw)
        sleep_time = int(sleep_time_raw)

        sleep_minutes = sleep_time // 60 + 1

        print('Job for ' + str(node_count) + ' nodes, ' + str(sleep_time) + ' sleep seconds.')

        sbatch_file = open(os.path.join(os.path.dirname(os.path.abspath(__file__)), sbatch_file_name), 'w')
        sbatch_file.write('#!/bin/sh\n#SBATCH --time=00:' + zfill(str(sleep_minutes), 2) + ':00\n#SBATCH -n ' + str(node_count) + '\npython batch-script.py $1')

        sbatch_file.close()

        subprocess.call(['sbatch', sbatch_file_name, str(sleep_time) ])

        os.remove(os.path.join(os.path.dirname(os.path.abspath(__file__)), sbatch_file_name))

        interval_end_sec = jobs_interval_sec - (datetime.now() - current_time).microseconds / 1000000

        print('Waiting ' + str(interval_end_sec) + ' seconds till next job...')

        time.sleep(interval_end_sec)

        i += 1

run()