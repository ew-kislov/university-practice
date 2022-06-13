from sqlalchemy import Column, Integer, DateTime
from sqlalchemy.orm import relationship

from ..config.DbConfig import DbConfig

db_config = DbConfig()

class Job(db_config.Base):
    __tablename__ = 'job'

    id = Column(Integer, primary_key=True)
    uid = Column(Integer, nullable=False)
    node_hours = Column(Integer, nullable=False)
    creation_date = Column(DateTime, nullable=False)