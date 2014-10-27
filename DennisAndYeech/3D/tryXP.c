// Dencheng Code -- Experimental
// Not to be confused with the Da Vinci Code

ZRState me;
int state, POIID, picNum, solarFlareBegin;
float POI[3],otherPOI1[3],otherPOI2[3],brakingPos[3],newBrakingPos[3],facing[3],uploadPos[3],origin[3],dis;

void init() {

	for (int i = 0 ; i < 3 ; i++) {
		origin[i] = 0.0;
	}

	solarFlareBegin = 1000; //Just to make the solar storm evasion code neater

}

void loop() {
	
	api.getMyZRState(me);

	if(api.getTime() % 60 == 0 && state != 5) state = 0;
	
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
	    solarFlareBegin = 1000; /*Fixes a glitch that makes game.getNextFlare()
	    return 30s at some random point in the beginning of the game,
	    and from then on return -1 until the next actual flare, so that the 
	    SPHERE reboots for no reason.*/
	}

	DEBUG(("I can still take %d photos", game.getMemorySize() - game.getMemoryFilled()));
	
	switch (state) {

		case 0: // POI Selection
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
				brakingPos[i] = POI[i] * 0.38 / mathVecMagnitude(POI,3); //inner
			}
			
			state = 1;
			break;

		case 1: // Move to pic taking site in inner zone
			if (AreWeThereYet(brakingPos,0.01,0.01)) state = 2;
			else {
			    setPositionTarget(brakingPos);
			    mathVecSubtract(facing,POI,me,3);
			    mathVecNormalize(facing,3);
			    api.setAttitudeTarget(facing);
			}
			break;

		case 2: // First Pic in inner zone
			setPositionTarget(brakingPos);
			mathVecSubtract(facing,POI,me,3);
			mathVecNormalize(facing,3);
			api.setAttitudeTarget(facing);
            game.takePic(POIID);

			if (picNum > 0) {
				DEBUG(("%d picture(s) have been taken\n", picNum));

				for (int i = 0; i < 3; i++) {
					newBrakingPos[i] = POI[i] * 0.43 / mathVecMagnitude(POI,3);
				}
				setPositionTarget(newBrakingPos);
				mathVecSubtract(facing,POI,me,3);
				mathVecNormalize(facing,3);
				api.setAttitudeTarget(facing);

				state = 3;
			}

			break;

		case 3: // Moving to outer zone

			
			if (AreWeThereYet(newBrakingPos,0.01,0.01)) state = 4;
			
			else {
			    api.setPositionTarget(newBrakingPos);
			    mathVecSubtract(facing,POI,me,3);
			    mathVecNormalize(facing,3);
			    api.setAttitudeTarget(facing);
			    //game.takePic(POIID);
			}
			break;
		
		case 4: //Taking pic in outer zone
			api.setPositionTarget(newBrakingPos);
			mathVecSubtract(facing,POI,me,3);
			mathVecNormalize(facing,3);
			api.setAttitudeTarget(facing);

			dis = distance(me, origin);
			
			DEBUG(("I am %f away from the origin\n", dis));

			if(game.alignLine(POIID)) { // returns true
            	DEBUG(("Alignment achieved\n"));
				
				if (dis < 0.53 && dis > 0.42){
					game.takePic(POIID);
					DEBUG(("Imma take a photo!")); // NVM problem solve
				}
				/*

				else if (dis > 0.53) {
					DEBUG(("I'm too far!"));

					for (int i = 0 ; i < 3 ; i++) {
						newBrakingPos[i] = newBrakingPos[i] * 0.95;
					}

					api.setPositionTarget(newBrakingPos);
				}
				else {
					DEBUG(("I'm too close!"));
					
					for (int i = 0 ; i < 3 ; i++) {
						newBrakingPos[i] = newBrakingPos[i] * 1.05;
					}

					api.setPositionTarget(newBrakingPos);
				}
				*/
			}

			else {
				DEBUG(("Still not focused\n"));
			}

			if (picNum > 1) { // Doesn't take a picture, f(3x) and let u be 3x...
				DEBUG(("%d picture(s) have been taken\n", picNum));
				uploadCalc(uploadPos,POI);
				api.setPositionTarget(uploadPos);
				state = 5;
			}

			break;

		case 5: // Upload the picture
			if (AreWeThereYet(uploadPos,0.01,0.01)) {
				game.uploadPic();
				DEBUG(("I just uploaded %d picture(s).\n", picNum));
				DEBUG(("I am in state %d.\n", state)); //Why the f**k does it say it's in State 3???
				// B/c ding-dongs forgot to type "break;"
				state = 0;
			}
			else {

				//mathVecSubtract(facing,POI,me,3);
				//mathVecNormalize(facing,3);
				//api.setAttitudeTarget(facing);
				api.setPositionTarget(uploadPos);
				game.takePic(POIID); // why the f**k not
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

void uploadCalc(float uploadPos[], float POI[]){
	for (int i = 0; i < 3; i++) {
		uploadPos[i] = POI[i] * 0.6 / mathVecMagnitude(POI,3);
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

	if (minDistanceFromOrigin(target) > 0.30) {
		api.setPositionTarget(target);
		DEBUG(("JUST GO!!!\n"));
	}
	
	else if (meMag >= 0.22 && meMag <= 0.32) {
		for (int i = 0; i < 3; i++) {
			myPos[i] = myPos[i] * 1.6;
		}
		
		api.setPositionTarget(myPos);
		DEBUG(("TOO CLOSE\n"));
	}
	
	else if (mathVecMagnitude(cross,3) < 0.05 && inner > 0.95 && inner < 1.05) {
		api.setPositionTarget(target);
		DEBUG(("GOOD COLLINEARITY DETECTED"));
	}
	
	else if (mathVecMagnitude(cross,3) < 0.01 && inner < -0.99 && inner > -1.01) {
		api.setPositionTarget(myPos);
		DEBUG(("BAD COLLINEARITY DETECTED"));
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
	ZRState me;
	api.getMyZRState(me);
	return (distance(me,target) < maxDis) && (velocity(me) < maxSpeed);
}

float minDistanceFromOrigin(float target[]) {
	ZRState me;
	float path1[3], path2[3], dot, cosine;
	
	mathVecSubtract(path1,target,me,3);
	mathVecSubtract(path2,me,target,3);
	
	dot = mathVecInner(path1,me,3);
	
	cosine = dot / (mathVecMagnitude(path1, 3) * mathVecMagnitude(me, 3));
	
	if (cosine < 0) {
		return mathVecMagnitude(me, 3);
	}
	
	dot = mathVecInner(path2,target,3);
	
	cosine = dot / (mathVecMagnitude(path2, 3) * mathVecMagnitude(target, 3));
	
	if (cosine < 0) {
		return mathVecMagnitude(target,3);
	}
	
	else {
		float dis[3], negMe[3];
		
		mathVecSubtract(path1,target,me,3);
		
		for(int i = 0; i < 3; i++) {
			negMe[i] = -me[i];
		}
		
		mathVecProject(path2,negMe,path1,3);
		
		mathVecAdd(dis,me,path2,3);
		
		return mathVecMagnitude(dis,3);
	}
}
/*
float opponentTarget(){
	ZRState other;
	api.getOtherZRState(other);

	float otherVel[3], POIs[3][3], distances[3], projection[3], prep[3];

	for (int i = 0 ; i < 3 ; i++) {
		otherVel[i] = other[3 + i]*3;
		game.getPOILoc(POIs[i],i);
	}

	mathVecAdd(other,otherVel,3);

	for (int i = 0 ; i < 3 ; i++) {
		projection = mathVecProject(POIs[i],)
	}

}*/
