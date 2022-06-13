from datetime import datetime
import json
import random
import os

from collections import OrderedDict
import matplotlib.pyplot as plt

path_to_sacct = '/Users/evgeniykislov/Documents/msu/ski/glurmo/benchmarks-setup/supercomputer-data.txt'
params_file_name = 'job_params.txt'

def run():
    f = open(path_to_sacct, 'r')
    lines = f.readlines()

    heading = lines.pop(0).split('|')[0:-1]

    print(heading)

    days_distribution = dict()
    duration_distribution = []
    nodes_distribution = []

    dataset = []

    for line in lines:
        values = line.split('|')[0:-1]

        if values[6] != 'COMPLETED':
            continue
        
        start = datetime.strptime(values[3], "%Y-%m-%dT%H:%M:%S")
        end = datetime.strptime(values[4], "%Y-%m-%dT%H:%M:%S")

        if start.day not in days_distribution:
            days_distribution[start.day] = 1
        else:
            days_distribution[start.day] += 1

        duration_seconds = (end-start).seconds
        duration_distribution.append(duration_seconds * 1.0 / 60)
        nodes_distribution.append(int(values[9]))

        if duration_seconds < 60 * 5:
            should_append = random.randint(1, 4) == 4
            if should_append:
                dataset.append((duration_seconds, int(values[9])))

    per_day_avg = sum(list(days_distribution.values())) / len(list(days_distribution.values()))
    duration_avg = sum(duration_distribution) / len(duration_distribution)

    print('Total successfull jobs')
    print(len(duration_distribution))

    print('Average per day')
    print(per_day_avg)

    print('Jobs count per day distribution')
    print(json.dumps(days_distribution, indent=4))

    print('Average duration')
    print(duration_avg)

    duration_groupping = dict()

    for duration in duration_distribution:
        rounded_duration = 10 * (int(round(duration)) // 10)

        if rounded_duration not in duration_groupping:
            duration_groupping[rounded_duration] = 1
        else:
            duration_groupping[rounded_duration] += 1

    ordered_duration_groupping = OrderedDict(sorted(duration_groupping.items(), key=lambda t: t[0]))

    print('Duration frequencies')
    print(json.dumps(ordered_duration_groupping, indent=4))

    print(len(dataset))

    f = open(os.path.join(os.path.dirname(os.path.abspath(__file__)), params_file_name), 'a+')

    total_time = 0

    for item in dataset:
        total_time += item[0]
        f.write(str(item[0]) + ' ' +  str(item[1]) + '\n')

    print(total_time / 60)

    f.close()

    # plot_duration_distrib(duration_distribution)
    # plot_nodes_distrib(nodes_distribution)


def plot_duration_distrib(queued_time_distr):
    plt.hist(queued_time_distr, bins = len(queued_time_distr) // 70, density = False, label = 'Job duration frequency diagram', stacked = True, alpha = 0.5)
    plt.legend(loc = 'upper right')
    plt.xlabel("Jobs duration, minutes")
    plt.ylabel("Job count")
    plt.show()

def plot_nodes_distrib(node_distr):
    distr_dict = dict()

    for nodes in node_distr:
        if str(nodes) in distr_dict:
            distr_dict[str(nodes)] += 1
        else:
            distr_dict[str(nodes)] = 1

    ordered = OrderedDict(sorted(distr_dict.items(), key=lambda t: int(t[0])))

    plt.bar(ordered.keys(), ordered.values(), label = 'Job nodes count')
    plt.legend(loc = 'upper right')
    plt.xlabel("Jobs nodes")
    plt.ylabel("Job count")
    plt.show()

run()
