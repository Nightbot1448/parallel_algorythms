import matplotlib.pyplot as plt

process = []
time_consist = []
time_parallel = []


with open('lab4_statistic.txt') as stat:
	for line in stat:
		res = line.strip().split()
		process.append(int(res[0]))
		time_consist.append(float(res[1]))
		time_parallel.append(float(res[2]))

# print(diff)


# plt.semilogx(process, time_consist, 'bo')
plt.loglog(process, time_consist, 'bo')
# plt.semilogx(process, time_consist, 'b')
plt.loglog(process, time_consist, 'b')
# plt.semilogx(process, time_parallel, 'ro')
plt.loglog(process, time_parallel, 'ro')
# plt.semilogx(process, time_parallel, 'r')
plt.loglog(process, time_parallel, 'r')
plt.xticks(process)
plt.title('time')
plt.grid(True)

# plt.subplot(122)
# plt.semilogx(process, diff, 'bo')
# plt.semilogx(process, diff, 'r')
# plt.xticks(process)
# plt.title('diff')
# plt.grid(True)

plt.show()


# fig = plt.figure()
# ax1 = fig.add_subplot(121)
# ax2 = fig.add_subplot(122)
# # ax1.plot([1,2,3], [1,2,3])
# ax2.plot([1,2,3], [3,2,1])

# ax1.plot(process, time, 'bo')
# ax1.plot(process, time, 'r')

# ax2.plot(process, diff, 'bo')
# ax2.plot(process, diff, 'r')

# plt.grid(True)
# plt.show()