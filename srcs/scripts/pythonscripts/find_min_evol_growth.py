data=[]

prefix="../../out/runtime_growth/"
sufix="_targets.csv"

for i in range(1, 21):
	with open(prefix+str(i*50)+sufix) as f:
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
			data.append([float(row[6]), float(row[7]), float(row[8]), float(row[9])])
		else :
			print("\"DANGER!!!\", Mystikal")

f=open("../outs/results_runtimes.csv","w")

f.write("# tot runtime, time_spent_1st_phase, time_spent_lp_prblm, time_spent_cc_phase\n")

for i in range(1, len(data)) :
	f.write(str(data[i][0])+","+str(data[i][1])+","+str(data[i][2])+","+str(data[i][3])+"\n")

f.close()
