from src.core.SchedulerAlgorithm import SchedulerAlgorithm
from src.core.SchedulerState import SchedulerState


class FifoAlgorithm(SchedulerAlgorithm):
    def run(self, jobs, state: SchedulerState):
        print(f"FIFO: nodes available - {state.nodes_available}")

        available_jobs = []

        for job in jobs:
            if state.nodes_available >= int(job['TASKS']):
                available_jobs.append(job)

        if len(available_jobs) == 0:
            return (False, available_jobs)
        else:
            return (True, available_jobs)