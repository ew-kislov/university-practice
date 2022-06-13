from abc import ABCMeta, abstractmethod

from ..slurm import Event

class SchedulerEventHandler(object):
    __metaclass__ = ABCMeta

    @abstractmethod
    def handle_event(self, event: Event):
        raise NotImplementedError