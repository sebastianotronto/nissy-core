# This example is meant to test the pause / resume / stop mechanism
# See the solve.py example for more details on how this works

import sys, os, time

sys.path.append(os.getcwd())
sys.path.append(os.getcwd() + os.path.sep + "python")
import nissy

# Function used to pause, resume and kill the solver
t0 = time.time()
def poll():
	t = time.time() - t0
	if t < 3:
		print("[{}s] pausing for the first 3 seconds".format(t))
		return nissy.status_pause
	if t < 6:
		print("[{}s] Letting the solver run for 3 seconds".format(t))
		return nissy.status_run
	print("[{}s] Too much time has passed, giving up".format(t))
	return nissy.status_stop

solver = "h48h0k4"
datapath = "tables" + os.path.sep + solver
if os.path.exists(datapath):
	data = bytearray(open(datapath, "rb").read())
else:
	data = nissy.gendata("h48h0k4")
	print("Generated data will NOT be persisted")

# Scramble is 19 moves optimal, should take a while with the h48h0k4 solver
scramble = "R' U' F D R F2 D L F D2 F2 L' U R' L2 D' R2 F2 R2 D L2 U2 R' U' F"
cube = nissy.applymoves(nissy.solved_cube, scramble);

nissy.solve(cube, solver, nissy.nissflag_normal, 0, 9, 3, 20, 4, data, poll)
