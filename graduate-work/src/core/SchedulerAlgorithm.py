from typing import Any, Tuple, List
from abc import ABCMeta, abstractmethod

from src.core.SchedulerState import SchedulerState

class SchedulerAlgorithm(object):
    __metaclass__ = ABCMeta

    @abstractmethod
    def run(self, jobs, state: SchedulerState) -> Tuple[bool, List[dict]]:
        raise NotImplementedError