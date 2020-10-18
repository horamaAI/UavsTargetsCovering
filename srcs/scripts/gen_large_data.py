alldata=[]
import random
from random import randint
import numpy as npy

nuavs=20000
nrelaxations=300
r=125
r2=300
delta=50
alldata=[]
centroids=[[218.04, 728.903], [235.699, 303.518], [701.801, 264.429], [783.016, 738.101]]
x0,xinf=0,10000
y0,yinf=0,10000

def euclidiandistance(p1, p2):
	return npy.linalg.norm(p1-p2)

def satisfyConstraints(aPoint):
	# within map
	if(aPoint[0]<=0 or aPoint[0]>=xinf or aPoint[1]<=0 or aPoint[1]>=yinf):
		return False

	# inside a cluster
	for centroid in centroids:
		if euclidiandistance(centroid, aPoint) < r:
			return True

	# in allowed relaxation
	for centroid in centroids:
		if euclidiandistance(centroid, aPoint) < r2 and nrelaxations>0 :
			return True

	return False



for i in range(nuavs) :
	alldata.append([round(random.uniform(x0,xinf),2),round(random.uniform(y0,yinf),2)])# round to 2nd member decimal

f=open("dummy"+str(nuavs),"w")
f.write(str(18)+"\n")# to correct, at stage useless, depicts number of available uavs
f.write(str(uavsrange)+"\n")
f.write(str(nuavs)+"\n")
f.write(str(xinf)+","+str(yinf)+"\n")

for point in alldata :
	f.write(str(point[0])+","+str(point[1])+"\n")

f.close()
