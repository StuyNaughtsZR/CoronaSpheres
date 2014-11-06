// Dencheng Code: Revolutions
// Not to be confused with the Da Vinci Code

ZRState me;
int state, POIID, solarFlareBegin;
float POI[3],otherPOI1[3],otherPOI2[3],destination[3],facing[3],dis;

void init() {

	solarFlareBegin = 1000; //Just to make the solar storm evasion code neater

}

void loop() {
	
	api.getMyZRState(me);

	if(api.getTime() % 60 == 0 && state != 3) state = 0;

	rebootIfStorm();
	
	DEBUG(("I am in state %d.\n", state));

	DEBUG(("I can still take %d photos.\n", game.getMemorySize() - game.getMemoryFilled()));
	
	if (state == 0) {// POI Selection
	
		game.getPOILoc(POI,0);
		game.getPOILoc(otherPOI1,1);
		game.getPOILoc(otherPOI2,2);
		
		if (distance(me,otherPOI1) <= distance(me,POI)) {
			if (distance(me,otherPOI2) <= distance(me,otherPOI1)){
				POIID = 2;
				for (int i = 0; i < 3; i++) POI[i] = otherPOI2[i];
			}
			else {
				POIID = 1;
				for (int i = 0; i < 3; i++) POI[i] = otherPOI1[i];
			}
		}
		
		else if (distance(me,otherPOI2) <= distance(me,POI)) {
			POIID = 2;
			for (int i = 0; i < 3; i++) POI[i] = otherPOI2[i];
		}
		
		else POIID = 0;
		
		DEBUG(("POI Coors = %f,%f,%f\n",POI[0],POI[1],POI[2]));
	
		for (int i = 0; i < 3; i++) {
			destination[i] = POI[i] * 0.38 / mathVecMagnitude(POI,3); //inner
		}

		state = 1;
		
	}

	if (state == 1) {// Take pic in inner zone
	
		if (!AreWeThereYet(destination,0.05,0.05)) {
		    setPositionTarget(destination);
		    mathVecSubtract(facing,POI,me,3);
		    if (mathVecMagnitude(facing,3) > 0.4) api.setVelocityTarget(facing);
		    mathVecNormalize(facing,3);
		    api.setAttitudeTarget(facing);
		}
		else game.takePic(POIID);
		
		if (game.getMemoryFilled() > 0 && mathVecMagnitude(me, 3) < 0.42) {
			DEBUG(("%d picture(s) have been taken\n", game.getMemoryFilled()));
			for (int i = 0; i < 3; i++) {
				destination[i] = POI[i] * 0.43 / mathVecMagnitude(POI,3);
		    }
			setPositionTarget(destination,3);
			mathVecSubtract(facing,POI,me,3);
			mathVecNormalize(facing,3);
    		api.setAttitudeTarget(facing);

			state = 2;
		}
		
	}

	if (state == 2) {// Take pic in outer zone
	
        if (!AreWeThereYet(destination,0.05,0.05)) {
		    setPositionTarget(destination);
		    mathVecSubtract(facing,POI,me,3);
		    mathVecNormalize(facing,3);
		    api.setAttitudeTarget(facing);
		}
		else game.takePic(POIID);
		
		if (game.getMemoryFilled() > 1) {
			DEBUG(("%d picture(s) have been taken\n", game.getMemoryFilled()));
			for (int i = 0; i < 3; i++) {
	        	destination[i] = me[i] * 0.61 / mathVecMagnitude(me,3);
	        }
			setPositionTarget(destination,3);

			state = 3;
		}
		
	}

	if (state == 3) {// Upload the picture
	
		if (mathVecMagnitude(me,3)>0.53) {
		    int picNum = game.getMemoryFilled();
			game.uploadPic();
			DEBUG(("I just uploaded %d picture(s).\n", (picNum - game.getMemoryFilled())));
			state = 0;
		}
	
		else {
			for (int i = 0; i < 3; i++) {
	        	destination[i] = me[i] * 0.61 / mathVecMagnitude(me,3);
	        }
			api.setPositionTarget(destination);
		}

	}
	
}

