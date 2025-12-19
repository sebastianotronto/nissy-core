import matplotlib.pyplot as plt
import results_h48
import results_vcube

# Table sizes in bytes

sizes_h48 = {
	6: 1897951528,
	7: 3793842344,
	8: 7585624040,
	9: 15169187432,
	10: 30336314216,
	11: 60670567784,
}

sizes_vcube = {
	112: 2603089920,
	208: 7809269760,
	308: 22777036800,
	404: 34165555200,
	212: 62474158080,
}

# Printing tables in markdown format

def print_row(solver_name, solver_size, dict, mul_by_size, superflip_star):
	if dict is None:
		return
	solver_gib = solver_size / (2**30)
	m = solver_gib if mul_by_size else 1
	s = "   (a)   " if superflip_star else "         "
	cols = [f"{solver_name: <10}", f"{solver_gib:>4.1f} GiB"] + [
		f"{dict[key]*m/25:>8.2f}" if key in dict else "        "
		for key in [17, 18, 19, 20]
	] + [
		f"{dict["superflip"]*m:>9.2f}" if "superflip" in dict else s
	]
	sep = "|"
	print(sep + sep.join(cols) + sep)

def print_table(h48, vcube, ms, st):
	vcube = vcube or {}
	print("|  Solver  |  Size  |17 moves|18 moves|19 moves|20 moves|Superflip|")
	print("|:---------|:-------|-------:|-------:|-------:|-------:|--------:|")
	print_row("vcube 212", sizes_vcube[212], vcube.get(212), ms, not st)
	print_row("H48 h11", sizes_h48[11], h48[11], ms, False)
	print_row("vcube 404", sizes_vcube[404], vcube.get(404), ms, not st)
	print_row("H48 h10", sizes_h48[10], h48[10], ms, False)
	print_row("vcube 308", sizes_vcube[308], vcube.get(308), ms, not st)
	print_row("H48 h9", sizes_h48[9], h48[9], ms, False)
	print_row("vcube 208", sizes_vcube[208], vcube.get(208), ms, not st)
	print_row("H48 h8", sizes_h48[8], h48[8], ms, False)
	print_row("H48 h7", sizes_h48[7], h48[7], ms, False)
	print_row("vcube 112", sizes_vcube[112], vcube.get(112), ms, not st)
	print_row("H48 h6", sizes_h48[6], h48[6], ms, False)

print("<details><summary>Single solution, single thread</summary>")
print()
print("Time per cube (in seconds, lower is better).")
print()
print_table(results_h48.h48_single_thread, results_vcube.vcube_single_thread, False, True)
print()
print("Time per cube adjusted for table size (in seconds \\* GiB, lower is better).")
print()
print_table(results_h48.h48_single_thread, results_vcube.vcube_single_thread, True, True)
print()
print(
	'<img src="img/17moves1thread.png">\n'
	'<img src="img/18moves1thread.png">\n'
	'<img src="img/19moves1thread.png">\n'
	'<img src="img/20moves1thread.png">'
)
print("</details>")

print()
print("<details><summary>Single solution, 4 threads</summary>")
print()
print("Time per cube (in seconds, lower is better).")
print()
print_table(results_h48.h48_4_threads, results_vcube.vcube_4_threads, False, False)
print()
print("Time per cube adjusted for table size (in seconds \\* GiB, lower is better).")
print()
print_table(results_h48.h48_4_threads, results_vcube.vcube_4_threads, True, False)
print()
print("(a) vcube cannot parallelize on a single scramble, the results for the")
print("superflip are going to be the same as in the single thread case.")
print()
print(
	'<img src="img/17moves4threads.png">\n'
	'<img src="img/18moves4threads.png">\n'
	'<img src="img/19moves4threads.png">\n'
	'<img src="img/20moves4threads.png">'
)
print("</details>")

print()
print("<details><summary>Single solution, 16 threads</summary>")
print()
print("Time per cube (in seconds, lower is better).")
print()
print_table(results_h48.h48_16_threads, results_vcube.vcube_16_threads, False, False)
print()
print("Time per cube adjusted for table size (in seconds \\* GiB, lower is better).")
print()
print_table(results_h48.h48_16_threads, results_vcube.vcube_16_threads, True, False)
print()
print("(a) vcube cannot parallelize on a single scramble, the results for the")
print("superflip are going to be the same as in the single thread case.")
print()
print(
	'<img src="img/17moves16threads.png">\n'
	'<img src="img/18moves16threads.png">\n'
	'<img src="img/19moves16threads.png">\n'
	'<img src="img/20moves16threads.png">'
)
print("</details>")

print()
print("<details><summary>All solutions, 16 threads</summary>")
print()
print("*Note: vcube does not have an option for finding multiple solutions.*")
print()
print("Time per cube (in seconds, lower is better).")
print()
print_table(results_h48.h48_all_solutions, None, False, False)
print()
print("Time per cube adjusted for table size (in seconds \\* GiB, lower is better).")
print()
print_table(results_h48.h48_all_solutions, None, True, False)
print()
print("</details>")

# Plotting

def plot(title, hd, vd, key):
	d = 1 if key == "superflip" else 25
	h48x = [sizes_h48[m]/(2**30) for m in hd.keys() if key in hd[m]]
	vcubex = [sizes_vcube[m]/(2**30) for m in vd.keys() if key in vd[m]]
	h48y = [d[key] for _, d in hd.items() if key in d]
	vcubey = [d[key] for _, d in vd.items() if key in d]

	plt.clf()
	plt.title(title)
	plt.xlabel("Table size (GiB)")
	plt.ylabel("Time to solve (s / cube)")
	plt.plot(h48x, h48y, "o--", label = "H48")
	plt.plot(vcubex, vcubey, "o--", label = "vcube")
	plt.legend(loc = "right")
	filename = title.replace(" ", "").replace(",", "") + ".png"
	plt.savefig("benchmarks/img/" + filename, dpi=300)
	#plt.show()

rh, rv = results_h48.h48_single_thread, results_vcube.vcube_single_thread
for m in [17, 18, 19, 20]:
	plot(f"{m} moves 1 thread", rh, rv, m)

rh, rv = results_h48.h48_4_threads, results_vcube.vcube_4_threads
for m in [17, 18, 19, 20]:
	plot(f"{m} moves 4 threads", rh, rv, m)

rh, rv = results_h48.h48_16_threads, results_vcube.vcube_16_threads
for m in [17, 18, 19, 20]:
	plot(f"{m} moves 16 threads", rh, rv, m)
