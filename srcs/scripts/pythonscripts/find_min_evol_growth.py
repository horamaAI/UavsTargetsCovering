data=[[] for i in range(21)]

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
			data[i].append(float(row[6]))
		else :
			print("\"DANGER!!!\", Mystikal")

for ele in data:
	print(ele)
