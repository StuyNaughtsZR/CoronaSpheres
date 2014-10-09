float dist(float p1[], float p2[]){
	float d = 0;
	for(int i=0; i < 3; i++){
		d += (p2[i]-p1[i]) * (p2[i]-p1[i]);
	}
	return sqrtf(d);
}

void dilateValue(float pt1[3], float pt2[3], float dilation , float dst[3]){
	//pt1 is starting point (anchor)
	//pt2 is second point along the line
	//dilation will scale out from pt2 away or towards pt1
	//Sets dst to be destination/target
	for(int i=0; i < 3; i++){
		dst[i] = dilation * (pt1[i] - pt2[i]) + pt1[i];
	}
}

void goToLoc(ZRState state, float dest[3], float smoothness){
	//You should run getMyZRState a few lines before this
	//state is your ZRState (a 12 element array)
	//
	float val[3];
	mathVecSubtract(val, dest, state , 3);
	for(int i=0; i<3; i++){
		val[i] *= smoothness;
	}
	api.setVelocityTarget(val);
}


