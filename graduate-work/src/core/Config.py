import json

class Config:
    def init(self):
        file = open('config.json')

        config_raw = json.load(file)

        self.__total_jobs_limit = config_raw["totalJobsLimit"]
        self.__jobs_per_queue_limit = config_raw["jobsPerQueueLimit"]

    def get_total_jobs_limit(self):
        return self.__total_jobs_limit

    def get_jobs_per_queue_limit(self):
        return self.__jobs_per_queue_limit