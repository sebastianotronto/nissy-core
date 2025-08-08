# Small example of nissy_python_module usage

# Compile the python module with "make python", then run this from the main
# folder containing nissy_python_module.so

# Append the main folder to the python path so we can load the module
import sys, os
sys.path.append(os.getcwd())

# Import with a nicer name
import nissy_python_module as nissy

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
