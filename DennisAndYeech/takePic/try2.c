//Yeech + Dennis = EPIC SWAG

ZRState me;
int state, POIID;
float POI[3],test0[3],test1[3];

void init() {

	state = 0;

}

void loop() {
	
	DEBUG(("State = %d\n", state));

	api.getMyZRState(me);

	switch (state) {

		case  0:
			game.getPOILoc(test0,0);
			game.getPOILoc(test1,1);
			if (distance(me,test0) <= distance(me,test1)) {
			    POIID = 0;
		    for (int i = 0; i < 3; i++) POI[i] = test0[i];
	        state = 1;
			}
			else {
			    POIID = 1;
	    		for (int i = 0; i < 3; i++) POI[i] = test1[i];
	       		state = 1;
			}
			
			DEBUG(("POI Coors = %f,%f,%f\n",POI[0],POI[1],POI[2]));
		case 1:
			
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

