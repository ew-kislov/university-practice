from dataclasses import dataclass
import typing

@dataclass
class SchedulerState:
    nodes_available: int
    total_jobs: int
    jobs_per_partition: typing.Dict[str, int]