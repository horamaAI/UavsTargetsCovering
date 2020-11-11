import random
from random import randint
import math

import global_vbles
import cntrl_gen_data

allcentroids=[]

'''
delta=510
crop=1250
withinpointsdeviation=2750
maxtours=30
allcentroids=[]
centroidseed=[[235.699, 303.518]]
x0,xinf=0.,10000.
y0,yinf=0.,10000.
'''

'''
delta=75
crop=75
withinpointsdeviation=127
maxtours=30
allcentroids=[]
centroidseed=[[235.699, 303.518]]
x0,xinf=0.,1000.
y0,yinf=0.,1000.
'''


def euclidiandistance(p1, p2):
	return math.sqrt((p1[0]-p2[0])**2 + (p1[1]-p2[1])**2)

def satisfyConstraint(aPoint):
	# within map
	if(aPoint[0]<global_vbles.crop or aPoint[0]>global_vbles.xinf-global_vbles.crop
		or aPoint[1]<global_vbles.crop or aPoint[1]>global_vbles.yinf-global_vbles.crop):
		return False
	for centroid in allcentroids:
		if euclidiandistance(aPoint, centroid) < global_vbles.withinpointsdeviation:
			return False
	return True

for i in range(10) :
	for j in range(10) :
		if random.randint(0,1) > 0.2:# give a chance to skip some range
			continue
		for k in range(len(global_vbles.centroidseed)):
			tours = 0
			while True and tours < global_vbles.maxtours:
				tours = tours + 1
				#print(round(random.uniform(-xinf,xinf),3))
				aPoint=[0., 0.]
				aPoint[0]=round(i*(global_vbles.xinf/10.)
					+global_vbles.centroidseed[k][0]+random.uniform(-global_vbles.xinf,global_vbles.xinf),3)
				aPoint[1]=round(j*(global_vbles.yinf/10.)
					+global_vbles.centroidseed[k][0]+random.uniform(-global_vbles.yinf,global_vbles.yinf),3)

				if satisfyConstraint(aPoint):
					allcentroids.append(aPoint)# round to 2nd member decimal
					break

global_vbles.aFile="../outs/input_"+str(len(allcentroids))+"_centroids_"+str(int(global_vbles.xinf))+"_"\
	+str(int(global_vbles.yinf))+"_map.csv"

f=open(global_vbles.aFile,"w")

for point in allcentroids :
	f.write(str(point[0])+","+str(point[1])+"\n")

f.close()
