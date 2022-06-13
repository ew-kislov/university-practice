import socket
import os
import time
import select
from datetime import datetime

from .SlurmService import SlurmService
from .EEvent import EEvent
from .Event import Event


class SlurmWiki2Service(SlurmService):
    def __init__(self):
        self.__subscribers = []

        self.__host = os.getenv("WIKI2_HOST")
        self.__wiki2_port = os.getenv("WIKI2_PORT")
        self.__port_for_wiki2 = os.getenv("PORT_FOR_WIKI2")

        self.__latest_jobs = self.__get_jobs()
        self.__latest_nodes = self.__get_nodes()

        self.__init_connection()

        self.__state_last_updated = datetime.now()
        self.__last_event_time = datetime.now()

        print("SLURM service(wiki2): initialized")

    def __init_connection(self):
        self.__socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.__socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.__socket.bind((self.__host, int(self.__port_for_wiki2)))
        self.__socket.listen(10)

    def set_subscriber(self, subscriber):
        self.__subscribers.append(subscriber)

    def run(self):
        print("SLURM service(wiki2): running")

        read_list = [self.__socket]

        while True:
            readable, writable, errored = select.select(read_list, [], [], 0)
            
            processes_connections = 0

            for s in readable:
                if s is self.__socket:
                    client_socket, address = self.__socket.accept()
                    read_list.append(client_socket)
                else:
                    data_raw = s.recv(4)
                    if data_raw:
                        data = data_raw.decode()
                        
                        if data.strip() == '1234':
                            print("Slurm service(wiki2): job status changed")

                            self.__latest_nodes = self.__get_nodes()

                            new_jobs = self.__get_jobs()

                            # empty incoming jobs almost always mean that slurm wiki2 spawned a bug 
                            if (len(new_jobs) == 0):
                                continue

                            queued_jobs = self.__get_queued_jobs(new_jobs)
                            changed_status_jobs = self.__get_changed_status_jobs(
                                new_jobs)

                            for job in queued_jobs:
                                for subscriber in self.__subscribers:
                                    subscriber.handle_event(Event(EEvent.JobQueued, job))

                            for job in changed_status_jobs:
                                if job["STATE"] == "Completed":
                                    for subscriber in self.__subscribers:
                                        subscriber.handle_event(Event(EEvent.JobDone, job['id']))

                            self.__last_event_time = datetime.now()

                            self.__latest_jobs = new_jobs

                            processes_connections += 1
                        
                    else:
                        s.close()
                        read_list.remove(s)

            if processes_connections == 0:
                if (datetime.now() - self.__state_last_updated).seconds > 10:
                    self.__latest_nodes = self.__get_nodes()

                    for subscriber in self.__subscribers:
                        subscriber.handle_event(Event(EEvent.Ping, None))

                    new_jobs = self.__get_jobs()
                    queued_jobs = self.__get_queued_jobs(new_jobs)

                    for job in queued_jobs:
                        for subscriber in self.__subscribers:
                            subscriber.handle_event(Event(EEvent.JobQueued, job))

                    self.__latest_jobs = new_jobs

                    self.__state_last_updated = datetime.now()


    def start_job(self, job):
        id = job['id']

        print(f"Slurm service(wiki2): starting job #{id}(nodes - {job['TASKS']})")

        client_fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_fd.connect((self.__host, int(self.__wiki2_port)))

        node_ids = list(map(lambda x: x['id'], self.__latest_nodes))

        nodes_string = ','.join(node_ids[0:int(job['TASKS'])])

        command = f'AUTH=slurm DT=SC=-300 TS={round(time.time())} CMD=STARTJOB ARG={id} TASKLIST={nodes_string}'
        header = str(len(command) - 1).zfill(8)

        client_fd.send(str.encode(header))
        client_fd.send(str.encode(command))

        header = int(client_fd.recv(8).decode())
        response = client_fd.recv(header).decode()

        client_fd.close()

        self.__latest_nodes = self.__latest_nodes[int(job['TASKS']):]

    def get_idle_nodes_count(self):
        return len(self.__latest_nodes)

    def get_total_jobs(self):
        count = 0

        for job in self.__latest_jobs:
            if ("STATE" in job and job["STATE"] == "Idle"):
                count += 1

        return count

    def get_jobs_per_partition(self):
        jobs_per_partition = dict()

        for job in self.__latest_jobs:
            if ("STATE" in job and job["STATE"] == "Idle"):
                if "RCLASS" not in job:
                    continue
                
                if job["RCLASS"] not in jobs_per_partition:
                    jobs_per_partition[job["RCLASS"]] = 1
                else:
                    jobs_per_partition[
                        job["RCLASS"]] = jobs_per_partition[job["RCLASS"]] + 1

        return jobs_per_partition

    def __get_queued_jobs(self, new_jobs):
        queued_jobs = []

        for job in new_jobs:
            latest_job = next(
                (x for x in self.__latest_jobs if x["id"] == job["id"]), None)
            if not latest_job and "STATE" in job and job["STATE"] == "Idle":
                queued_jobs.append(job)

        return queued_jobs

    def __get_changed_status_jobs(self, new_jobs):
        changed_status_jobs = []

        for job in new_jobs:
            latest_job = next(
                (x for x in self.__latest_jobs if x["id"] == job["id"]), None)
            if latest_job and "STATE" in job and "STATE" in latest_job and job["STATE"] != latest_job["STATE"]:
                changed_status_jobs.append(job)

        return changed_status_jobs

    def __get_nodes(self):
        client_fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_fd.connect((self.__host, int(self.__wiki2_port)))

        command = f'AUTH=slurm DT=SC=-300 TS={round(time.time())} CMD=GETNODES ARG=0:ALL\n'
        header = str(len(command) - 1).zfill(8)

        client_fd.send(str.encode(header))
        client_fd.send(str.encode(command))

        header = int(client_fd.recv(8).decode())
        nodesRaw = client_fd.recv(header).decode()

        nodes = self.__parse_response(nodesRaw)

        client_fd.close()

        return list(filter(lambda node: node["STATE"] == "Idle", nodes))

    def __get_jobs(self):
        client_fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_fd.connect((self.__host, int(self.__wiki2_port)))

        command = f'AUTH=slurm DT=SC=-300 TS={round(time.time())} CMD=GETJOBS ARG=0:ALL\n'
        header = str(len(command) - 1).zfill(8)

        client_fd.send(str.encode(header))
        client_fd.send(str.encode(command))

        header = int(client_fd.recv(8).decode())
        jobsRaw = client_fd.recv(header).decode()

        jobs = self.__parse_response(jobsRaw)

        client_fd.close()

        return jobs

    def __parse_response(self, jobsRaw):
        jobsRawList = jobsRaw.split('#')

        jobsRawList.pop(0)

        jobs = []

        for jobRaw in jobsRawList:
            pairs = jobRaw[(jobRaw.find(':') + 1):].split(';')
            job = dict()
            job['id'] = jobRaw[0:(jobRaw.find(':'))]
            for pair in pairs:
                pairArr = pair.split('=')
                if len(pairArr) != 2:
                    continue
                job[pairArr[0]] = pairArr[1]
            jobs.append(job)

        return jobs