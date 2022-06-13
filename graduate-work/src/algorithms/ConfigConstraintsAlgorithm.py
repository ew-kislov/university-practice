from src.core.SchedulerAlgorithm import SchedulerAlgorithm
from src.core.SchedulerState import SchedulerState

from src.core import Config

class ConfigConstraintsAlgorithm(SchedulerAlgorithm):
    def __init__(self):
        self.__config = Config()

    def run(self, jobs, state: SchedulerState):
        if self.__config.get_total_jobs_limit == state.total_jobs:
            return (False, [])

        available_jobs = list(filter(lambda job: state[job["RCLASS"]] != state.jobs_per_partition, jobs))

        if len(available_jobs) == 0:
            return (False, [])

        return (True, available_jobs)