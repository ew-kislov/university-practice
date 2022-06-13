from collections import OrderedDict
import matplotlib.pyplot as plt

path_to_slurm = '/Users/evgeniy/Documents/msu/ski/glurmo/benchmark-results/lomonosov-feb-jobs-280/slurm.txt'
path_to_glurmo = '/Users/evgeniy/Documents/msu/ski/glurmo/benchmark-results/lomonosov-feb-jobs-280/glurmo.txt'
label = 'GLURMO Backfill'

def run():
    slurm_queued_time_distr = process_sacct(path_to_slurm, False)
    glurmo_queued_time_distr = process_sacct(path_to_glurmo, True)

    plot_queues_time_distrib(slurm_queued_time_distr, glurmo_queued_time_distr)
    # plot_queued_time_avg_per_nodes(queued_time_avg_per_nodes)

def process_sacct(filename, should_modify):
    f = open(filename, 'r')
    lines = f.readlines()

    seconds_sum = 0
    count = 0
    queued_time_distr = []
    queued_time_distr_per_nodes = {}

    for line in lines:
        values = line.split(' ')

        # skipping *.batch jobs because they have no queued time
        if len(values[0]) > 5 and values[0][-5:] == 'batch':
            continue

        queued_time_string = values[-2]
        hours, minutes, seconds = queued_time_string.split(':')
        seconds = int(hours) * 3600 + int(minutes) * 60 + int(seconds)

        nodes = int(values[-5])

        count += 1
        seconds_sum += seconds
        queued_time_distr.append(seconds)

        if nodes not in queued_time_distr_per_nodes:
            queued_time_distr_per_nodes[nodes] = [seconds]
        else:
            queued_time_distr_per_nodes[nodes].append(seconds)

    queued_time_distr.sort()

    queued_time_avg_per_nodes = {
        nodes: sum(distr) / len(distr) for nodes, distr in queued_time_distr_per_nodes.items()
    }

    print('Average queued time for job: ' + str(sum(queued_time_distr) / len(queued_time_distr)))
    print('50% percentille queued time for job: ' + str(queued_time_distr[len(queued_time_distr) // 2]))
    print('95% percentille queued time for job: ' + str(queued_time_distr[int(len(queued_time_distr) * 0.95)]))

    print(queued_time_distr)

    return queued_time_distr

def plot_queues_time_distrib(slurm_queued_time_distr, glurmo_queued_time_distr):
    plt.hist(slurm_queued_time_distr, bins = len(slurm_queued_time_distr) // 5, density = False, label = 'Slurm', stacked = True, alpha = 0.5, color='red')
    plt.hist(glurmo_queued_time_distr, bins = len(glurmo_queued_time_distr) // 5, density = False, label = 'External scheduler', stacked = True, alpha = 0.5, color='blue')
    plt.legend(loc = 'upper right')
    plt.xlabel("Time queued")
    plt.ylabel("Jobs count")
    plt.show()

def plot_queued_time_avg_per_nodes(queued_time_avg_per_nodes):
    ordered = OrderedDict(sorted(queued_time_avg_per_nodes.items(), key=lambda t: t[0]))

    names = list(ordered.keys())
    values = list(ordered.values())

    plt.bar(range(len(ordered)), values, tick_label=names)
    plt.show()

run()
