// Yeech's Code
// Not to be confused with the Vinci Code

ZRState me;
int state, POIID, picNum, solarFlareBegin;
float POI[3],otherPOI[3],brakingPos[3],facing[3],uploadPos[3];

void init() {

	solarFlareBegin = 1000; //Just to make the solar storm evasion code neater

}

void loop() {
	
	api.getMyZRState(me);

	if(api.getTime() % 60 == 0 && state != 4) state = 0;
	
	DEBUG(("State = %d\n", state));

	picNum = game.getMemoryFilled();

	// Solar Storm Evasion

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
	    DEBUG(("I don't know when the next flare is, so stop asking.\n"));
	    solarFlareBegin = 1000; //Fixes a glitch that makes game.getNextFlare()
	    //return 30s at some random point in the beginning of the game,
	    //and from then on return -1 until the next actual flare, so that the 
	    //SPHERE reboots for no reason.
	}
	
	switch (state) {

		case 0: // POI Selection
			game.getPOILoc(POI,0);
			game.getPOILoc(otherPOI,1);
			if (distance(me,otherPOI) <= distance(me,POI)) {
				POIID = 1;
				for (int i = 0; i < 3; i++) POI[i] = otherPOI[i];
			}
			else POIID = 0;
			
			DEBUG(("POI Coors = %f,%f,%f\n",POI[0],POI[1],POI[2]));

			for (int i = 0; i < 3; i++) {
				brakingPos[i] = POI[i] * 2.0; //1.5625; <- let's try the outer zone
			}
			
			state = 1;
			break;

		case 1: // Orbit Function here
			if (AreWeThereYet(brakingPos,0.01,0.01)) state = 2;
			else {
			    setPositionTarget(brakingPos);
			    mathVecSubtract(facing,POI,me,3);
			    mathVecNormalize(facing,3);
			    api.setAttitudeTarget(facing);
			}
			break;

		case 2: // First Pic in inner Zone
			setPositionTarget(brakingPos);
			mathVecSubtract(facing,POI,me,3);
			mathVecNormalize(facing,3);
			api.setAttitudeTarget(facing);
            		game.takePic(POIID);

			if (picNum > 0) {
				DEBUG(("%d picture(s) have been taken\n", picNum));
				state = 3;
			}
			break;

		case 3: // Moving to outer zone
			for (int i = 0; i < 3; i++) {
				brakingPos[i] = POI[i] * 2.5;
			}
			
			if (AreWeThereYet(brakingPos,0.01,0.01)) state = 4;
			else {
			    api.setPositionTarget(brakingPos);
			    mathVecSubtract(facing,POI,me,3);
			    mathVecNormalize(facing,3);
			    api.setAttitudeTarget(facing);
			    game.takePic(POIID);
			}
			break;
		
		case 4: //Taking pic in outer zone
			api.setPositionTarget(brakingPos);
			mathVecSubtract(facing,POI,me,3);
			mathVecNormalize(facing,3);
			api.setAttitudeTarget(facing);
            		game.takePic(POIID);

			if (picNum > 1) {
				DEBUG(("%d picture(s) have been taken\n", picNum));
				state = 5;
			}
			break;

		case 5: // Upload the picture
			if (velocity(me) < 0.01 && distance(me,uploadPos) < 0.05) {
				game.uploadPic();
				DEBUG(("I just uploaded %d picture(s).\n", picNum));
				DEBUG(("I am in state %d.\n", state)); //Why the f**k does it say it's in State 3???
				// B/c ding-dongs forgot to type "break;"
				state = 0;
			}
			else {

				mathVecSubtract(facing,POI,me,3);
				mathVecNormalize(facing,3);
				api.setAttitudeTarget(facing);

				api.setPositionTarget(uploadPos);
				game.takePic(POIID);
			}

			break;
			
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

void uploadCalc(float uploadPos[], float me[]){
	mathVecNormalize(me,3);
	for (int i = 0; i < 3; i++) {
		uploadPos[i] = me[i] * 0.6;
	}
}

void mathVecProject(float c[], float a[], float b[], int n) {
    // finds the projection of a onto b, puts the result in c
    for (int i = 0; i < n; i++) {
        c[i] = (mathVecInner(a,b,3) * b[i]) / (mathVecMagnitude(b,3) * mathVecMagnitude(b,3));
    }
}

void setPositionTarget(float target[3]) {
	ZRState me;
	api.getMyZRState(me);
	
	float myPos[3],meMag,zero[3],cross[3],inner;
	
	for(int i = 0; i < 3; i++) {
		myPos[i] = me[i];
		zero[i] = 0.0;
	}
	
	meMag = mathVecMagnitude(myPos,3);
	
	mathVecCross(cross,myPos,target);
	inner = mathVecInner(myPos,target,3);

	if (minDistanceFromAsteroid(target) > 0.32) {
		api.setPositionTarget(target);
	}
	
	else if (meMag >= 0.22 && meMag <= 0.32) {
		for (int i = 0; i < 3; i++) {
			myPos[i] = myPos[i] * 1.6;
		}
		
		api.setPositionTarget(myPos);
		DEBUG(("TOO CLOSE"));
	}
	
	else if (mathVecMagnitude(cross,3) < 0.01 && inner > 0.99 && inner < 1.01) {
		api.setPositionTarget(target);
		DEBUG(("GOOD COLLINEARITY DETECTED"));
	}
	
	else if (mathVecMagnitude(cross,3) < 0.01 && inner < -0.99 && inner > -1.01) {
		api.setPositionTarget(myPos);
		DEBUG(("BAD COLLINEARITY DETECTED"));
	}
	
	else {
		float opposite[3], perpendicular[3], mePrep[3];
		
		
		
		mathVecProject(opposite,target,myPos,3);
		mathVecSubtract(perpendicular,target,opposite,3);
		
		for (int i = 0; i < 3; i++) {
		    mePrep[i] = perpendicular[i] / mathVecMagnitude(perpendicular,3);
		}
		
		for (int i = 0; i < 3; i++) {
			mePrep[i] = (mePrep[i] * 0.32 * 2 * meMag) / (sqrtf(meMag*meMag - 0.32*0.32));
		}

		api.setPositionTarget(mePrep);
	}
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
