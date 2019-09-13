import numpy as np
import matplotlib.pyplot as plt
import csv
import string
import seaborn as sns
import math

if __name__ == '__main__':
	print("hello")
	f = open('data_step1_observations.txt')
	lines = f.readlines()

	rssi = []
	dist = []
	for line in lines:
		line = line.strip().split()
		rssi.append(float(line[3]))
		dist.append(float(line[6]))
		#print(line)

	# shadowing model
	size = 15
	P_0 = -64.0432
	gamma = 1.30937
	rssi_m_list = []
	dist_m_list = []
	for ID in range(size):
		width = (30.-1.)/size
		dist_m = 1 + width*float(ID)
		rssi_m = P_0 - 10.*gamma*(math.log(dist_m/1.,10.))
		print("{} {}".format(dist_m,rssi_m))
		rssi_m_list.append(rssi_m)
		dist_m_list.append(dist_m)

	#
	# plot: rssi
	#
	color = ['tab:blue', 'tab:orange', 'tab:green']
	fig = plt.figure(figsize=(6,5))
	plt.scatter(dist, rssi, marker='s',s=40,c=color[0],alpha=0.1,edgecolors='none',label='Target')
	plt.plot(dist_m_list, rssi_m_list,marker='o', c='k', lw=2, mec='m', mfc='w',label='Shadowing Model')
	plt.legend(frameon=True)
	plt.xlabel('Distance / m',fontdict={'family' : 'Times New Roman', 'size': 12})
	plt.ylabel("RSSI",fontdict={'family' : 'Times New Roman', 'size': 12})
	plt.xlim(-5,35)
	plt.ylim(-90,-50)
	plt.title('RSSI Distribution and Fitting Result')
	plt.savefig('figure-RSSI-Distance_fitting.png',dpi=300)

	plt.show()
