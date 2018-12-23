import matplotlib.pyplot as plt

threads = []
reduction = [[],[]]
without = [[],[]]


with open('lab5_statistic.txt') as stat:
	for line in stat:
		res = line.strip().split()
		threads.append(int(res[0]))
		reduction[0].append(float(res[1]))
		reduction[1].append(float(res[2]))
		without[0].append(float(res[3]))
		without[1].append(float(res[4]))

# print(diff)


# plt.semilogx(threads, time_consist, 'bo')
plt.subplot(221)
plt.plot(threads,reduction[1], 'bo')
plt.plot(threads,reduction[1], 'b')
plt.xticks(threads)
plt.grid(True)
plt.title('reduction time')

plt.subplot(222)
plt.plot(threads, without[1], 'ro')
w_line, = plt.plot(threads, without[1], 'r')
plt.title('without reduction time')
plt.xticks(threads)
plt.grid(True)

plt.subplot(223)
plt.plot(threads,reduction[0], 'bo')
plt.plot(threads,reduction[0], 'b')
plt.title('reduction diff')
plt.xticks(threads)
plt.grid(True)

plt.subplot(224)
plt.plot(threads, without[0], 'ro')
w_line, = plt.plot(threads, without[0], 'r')
plt.title('without reduction diff')
plt.xticks(threads)
plt.grid(True)

plt.show()


# fig = plt.figure()
# ax1 = fig.add_subplot(121)
# ax2 = fig.add_subplot(122)
# # ax1.plot([1,2,3], [1,2,3])
# ax2.plot([1,2,3], [3,2,1])

# ax1.plot(threads, time, 'bo')
# ax1.plot(threads, time, 'r')

# ax2.plot(threads, diff, 'bo')
# ax2.plot(threads, diff, 'r')

# plt.grid(True)
# plt.show()