void rebootIfStorm() {
    if (api.getTime() == (solarFlareBegin - 1)) {
	    DEBUG(("I shall now reboot.\n"));
		game.turnOff();
		game.turnOn();
		state = 0;
	}
	else if (api.getTime() >= (solarFlareBegin) && api.getTime() <= (solarFlareBegin + 2)) {
	    DEBUG(("Ah shit, it's a flare!\n"));
	}
	else if (game.getNextFlare() != -1) {
	    solarFlareBegin = api.getTime() + game.getNextFlare();
	    DEBUG(("Next solar flare will occur at %ds.\n", solarFlareBegin));
	}
	else {
	    DEBUG(("I don't know when the next flare is.\n"));
	    solarFlareBegin = 1000;
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

void mathVecProject(float c[], float a[], float b[], int n) {
    // finds the projection of a onto b, puts the result in c
    for (int i = 0; i < n; i++) {
        c[i] = (mathVecInner(a,b,3) * b[i]) / (mathVecMagnitude(b,3) * mathVecMagnitude(b,3));
    }
}

void setPositionTarget(float target[3], float multiplier) {
    float temp[3];
    mathVecSubtract(temp,target,me,3);
    for (int i = 0; i < 3; i++) temp[i] = me[i] + temp[i] * multiplier;
    setPositionTarget(temp);
}

void setPositionTarget(float target[3]) {
	api.getMyZRState(me);
	
	float myPos[3],meMag;
	
	for(int i = 0; i < 3; i++) {
		myPos[i] = me[i];
	}
	
	meMag = mathVecMagnitude(myPos,3);
	
	if (minDistanceFromOrigin(target) > 0.31) {
		api.setPositionTarget(target);
	}
	
	else if (meMag >= 0.22 && meMag <= 0.32) {
		for (int i = 0; i < 3; i++) {
			myPos[i] = myPos[i] * 1.6;
		}
		
		api.setPositionTarget(myPos);
		DEBUG(("TOO CLOSE\n"));
	}
	
	else {
		float opposite[3], perpendicular[3], mePrep[3], path[3], temp[3];
		
		mathVecProject(opposite,target,myPos,3);
		mathVecSubtract(perpendicular,target,opposite,3);
		
		for (int i = 0; i < 3; i++) {
		    mePrep[i] = perpendicular[i] / mathVecMagnitude(perpendicular,3);
		}
		
		for (int i = 0; i < 3; i++) {
			mePrep[i] = (mePrep[i] * 0.325 * meMag) / (sqrtf(meMag*meMag - 0.32*0.32));
		}
		
		mathVecSubtract(path,mePrep,myPos,3);
		
		for (int i = 0; i < 3; i++) {
			path[i] = path[i] * 2;
		}
		
		mathVecAdd(temp,myPos,path,3);

		api.setPositionTarget(temp);
		
		DEBUG(("TAKING THE TANGENT\n"));
	}
}

int AreWeThereYet(float target[3], float maxDis, float maxSpeed) {
	api.getMyZRState(me);
	return (distance(me,target) < maxDis) && (velocity(me) < maxSpeed);
}

float minDistanceFromOrigin(float target[]) {
	float path1[3], path2[3], cosine;
	
	mathVecSubtract(path1,target,me,3);
	mathVecSubtract(path2,me,target,3);
	
	cosine = mathVecInner(path1,me,3) / (mathVecMagnitude(path1, 3) * mathVecMagnitude(me, 3));
	
	if (cosine < 0) {
		return mathVecMagnitude(me, 3);
	}
	
	cosine = mathVecInner(path2,target,3) / (mathVecMagnitude(path2, 3) * mathVecMagnitude(target, 3));
	
	if (cosine < 0) {
		return mathVecMagnitude(target,3);
	}
	
	else {
		mathVecProject(path2,me,path1,3);
		
		mathVecSubtract(path1,me,path2,3);
		
		return mathVecMagnitude(path1,3);
	}
}
