from dotenv import load_dotenv

load_dotenv()

from .slurm import SlurmWiki2Service
from .core import Scheduler
from .algorithms import BackfillAlgorithm, LimitsAlgorithm, PrioritiesAlgorithm, ConfigConstraintsAlgorithm
from .event_handlers import SimpleEventLogger

# Initiate scheduler(providing slurm service implementation)

scheduler = Scheduler(SlurmWiki2Service())

# Adding chain of algorithms(order matters)

# scheduler.add_algorithm(ConfigConstraintsAlgorithm())
# scheduler.add_algorithm(LimitsAlgorithm())
scheduler.add_algorithm(BackfillAlgorithm())
# scheduler.add_algorithm(PrioritiesAlgorithm())

# Adding event handlers(order doesn't matter)

scheduler.add_event_handler(SimpleEventLogger())

# Starting scheduler

scheduler.run()