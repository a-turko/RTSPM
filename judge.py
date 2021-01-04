import sys
import subprocess

# runs the script with arguments and returns stdout
def runProg(prog, args):
	res = subprocess.run([prog] + args, capture_output=True, text = True)
	return res.stdout


def runTime(prog, test):
	line =  runProg('./runtime.sh', [prog, test])
	tokens = line.rstrip('s').split('m')
	return float(tokens[0])*60.0 + float(tokens[1])

def runMem(prog, test):
	line = runProg('./runmem.sh', [prog + '-mem', test])
	tokens = line.split()

	val = int(tokens[2])
	if tokens[3] == 'kB':
		val = val * 1024
	if tokens[4] == 'mB':
		val = val * 1024 * 1024
	
	return val

def runAns(prog, test, additional = []):
	output = runProg('./run.sh', [prog, test] + additional)
	return list(map(int, output.split()))


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


def checkMem(textLen, step):
	
	return


def checkTime(textLen, step):

	return




# check the probability of error on a single test: test.in
def checkErrors(modulo):

	errcnt = 0
	kmp = runAns('kmp', 'test.in')
	for base in range(1, modulo):
		hash = runAns('rtspm', 'test.in', [str(modulo), str(base)])

		status = errors(kmp, hash)
		assert(status >= 0)
		if status > 0:
			errcnt += 1

	return errcnt

# run a series of tests to ensure correctness of rtspm
def checkCorrectness():

	return



if sys.argv[1] == "memory":
	checkMem()
elif sys.argv[1] == "time":
	checkTime()
elif sys.argv[1] == "correctness":
	checkCorrectness()
elif sys.argv[1] == "probability":
	print(checkErrors(int(sys.argv[2])))
