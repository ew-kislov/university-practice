from copy import deepcopy

from src.core.SchedulerAlgorithm import SchedulerAlgorithm
from src.core.SchedulerEventHandler import SchedulerEventHandler
from src.core.SchedulerState import SchedulerState

from src.slurm import EEvent
from src.slurm.SlurmWiki2Service import SlurmWiki2Service
from src.slurm.SlurmService import SlurmService


class Scheduler(SchedulerEventHandler):
    def __init__(self, slurm_service: SlurmService):
        if slurm_service:
            self.__slurm_service = slurm_service
        else:
            self.__slurm_service = SlurmWiki2Service()

        self.__algorithms: list[SchedulerAlgorithm] = []
        self.__event_handlers: list[SchedulerEventHandler] = []
        self.__pending_jobs = dict()

        # initiating scheduler state

        self.__update_scheduler_state()

    def add_algorithm(self, algorithm: SchedulerAlgorithm):
        self.__algorithms.append(algorithm)

    def add_event_handler(self, event_handler):
        self.__event_handlers.append(event_handler)

    def run(self):
        self.__slurm_service.set_subscriber(self)

        for event_handler in self.__event_handlers:
            self.__slurm_service.set_subscriber(event_handler)

        self.__slurm_service.run()

    def handle_event(self, event):
        for event_handler in self.__event_handlers:
            event_handler.handle_event(event)

        if (event.get_type() == EEvent.JobQueued):
            print(f"Scheduler: job #{event.get_payload()['id']} queued")

            self.__pending_jobs[event.get_payload()
                                ['id']] = event.get_payload()

            self.__execute_available_jobs()

        elif (event.get_type() == EEvent.JobDone):
            print(f"Scheduler: job #{event.get_payload()} done")

            if len(self.__pending_jobs) == 0:
                return

            self.__execute_available_jobs()

        elif (event.get_type() == EEvent.Ping):
            print(f"Scheduler: got ping event")

            if len(self.__pending_jobs) == 0:
                return

            self.__execute_available_jobs()

    def __execute_available_jobs(self):
        print('Scheduler: executing queued jobs')
        while True:
            self.__update_scheduler_state()

            if len(self.__pending_jobs) == 0:
                break

            found_job = self.__apply_algorithms()

            if found_job:
                self.__slurm_service.start_job(found_job)
                del self.__pending_jobs[found_job['id']]
            else:
                break

    def __apply_algorithms(self):
        scheduler_state = deepcopy(self.__scheduler_state)

        # NOTE: using immutable list for more explicit approach

        buffer_jobs = deepcopy(list(self.__pending_jobs.values()))

        for algorithm in self.__algorithms:
            (found_job, jobs) = algorithm.run(buffer_jobs, scheduler_state)

            if not found_job:
                return None

            buffer_jobs = deepcopy(jobs)

        return buffer_jobs[0]

    def __update_scheduler_state(self):
        nodes_available = self.__slurm_service.get_idle_nodes_count()
        total_jobs = self.__slurm_service.get_total_jobs()
        jobs_per_partition = self.__slurm_service.get_jobs_per_partition()

        self.__scheduler_state = SchedulerState(nodes_available, total_jobs,
                                                jobs_per_partition)
