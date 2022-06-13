import random
import os

available_nodes_count = [1, 2, 4, 8, 12, 16, 20, 24, 28, 32]
params_file_name = 'job_params.txt'
jobs_count = 5


def get_random_sleep_time():
    return random.randint(1, 8)


def get_random_nodes_count():
    index = random.randint(0, len(available_nodes_count) - 1)

    return available_nodes_count[index]


def run():
    f = open(os.path.join(os.path.dirname(os.path.abspath(__file__)), params_file_name), 'a+')

    for i in range(jobs_count):
        f.write(
            str(get_random_nodes_count()) + ' ' +
            str(get_random_sleep_time()) + '\n'
        )

    f.close()


run()