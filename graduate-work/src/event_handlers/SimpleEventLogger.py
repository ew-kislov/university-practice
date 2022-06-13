from src.slurm import Event

from src.core import SchedulerEventHandler

class SimpleEventLogger(SchedulerEventHandler):
    def handle_event(self, event: Event):
        return