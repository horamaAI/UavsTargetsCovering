
# targets=["data", "set_large_gaps_for_cc"]

# targets=["set1_1000_1000_map", "set2_1500_1500_map", "set3_2000_2000_map", "set4_2500_2500_map", "set5_3000_3000_map",
# "set6_3500_3500_map"]

targets=["set_large_gaps_for_cc"]

data=[[] for i in range(len(targets))]

prefix="../../out/runtime_growth/"
sufix="_targets.csv"

z=0
for k in targets:
	for i in range(1, 21):
		with open(prefix+k+"/"+str(i*50)+sufix) as f:
			lines=f.readlines()
			lines=lines[1:]
			j=0
			minval=0
			for x in lines :
				row=x.rstrip('\n').split(',')
				if j==0 :
					j+=1
					minval=float(row[6])
				if minval > float(row[6]):
					minval=float(row[6])
			if minval != 0 :
				data[z].append([float(row[6]), float(row[7]), float(row[8]), float(row[9])])
			else :
				print("\"DANGER!!!\", Mystikal")
	z+=1

for k in range(len(data)):

	f=open("../outs/"+targets[k]+"_results_runtimes.csv","w")

	f.write("# tot runtime, time_spent_1st_phase, time_spent_lp_prblm, time_spent_cc_phase\n")

	for i in range(len(data[k])) :
		f.write(str(data[k][i][0])+","+str(data[k][i][1])+","+str(data[k][i][2])+","+str(data[k][i][3])+"\n")

	f.close()
