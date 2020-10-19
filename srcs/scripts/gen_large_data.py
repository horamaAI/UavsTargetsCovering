alldata=[]
import random
from random import randint
import math

dim=2
x0,xinf=0,10000
y0,yinf=0,10000
ntargets=20000
centroids=[]
in_centroids_counter=[]
uavsradius=125.
r=7*uavsradius
r2=3*r
delta=50.
nrelaxed=10250
nscattered=1050
targets=[]
avrg=0.
maxpercluster=0
maxtourperrandom=100


def euclidiandistance(p1, p2):
	return math.sqrt((p1[0]-p2[0])**2 + (p1[1]-p2[1])**2)

def satisfyConstraints(newpoint, centroid_index):
	global nrelaxed
	# within map
	if(newpoint[0]<=x0 or newpoint[0]>=xinf or newpoint[1]<=y0 or newpoint[1]>=yinf):
		return False

	# centroid has reached maximum number of allowed targets
	if in_centroids_counter[centroid_index] > maxpercluster :
		return False

	# inside a cluster
	if euclidiandistance(newpoint, centroids[centroid_index]) < r:
		return True

	# in allowed relaxation
	if ( ( euclidiandistance(newpoint, centroids[centroid_index]) < r2 ) and nrelaxed > 0 ):
		nrelaxed -= 1
		return True

	return False



aFile="./outs/input_6_centroids_10000_10000_map.csv"

with open(aFile) as f:
	lines=f.readlines()
	for x in lines :
		row=x.rstrip('\n').split(',')
		centroids.append([float(row[0]),float(row[1])])

for i in range(nscattered):
	aPoint=[0., 0.]
	aPoint[0]=round(random.uniform(x0+300, xinf-300),3)
	aPoint[1]=round(random.uniform(x0+300, xinf-300),3)
	targets.append(aPoint)# round to 2nd member decimal

capital=ntargets-nscattered
maxpercluster=ntargets/(len(centroids))
in_centroids_counter=[0 for i in range(len(centroids))]

while capital > 0 :
	for i in range(len(centroids)) :
		if capital <=0:
			break
		tours = 0
		while True and tours < maxtourperrandom:
			if capital <=0:
				break
			tours += 1
			#print(round(random.uniform(-xinf,xinf),3))
			aPoint=[0., 0.]
			aPoint[0]=round(random.uniform(centroids[i][0]-r2,centroids[i][0]+r2),3)
			aPoint[1]=round(random.uniform(centroids[i][1]-r2,centroids[i][1]+r2),3)
			result = satisfyConstraints(aPoint, i)
			if result:
				targets.append(aPoint)# round to 2nd member decimal
				in_centroids_counter[i] += 1
				capital -= 1
				break

	

f=open(str(ntargets)+"_grounds.csv","w")
f.write(str(18)+"\n")# to correct, at stage useless, depicts number of available uavs
f.write(str(125)+"\n")# uavs range
f.write(str(ntargets)+"\n")# number of ground targets
f.write(str(xinf)+","+str(yinf)+"\n")# size of map

for point in targets :
	f.write(str(point[0])+","+str(point[1])+"\n")

f.close()
