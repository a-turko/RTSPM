import sys
import subprocess
import matplotlib.pyplot as plt

# runs the script with arguments and returns stdout
def runProg(prog, args):
	res = subprocess.run([prog] + args, capture_output=True, text = True)
	return res.stdout


def runTime(prog, test):
	line =  runProg('./runtime.sh', [prog, test])
	tokens = line.rstrip().rstrip('s').split('m')
	return float(tokens[0])*60.0 + float(tokens[1])

def runMem(prog, test):
	line = runProg('./runmem.sh', [prog + '-mem', test])
	tokens = line.split()

	val = int(tokens[1])
	if tokens[2] == 'mB':
		val = val * 1024
	
	return val

def runAns(prog, test, additional = []):
	output = runProg('./run.sh', [prog, test] + additional)
	return list(map(int, output.split()))


class Test(object):
	def __init__(self, t, n, m):
		self.t = t
		self.n = n
		self.m = m
	
	def __enter__(self):
		runProg('./gentest.sh', [str(self.t), str(self.n), str(self.m), 'curr_test.in'])
	
	def __exit__(self, type, value, traceback):
		runProg('rm', ['curr_test.in'])

# return the number of false positives
# if a false negative was found, return -1
def errors(occKmp, occHash):

	k = 0
	h = 0
	while (k < len(occKmp) and h < len(occHash)):
		if occHash[h] == occKmp[k]:
			h+=1
			k+=1
		elif occHash[h] < occKmp[k]:
			h+=1
		else:
			# a false negative
			return -1
	
	if (k < len(occKmp)):
		return -1
	
	return len(occHash) - len(occKmp)

# run a test
def runTest(t, n, m):

	with Test(t, n, m):
		kmp = runAns('kmp', 'curr_test.in')
		rtspm = runAns('rtspm', 'curr_test.in')

	return errors(kmp, rtspm)

def checkMem(textLen, step):
	
	m = step
	K = []
	R = []
	M = []

	while m <= textLen:
		with Test(1, textLen, m):
			kmp = runMem('kmp', 'curr_test.in')
			rtspm = runMem('rtspm', 'curr_test.in')
		
		K.append(kmp)
		R.append(rtspm)
		M.append(m)
		print(m, kmp, rtspm, file=sys.stderr)
		m += (m+1)//2

	#print(K)
	#print(R)

	plt.plot(M, R, 'r-', label='Streaming model')
	plt.plot(M, K, 'b-', label='KMP')
	plt.legend()

	plt.suptitle('Memory usage for a fixed text length of {}'.format(textLen))
	plt.ylabel('virtual memory peak in kB')
	plt.xlabel('length of the pattern')
	
	fname = 'memory_plot'
	if len(sys.argv) > 4:
		fname = sys.argv[4]

	plt.savefig(fname + '.png')




def checkTime(textLen, step):

	m = step
	K = []
	R = []
	M = []

	while m <= textLen:
		with Test(1, textLen, m):
			kmp = runTime('kmp', 'curr_test.in')
			rtspm = runTime('rtspm', 'curr_test.in')
		
		K.append(kmp)
		R.append(rtspm)
		M.append(m)
		print(m, kmp, rtspm, file=sys.stderr)
		m += (m+1)//2

	#print(K)
	#print(R)

	plt.plot(M, K, 'b-', label='KMP')
	plt.plot(M, R, 'r-', label='Streaming model')
	plt.legend()
	plt.legend()

	plt.suptitle('Time consumption for a fixed text length of {}'.format(textLen))
	plt.ylabel('time consumption in seconds')
	plt.xlabel('length of the pattern')

	fname = 'time_plot'
	if len(sys.argv) > 4:
		fname = sys.argv[4]

	plt.savefig(fname + '.png', format='png')


# check the probability of error on a single test: test.in
def checkErrors(modulo):

	errcnt = 0
	kmp = runAns('kmp', 'test.in')
	for base in range(0, modulo):
		hash = runAns('rtspm', 'test.in', [str(modulo), str(base)])

		status = errors(kmp, hash)
		assert(status >= 0)
		if status > 0:
			errcnt += 1

	return errcnt

# run a series of tests to ensure correctness of rtspm
def checkCorrectness():
	errcnt = 0
	testcnt = 0
	types = [0, 1, 2, 3, 5, 10, 25]
	sizes = [[1, 1], [2, 1], [1, 2], [5, 1], [1000, 10], \
			[1000, 1000], [100000, 200], [100000, 500], \
			[1000000, 30], [1241242, 1234], [1452421, 142304]]


	for t in types:
		for n, m in sizes:
			
			testcnt += 1
			status = runTest(t, n, m)

			if (status > 0):
				print('Failed test: gen', t, n, m)
				errcnt += 1
	
	for n in range(0, 14):
		for m in range(n,14):
			
			testcnt +=1
			status = runTest(-1, n, m)
			if (status > 0):
				print('Failed test: gen', t, n, m)
				errcnt += 1


	print('False positives found in ', errcnt, '/', testcnt, ' tests.')



if sys.argv[1] == "memory":
	checkMem(int(sys.argv[2]), int(sys.argv[3]))
elif sys.argv[1] == "time":
	checkTime(int(sys.argv[2]), int(sys.argv[3]))
elif sys.argv[1] == "correctness":
	checkCorrectness()
elif sys.argv[1] == "probability":
	print('False positives for ', checkErrors(int(sys.argv[2])), ' bases of fingerprints.')
