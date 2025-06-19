import matplotlib.pyplot as plt

h48 = [
	("H48 h11 k2", 56.1,
		[0.23, 1.15, 5.08, 31.30],
		[0.06, 0.31, 1.31, 7.96],
		[0.02, 0.1, 0.43, 2.48]
	),
	("H48 h10 k2", 28.1,
		[0.34, 1.80, 7.77],
		[0.1, 0.47, 2.00, 13.54],
		[0.03, 0.16, 0.74, 4.43]
	),
	("H48 h9 k2", 14.1,
		[0.42, 2.84, 12.86],
		[0.14, 0.83, 3.82, 25.98],
		[0.04, 0.26, 1.18, 8.31]
	),
	("H48 h8 k2", 7.1,
		[0.86, 6.66, 27.4],
		[0.27, 2.02, 7.94],
		[0.08, 0.6, 2.48]
	),
	("H48 h7 k2", 3.6,
		[1.47, 8.9, 42.46],
		[0.35, 2.59, 12.41],
		[0.11, 0.81, 3.91]
	),
	("H48 h6 k2", 1.8,
		[2.28, 16.89],
		[0.65, 4.79, 23.91],
		[0.21, 1.53, 7.82]
	)
]

vcube = [
	("vcube 404", 31.8,
		[0.3, 1.25, 6.87, 57.49],
		[0.1, 0.38, 1.88, 16.98],
		[0.03, 0.16, 0.67, 6.36]
	),
	("vcube 308", 21.2,
		[0.2, 1.11, 6.92],
		[0.06, 0.42, 1.95, 17.73],
		[0.04, 0.26, 1.18, 9.53]
	),
	("vcube 208", 7.3,
		[0.57, 4.41, 20.75],
		[0.17, 1.49, 5.88],
		[0.08, 0.8, 2.38]
	),
	("vcube 112", 2.4,
		[1.01, 9.39],
		[0.29, 3.15, 12.06],
		[0.15, 1.66, 5.18]
	)
]

h48_x = [i[1] for i in h48]
vcube_x = [i[1] for i in vcube]

def getarr(m, t, a):
	return [i[2+t][m-17] for i in a if len(i[2+t]) > m-17]

def gethv(m, t):
	return getarr(m, t, h48), getarr(m, t, vcube)

def showplt(plt, title, h48y, vcubey):
	plt.clf()
	plt.title(title)
	plt.xlabel("Table size (GiB)")
	plt.ylabel("Time to solve (s / cube)")
	plt.plot(h48_x[:len(h48y)], h48y, "o--", label = "H48")
	plt.plot(vcube_x[:len(vcubey)], vcubey, "o--", label = "vcube")
	plt.legend(loc = "right")
	filename = title.replace(" ", "").replace(",", "") + ".png"
	plt.savefig("benchmarks/img/" + filename, dpi=300)
	#plt.show()

for i in range(17, 21):
	for j in range(0, 3):
		title = "{} moves, {} thread{}".format(
			i, 4**j, "s" if j > 0 else "")
		h, v = gethv(i, j)
		showplt(plt, title, h, v)
