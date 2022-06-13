from sqlalchemy import or_
from sqlalchemy.sql.expression import true

from src.config import DbConfig
from src.model import User, BoundLimit, LimitItem, Job
from src.core import SchedulerAlgorithm
from src.core.SchedulerState import SchedulerState

class LimitsAlgorithm(SchedulerAlgorithm):
    def __init__(self):
        self.__db_config = DbConfig()
        self.__session = self.__db_config.sesson_factory()

    def run(self, jobs, state: SchedulerState):
        current_uids = []
        for job in jobs:
            uid = job['UNAME']
            if uid not in current_uids:
                current_uids.append(uid)

        current_limit = self.__get_user_limits(current_uids)

        return (True, jobs)

    def __get_user_limits(self, uids):
        try:
            bound_limits = list(
                self.__session
                    .query(BoundLimit)
                    .join(User, isouter=True)
                    .join(LimitItem, isouter=True)
                    .filter(or_(User.uid == User.uid.in_(uids), BoundLimit.is_default == true()))
            )
        except:
            print('Error while processing database query. Please try again later.')

        limits = dict()

        for uid in uids:
            uid_bound_limits = list(filter(lambda limit: limit.user and limit.user.uid == uid, bound_limits))

            if uid_bound_limits and uid_bound_limits[0] != None:
                min_limit = uid_bound_limits[0].limit_item.max_value

                for limit in uid_bound_limits:
                    if limit.limit_item.max_value < min_limit:
                        min_limit = limit.limit_item.max_value

                limits[uid] = min_limit

        # calculating default limits

        default_bound_limits = list(filter(lambda rule: rule.is_default == True, bound_limits))

        default_limit

        if default_bound_limits and default_bound_limits[0] != None:
            min_limit = uid_bound_limits[0].limit_item.max_value

            for limit in default_bound_limits:
                if limit.limit_item.max_value < min_limit:
                    min_limit = limit.limit_item.max_value

            default_limit = min_limit

    def __calculate_used_node_hours(self, uid):
        try:
            past_jobs = list(
                self.__session
                    .query(Job)
                    .filter(or_(User.uid == uid, BoundLimit.is_default == true()))
            )
        except:
            print('Error while processing database query. Please try again later.')