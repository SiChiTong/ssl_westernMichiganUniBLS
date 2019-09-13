import numpy as np
import matplotlib.pyplot as plt
import csv
import string
import seaborn as sns

def GetLocationLabel(anchorName):
	if anchorName=='b3001': return 'F09'
	if anchorName=='b3002': return 'J04'
	if anchorName=='b3003': return 'N04'
	if anchorName=='b3004': return 'S04'
	if anchorName=='b3005': return 'J07'
	if anchorName=='b3006': return 'N07'
	if anchorName=='b3007': return 'S07'
	if anchorName=='b3008': return 'J10'
	if anchorName=='b3009': return 'D15'
	if anchorName=='b3010': return 'J15'
	if anchorName=='b3011': return 'N15'
	if anchorName=='b3012': return 'R15'
	if anchorName=='b3013': return 'W15'

	return 'NONE'

def GetLocation(label):
	Alphabet = ['A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X']
	HorID = Alphabet.index(label[0])
	VerID = 18 - (int(label[1])*10 + int(label[2]))
	
	# x (Horrizontal) : binWidth 3.05 m, min = 0m
	# y (Vertical)    : binWidth 3.05 m, min = 0m
	# center of A18 being the origin of the coordinate system
	width = 3.048 # unit : m

	x = 0. + float(HorID)*width
	y = 0. + float(VerID)*width

	print("Label {} : ID {}, {}, Loc {}, {}".format(label, HorID, VerID,x,y))
	return x, y

def GetRSSIData(filename):
	print("data file path : {}".format(filename))

	anchorNameList = []
	anchorNameList.append('b3001')
	anchorNameList.append('b3002')
	anchorNameList.append('b3003')
	anchorNameList.append('b3004')
	anchorNameList.append('b3005')
	anchorNameList.append('b3006')
	anchorNameList.append('b3007')
	anchorNameList.append('b3008')
	anchorNameList.append('b3009')
	anchorNameList.append('b3010')
	anchorNameList.append('b3011')
	anchorNameList.append('b3012')
	anchorNameList.append('b3013')

	data = {}
	with open(filename, 'rb') as sourceFile:
		reader = csv.DictReader( sourceFile )

		ID_t = 0
		for line in reader:
			# debug
			#if ID_t==5:
			#	break

			locLabel = line['location'] #location
			date = line['date'] # tiem stamp
			tarx, tary = GetLocation(locLabel)

			# anchor
			for anchorName in anchorNameList:
				dataALine = []
				dataALine.append(locLabel)
				dataALine.append(tarx)
				dataALine.append(tary)

				rssi = float(line[anchorName]) # rssi
				dataALine.append(rssi)

				ancx, ancy = GetLocation(GetLocationLabel(anchorName))
				dataALine.append(ancx)
				dataALine.append(ancy)

				d = 0
				d = d + (tarx-ancx)**2
				d = d + (tary-ancy)**2
				d = d**0.5
				dataALine.append(d)

				data[ID_t] = dataALine
				ID_t += 1
	#print(data)

	# write file
	with open ('data_step1_observations.txt','w')as g:
		for ID in range(ID_t):
			line = ''
			line = line + str(data[ID][0]) + ' ' 
			line = line + str(data[ID][1]) + ' '
			line = line + str(data[ID][2]) + ' '
			line = line + str(data[ID][3]) + ' '
			line = line + str(data[ID][4]) + ' '
			line = line + str(data[ID][5]) + ' '
			line = line + str(data[ID][6]) + '\n'
			print("ID {}: {}".format(ID,line))
			g.write(line)
		g.close()

	return data

def Plot_location(data):

	tarx = [ele[1][1] for ele in data.items()]
	tary = [ele[1][2] for ele in data.items()]
	rssi = [ele[1][3] for ele in data.items()]
	ancx = [ele[1][4] for ele in data.items()]
	ancy = [ele[1][5] for ele in data.items()]
	dist = [ele[1][6] for ele in data.items()]

#	dist = []
#	for ID in range(len(tarx)):
#		d = (tarx[ID]-ancx[ID])**2 + (tary[ID]-ancy[ID])**2
#		d = d**0.5
#		dist.append(d)

	print("Anchor b3002, (JO4): {}, {}".format(ancx,ancy))

	color = ['tab:blue', 'tab:orange', 'tab:green']
	#
	# target locations
	#
	fig = plt.figure(figsize=(7,6))
	plt.scatter(tarx, tary, marker='s',s=60,c=color[0],alpha=0.1,edgecolors='none',label='Target')
	plt.legend(frameon=True)
	plt.xlabel('X (Horizontal) / m',fontdict={'family' : 'Times New Roman', 'size': 12})
	plt.ylabel("Y (Vertical) / m",fontdict={'family' : 'Times New Roman', 'size': 12})
	#plt.xlim(-105,170)
	#plt.ylim(-110,110)
	plt.title('Target Distribution')
	plt.savefig('figure-TargetDistribution.png',dpi=300)

	#
	# rssi
	#
	fig = plt.figure(figsize=(7,6))
	plt.scatter(dist, rssi, marker='s',s=60,c=color[0],alpha=0.1,edgecolors='none',label='Target')
	plt.legend(frameon=True)
	plt.xlabel('Distance / m',fontdict={'family' : 'Times New Roman', 'size': 12})
	plt.ylabel("RSSI",fontdict={'family' : 'Times New Roman', 'size': 12})
	plt.xlim(-5,40)
	plt.ylim(-100,-50)
	plt.title('RSSI Distribution')
	plt.savefig('figure-RSSI-Distance.png',dpi=300)

	plt.show()



if __name__ == '__main__':
	print("hello")
	filename = '../ble-rssi-dataset/iBeacon_RSSI_Labeled.csv'
	data = GetRSSIData(filename)
	#Plot_location(data)

