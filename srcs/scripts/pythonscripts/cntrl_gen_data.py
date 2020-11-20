
import global_vbles

with open(global_vbles.param_file) as f:
	lines=f.readlines()
	gen_centroids_lines=lines[1:8]
	gen_large_data_lines=lines[10:22]

	# gen centroids
	global_vbles.delta=int(gen_centroids_lines[0].rstrip('\n').split('=')[1])
	global_vbles.crop=int(gen_centroids_lines[1].rstrip('\n').split('=')[1])
	global_vbles.withinpointsdeviation=int(gen_centroids_lines[2].rstrip('\n').split('=')[1])
	global_vbles.maxtours=int(gen_centroids_lines[3].rstrip('\n').split('=')[1])
	global_vbles.centroidseed=[[float(ele) for ele in gen_centroids_lines[4].rstrip('\n').split('=')[1].split(',')]]
	global_vbles.x0=float(gen_centroids_lines[5].rstrip('\n').split('=')[1].split(',')[0])
	global_vbles.xinf=float(gen_centroids_lines[5].rstrip('\n').split('=')[1].split(',')[1])
	global_vbles.y0=float(gen_centroids_lines[6].rstrip('\n').split('=')[1].split(',')[0])
	global_vbles.yinf=float(gen_centroids_lines[6].rstrip('\n').split('=')[1].split(',')[1])


	# gen large data
	global_vbles.dim=int(gen_large_data_lines[0].rstrip('\n').split('=')[1])
	global_vbles.x0=float(gen_large_data_lines[1].rstrip('\n').split('=')[1].split(',')[0])
	global_vbles.xinf=float(gen_large_data_lines[1].rstrip('\n').split('=')[1].split(',')[1])
	global_vbles.y0=float(gen_large_data_lines[2].rstrip('\n').split('=')[1].split(',')[0])
	global_vbles.yinf=float(gen_large_data_lines[2].rstrip('\n').split('=')[1].split(',')[1])
	global_vbles.ntargets=int(gen_large_data_lines[3].rstrip('\n').split('=')[1])
	global_vbles.uavsradius=float(gen_large_data_lines[4].rstrip('\n').split('=')[1])
	global_vbles.r=float(gen_large_data_lines[5].rstrip('\n').split('=')[1])*global_vbles.uavsradius
	global_vbles.r2=global_vbles.r*(1+float(gen_large_data_lines[6].rstrip('\n').split('=')[1]))
	global_vbles.delta=float(gen_large_data_lines[7].rstrip('\n').split('=')[1])
	global_vbles.nrelaxed=int(gen_large_data_lines[8].rstrip('\n').split('=')[1])
	global_vbles.nscattered=int(gen_large_data_lines[9].rstrip('\n').split('=')[1])
	global_vbles.maxpercluster=int(gen_large_data_lines[10].rstrip('\n').split('=')[1])
	global_vbles.maxtourperrandom=int(gen_large_data_lines[11].rstrip('\n').split('=')[1])

