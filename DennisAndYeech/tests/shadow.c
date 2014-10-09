// Testing Time and Fuel usage to get to shadow zone from starting position

ZRState me;
float shadow1[3],Timei,Timef,Fueli,Fuelf;
int state;

void init() {
	state = 0;

	Timei = 0.0;

	Fueli = game.getFuelRemaining();

	shadow1[0] = 0.32;
	shadow1[1] = -0.19;
	shadow1[2] = 0.0;
}

void loop() {
	api.getMyZRState(me);
	switch (state) {
		
		case 0:

			if (AreWeThereYet(shadow1,0.01,0.01)) state = 1;
			else {
				setPositionTarget(shadow1);
			}

			break;

		case 1:
			Timef = api.getTime();
			Fuelf = game.getFuelRemaining();

			DEBUG(("Time elapsed: %f \n Fuel Spent: %f", Timef-Timei, Fueli-Fuelf));
			
			break;
	}
}

void setPositionTarget(float target[]) {
	ZRState me;
	api.getMyZRState(me);
	float myPos[3];
	
	for(int i = 0; i < 3; i++) {
		myPos[i] = me[i];
	}

	if (minDistanceFromAsteroid(target) > 0.32) {
		api.setPositionTarget(target);
	}
	
	else {
		float opposite[3], perpendicular[3], temp[3];
		mathVecProject(opposite,target,myPos,3);
		mathVecSubtract(perpendicular,target,opposite,3);
		for (int i = 0; i < 3; i++) {
			temp[i] = mathVecMagnitude(myPos,3) * perpendicular[i] / mathVecMagnitude(perpendicular,3);;
		}

		api.setPositionTarget(temp);
	}
}

void mathVecProject(float c[], float a[], float b[], int n) {
    // finds the projection of a onto b, puts the result in c
    for (int i = 0; i < n; i++) {
        c[i] = mathVecInner(a, b, 3)/mathVecInner(b, b, 3) * b[i];
    }
}

float distance(float p1[], float p2[]){
	float d = 0;
	for(int i=0; i < 3; i++){
		d += (p2[i]-p1[i])*(p2[i]-p1[i]);
	}
	return sqrtf(d);
}

float velocity(float p1[]){
	float d = 0;
	for(int i=3; i < 6; i++){
		d += p1[i]*p1[i];
	}
	return sqrtf(d);
}

int AreWeThereYet(float target[3], float maxDis, float maxSpeed) {
	ZRState me;
	api.getMyZRState(me);
	return (distance(me,target) < maxDis) && (velocity(me) < maxSpeed);
}

float minDistanceFromAsteroid(float target[3]){
	float path[3], proj[3], dis[3], negMe[3];
	ZRState me;
	api.getMyZRState(me);
	
	for (int i = 0; i < 3; i++) {
		path[i] = target[i] - me[i];
		negMe[i] = -me[i];
	}
	
	mathVecProject(proj,negMe,path,3);

	for (int i = 0; i < 3; i++) {
		dis[i] = me[i] + proj[i];
	}

	return mathVecMagnitude(dis,3);
}
