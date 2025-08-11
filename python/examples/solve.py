# Small example of nissy Python module usage

# Run "./build python", then run this from either the top-level directory
# of the nissy-core repo or from the python subdirectory.

# Append the directories to the python path so we can load the module
import sys, os
sys.path.append(os.getcwd())
sys.path.append(os.getcwd() + os.path.sep + "python")

# Import with a nicer name
import nissy

# Choose the solver you prefer
solver = "h48h0k4"

# Load the pruning table from file
data = bytearray(open("tables/" + solver, "rb").read())

# If you have not generated the table yet, you can do so:
# data = nissy.gendata("h48h0k4")

# Get a scrambled cube
cube = nissy.applymoves(nissy.solved_cube, "U F R2");

# Solve!
solutions = nissy.solve(cube, solver, nissy.nissflag_normal, 0, 9, 3, 20, 4, data)

# Print the solutions, one per line
print("Found ", len(solutions), " solutions:")
for s in solutions:
	print(s)

# You can use help(nissy) for more info about the available methods:
# help(nissy)

# Or help(nissy.methodname) if you want to know more about a specific method:
# help(nissy.solve)
