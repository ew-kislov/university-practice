from src.core.SchedulerAlgorithm import SchedulerAlgorithm
from src.core.SchedulerState import SchedulerState

from datetime import datetime
from functools import cmp_to_key

NODES_PRIO_COEFF = 1
QUEUED_TIME_PRIO_COEFF = 2.5

def get_comparator(nodes_available):
    def comparator(x, y):
        x_nodes_fit = 100.0 * int(x['TASKS']) / nodes_available
        y_nodes_fit = 100.0 * int(y['TASKS']) / nodes_available

        x_queued = (datetime.now() - datetime.fromtimestamp(int(x['QUEUETIME']))).seconds
        y_queued = (datetime.now() - datetime.fromtimestamp(int(y['QUEUETIME']))).seconds

        x_prio = x_nodes_fit * NODES_PRIO_COEFF + x_queued * QUEUED_TIME_PRIO_COEFF
        y_prio = y_nodes_fit * NODES_PRIO_COEFF + y_queued * QUEUED_TIME_PRIO_COEFF

        if x_prio < y_prio:
            return 1
        elif x_prio > y_prio:
            return -1
        else:
            return 0

    return comparator

class BackfillAlgorithm(SchedulerAlgorithm):
    def run(self, jobs, state: SchedulerState):
        print(f"Backfill: nodes available - {state.nodes_available}")

        sorted_jobs = sorted([job for job in jobs if state.nodes_available >= int(job['TASKS'])], key=cmp_to_key(get_comparator(state.nodes_available)), reverse=False)

        print([job['id'] for job in sorted_jobs])

        if len(sorted_jobs) == 0:
            return (False, sorted_jobs)
        else:
            return (True, sorted_jobs)