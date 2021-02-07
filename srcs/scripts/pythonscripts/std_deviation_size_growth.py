
import matplotlib.pyplot as plt
import numpy as np

# targets=["set1_1000_1000_map", "set2_1500_1500_map", "set3_2000_2000_map", "set4_2500_2500_map", "set5_3000_3000_map",
# "set6_3500_3500_map", "set_large_gaps_for_cc"]


targets=["set1_1000_1000_map", "set2_1500_1500_map", "set3_2000_2000_map", "set4_2500_2500_map", "set5_3000_3000_map",
"set6_3500_3500_map"]

alldata=[[[] for j in range(20)] for i in targets]

k=0
for i in targets:
	for j in range(20):
		with open("../outs/"+i+"/"+str((j+1)*50)+"_grounds_coords.csv") as f:
			lines=f.readlines()
			for x in lines:
				row=x.rstrip('\n').split(',')
				alldata[k][j].append([float(row[0]), float(row[1])])
	k+=1

avrgstds=[[] for i in targets]

for i in range(len(alldata)):
	for j in range(len(alldata[i])):
		avrgstds[i].append(np.std(alldata[i][j]))

#for row in avrgstds:
#	print(len(row))
#	print("==============================\n")

# fig = plt.figure(figsize=(10, 7))

# ax=fig.add_axes([0, 0, 1, 1])

# bp=ax.boxplot(avrgstds)

plt.boxplot(avrgstds)
plt.xlabel('datasets')
plt.ylabel('standard deviation')
# plt.xticks([1, 2, 3, 4, 5, 6, 7], ['set 1', 'set 2', 'set 3', 'set 4', 'set 5', 'set 6', 'Large'])
plt.xticks([1, 2, 3, 4, 5, 6], ['set 1', 'set 2', 'set 3', 'set 4', 'set 5', 'set 6'])
plt.show()