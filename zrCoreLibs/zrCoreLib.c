//Universal commands

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
	//Smoothness is a multiplier on the array. Optimal range: 0.15-0.35
	float val[3];
	mathVecSubtract(val, dest, state , 3);
	for(int i=0; i<3; i++){
		val[i] *= smoothness;
	}
	api.setVelocityTarget(val);
}

int setPosTarg(float dst[3], float maxdist, float maxvel, ZRState state){
    goToLoc(state, dst, 0.25);
    if (dist(state , dst) <= maxdist) {
        //STOPPPP
        return 1;
    }
    else {
        return 0;
    }
}

// CoronaSpheres specific commands:
//Put /* */ around if not using

void goToNoCollision(ZRState start , float end[3] , float currSpeed[3] , float leniency){
	/*
		Go in an elliptical path by
		1) Creating a waypoint as an outer bound 'mirror'
		2) Using goToLoc with varying speeds
		3) Go to a destination point at the same time, resulting in a curve
		4) 2 states: Before waypoint and After waypoint
	*/
	//START DENOTES YOUR LOCATION!!!
	//Create center of circle:
	float center[3];
	for(int i = 0; i < 3; i++){
		center[i] = (start[i] + end[i]) / 2;
	}
<<<<<<< HEAD
	
=======
	/*
		Since the center of the circle we created is always the midpoint of the previous 2 lines, we have to figure out which direction to go wrt origin
		Using quadrants, if both are in the same quadrant, use goToLoc instead.

	*/
>>>>>>> b83696af5ebae1baec11db971e1edff121030a35
}
