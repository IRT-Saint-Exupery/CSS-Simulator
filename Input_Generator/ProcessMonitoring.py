from subprocess import Popen, PIPE

def MonitorProcess():

	pipe = Popen("ps aux | grep core-cpu1 | grep -v \"grep\"", shell=True, stdout=PIPE).stdout
	try:
		out = pipe.readlines()[0].decode()
	except:
		print("SAT NOT LAUNCHED !")
		return -1
	else:
		pid_to_monitor = out.split()[1]
		return pid_to_monitor
		
def CheckProcessIsAlive(pid):
	
	pipe = Popen("ps --pid "+str(pid), shell=True, stdout=PIPE).stdout
	try:
		out = pipe.readlines()[1].decode()
		return True
	except:
		print("cFS DEAD !")
		return False

# utiliser le SIGSEGV avec valgrind pour récup une segfault ?
