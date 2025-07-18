# Small example of nissy_python_module usage
# See the solve.py example for more details on how this works

from sys import path
path.append("./")
import nissy_python_module as nissy

cube = nissy.applymoves(nissy.solved_cube, "R' U' F");
print("Cube after the moves R' U' F:")
print(cube)
