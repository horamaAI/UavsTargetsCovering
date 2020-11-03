alldata=[]
import random
from random import randint
import math

dim=2
x0,xinf=0,10000
y0,yinf=0,10000
ntargets=5500
centroids=[]
in_centroids_counter=[]
uavsradius=125.
r=10*uavsradius
r2=r+(0.5*r)
delta=50.
nrelaxed=250
nscattered=0
targets=[]
avrg=0.
maxpercluster=0
maxtourperrandom=100
aFile="./outs/input_8_centroids_10000_10000_map.csv"


'''
dim=2
x0,xinf=0,1000
y0,yinf=0,1000
ntargets=300
centroids=[]
in_centroids_counter=[]
uavsradius=125.
r=2*uavsradius
r2=r+(0.5*r)
delta=50.
nrelaxed=75
nscattered=0
targets=[]
avrg=0.
maxpercluster=0
maxtourperrandom=100
aFile="./outs/input_4_centroids_1000_1000_map.csv"
'''


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



with open(aFile) as f:
	lines=f.readlines()
	for x in lines :
		row=x.rstrip('\n').split(',')
		centroids.append([float(row[0]),float(row[1])])

for i in range(nscattered):
	aPoint=[0., 0.]
	aPoint[0]=round(random.uniform(x0+1300, xinf-1300),3)
	aPoint[1]=round(random.uniform(x0+3300, xinf-3300),3)
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

	

f=open("./outs/"+str(ntargets)+"_grounds.csv","w")
f2=open("./outs/"+str(ntargets)+"_grounds_coords.csv","w")
f.write(str(18)+"\n")# to correct, at stage useless, depicts number of available uavs
f.write(str(125)+"\n")# uavs range
f.write(str(ntargets)+"\n")# number of ground targets
f.write(str(xinf)+","+str(yinf)+"\n")# size of map

for point in targets :
	f.write(str(point[0])+","+str(point[1])+"\n")
	f2.write(str(point[0])+","+str(point[1])+"\n")

f.close()
