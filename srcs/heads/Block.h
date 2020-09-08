#ifndef UAVSCOVS_HEADS_BLOCK_H_
#define UAVSCOVS_HEADS_BLOCK_H_

class Block{

	float lowleft[2];// coordinates of the lower left bound
	float upright[2];// coords of upper right bound
	char color;// if leaf : 'b' for black (block contains only 	), 'w' for white (area empty), 'g' otherwise ('grey 'node, not leaf)
	int label;// tells which connected component the block belongs to
	Block father;
	Block quadrants[4];// 4 regions of a quadtree's node : NW, NE, SW, SE

}

#endif // UAVSCOVS_HEADS_BLOCK_H_
