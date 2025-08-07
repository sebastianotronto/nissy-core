# Small example of nissy_python_module usage
# See the solve.py example for more details on how this works

from sys import path
path.append("./")
import nissy_python_module as nissy

moves = "R U' Bw2 M D' x' F B(E2 F D B' Lw2 U2 U' S2 B)"

print("Changing the last quarter turns of {} gives:".format(moves))
for s in nissy.variations(moves, "lastqt"):
	print(s)
