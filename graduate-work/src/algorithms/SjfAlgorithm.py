from functools import cmp_to_key

from src.core.SchedulerAlgorithm import SchedulerAlgorithm
from src.core.SchedulerState import SchedulerState


def comparator(x, y):
        if int(x['TASKS']) > int(y['TASKS']):
            return 1
        elif int(x['TASKS']) < int(y['TASKS']):
            return -1
        else:
            return 0


class SjfAlgorithm(SchedulerAlgorithm):
    def run(self, jobs, state: SchedulerState):
        deferred_jobs = 0
        iteration = 0

        sorted_jobs = sorted(jobs, key=cmp_to_key(comparator), reverse=False)

        print(f"SJF: nodes available - {state.nodes_available}")

        for job in sorted_jobs:
            if state.nodes_available < int(job['TASKS']):
                if iteration == len(sorted_jobs) - 1:
                    sorted_jobs.append(sorted_jobs.pop(iteration))
                deferred_jobs += 1

            iteration += 1

        if deferred_jobs == len(sorted_jobs):
            return (False, sorted_jobs)
        else:
            return (True, sorted_jobs)