//Yeech + Dennis = EPIC SWAG
//Seriously use the github Version
ZRState me;
int state, POIID;
float POI[3], rotation[3], myPos[3],brakingPos[3];
void init(){
	
	state = 0;
	
	rotation[0] = 0;
	rotation[1] = 0;
	rotation[2] = 0;

}

void loop(){
	api.getMyZRState(me);
	
	for (int i = 0; i < 3; i++) {
	    myPos[i] = me[i];
	}
	
	DEBUG(("State = %d\n",state));
	
	if(state == 0){ // select POI
		float test0[3];
		game.getPOILoc(test0,0);
		float test1[3];
		game.getPOILoc(test1,1);
		if (distance(myPos,test0) <= distance(myPos,test1)) {
		    POIID = 0;
		    for (int i = 0; i < 3; i++) POI[i] = test0[i];
	        state = 1;
		}
		else {
		    POIID = 1;
		    for (int i = 0; i < 3; i++) POI[i] = test1[i];
	        state = 1;
		}
		for(int i = 0; i < 3; i++) brakingPos[i] = 1.25*POI[i]; // Smart...
		DEBUG(("POIcoors = %f,%f,%f\n",POI[0],POI[1],POI[2]));
	}
	if(state == 1) { // move to brakingPos
		if (api.getTime() % 60 == 0) state = 0;
	    else if (velocity(me) < 0.001) {
	        state = 2;
	    }
	    else {
	        api.setPositionTarget(brakingPos);
	    }
	}
	if(state == 2) { // take picture
		if (api.getTime() % 60 < 2) state = 0;
	    else {
	        if (game.alignLine(POIID)) {
	            DEBUG(("The damn align function worked\n"));
	            // state = 3; // Maybe I am being stupid... But if this was carried out, the game is no longer at state 2, which means that it won't take picture.
	        }
	        else {
	            api.setAttitudeTarget(POI);
	            DEBUG(("The damn align function didn't work\n")); // So far it is at this stage
	        }
	        if (game.getMemoryFilled() > 0) {
	            DEBUG(("The picture was taken!\n"));
	            state = 3;
	        }
	        else {
	            api.setAttitudeTarget(POI);
	            game.takePic(POIID);
	            DEBUG(("Still no picture...\n"));
	        }
	    }
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

