import time
import sys

is_default = (len(sys.argv) == 1)
default_sleep = 5
actual_sleep = default_sleep if is_default else int(sys.argv[1])

time.sleep(actual_sleep)

f = open('./out.txt', 'a+')
f.write('Job output. Slept ' + str(actual_sleep) + ' seconds \n')
f.close()