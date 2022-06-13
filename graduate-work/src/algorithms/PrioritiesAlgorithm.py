from sqlalchemy import or_
from sqlalchemy.sql.expression import true

from src.config import DbConfig
from src.model import User, JobRule, BoundJobRule
from src.schema import BoundJobRuleSchema
from src.core import SchedulerAlgorithm
from src.core.SchedulerState import SchedulerState

class PrioritiesAlgorithm(SchedulerAlgorithm):
    # def __init__(self):
        # self.__db_config = DbConfig()
        # self.__session = self.__db_config.sesson_factory()
        # self.__prioritized_jobs = dict()

    def run(self, jobs, state: SchedulerState):
        # TODO 
        return (True, jobs)

    # def __get_hard_priority(self, uid):
    #     try:
    #         bound_rules = list(
    #             self.__session
    #                 .query(BoundJobRuleSchema)
    #                 .join(User, isouter=True)
    #                 .join(JobRule, isouter=True)
    #                 .filter(JobRule.hard_priority != None)
    #                 .filter(or_(User.uid == uid, BoundJobRule.is_default == true()))
    #         )
    #     except:
    #         print('Error while processing database query. Please tru again later.')

    #     uid_bound_rules = list(filter(lambda rule: rule.user and rule.user.uid == uid, bound_rules))

    #     if uid_bound_rules and uid_bound_rules[0] != None:
    #         return uid_bound_rules[0].job_rule.hard_priority

    #     default_bound_rules = list(filter(lambda rule: rule.is_default == True, bound_rules))

    #     if default_bound_rules and default_bound_rules[0] != None:
    #         return default_bound_rules[0].job_rule.hard_priority

    #     raise Exception('No priorities found for this job.')


    # def __queue_job(self, job):
    #     hard_priority = self.__get_hard_priority(job['user'])

    #     if hard_priority not in self.__prioritized_jobs:
    #         self.__prioritized_jobs[hard_priority] = [ job ]
    #     else:
    #         self.__prioritized_jobs[hard_priority].append(job)


    # def __start_next_job(self):
    #     if not self.__prioritized_jobs.keys():
    #         return

    #     first_hard_priority = min(self.__prioritized_jobs.keys())
    #     first_priority_job = self.__prioritized_jobs[first_hard_priority].pop(0)

    #     if not self.__prioritized_jobs[first_hard_priority]:
    #         del self.__prioritized_jobs[first_hard_priority]

    #     return first_priority_job