// Yeech's Code
// Not to be confused with the Vinci Code

ZRState me;
int state, POIID, picNum, solarFlareBegin;
float target1[3],target2[3],shadow[3],timei1,timef1,fueli2,fuelf2;

void init() {

	state = 0;

	shadow[0] = 0.35;
	shadow[1] = 0.1;
	shadow[2] = 0;

	target1[0] = -0.292;
	target1[1] = -0.121;
	target2[2] = 0;
	
	target2[0] = -0.43;
	target2[1] = -0.146;
	target2[2] = 0;


	solarFlareBegin = 1000; //Just to make the solar storm evasion code neater

}

void loop() {

/*
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
				uploadCalc(uploadPos,me);
				api.setPositionTarget(uploadPos);
				state = 5;
			}
			break;

		case 3: // Moving to outer zone
			for (int i = 0; i < 3; i++) {
				newBrakingPos[i] = POI[i] * 2.5;
			}
			
			if (AreWeThereYet(newBrakingPos,0.01,0.01)) state = 4;
			else {
			    api.setPositionTarget(newBrakingPos);
			    mathVecSubtract(facing,POI,me,3);
			    mathVecNormalize(facing,3);
			    api.setAttitudeTarget(facing);
			    game.takePic(POIID);
			}
			break;
		
		case 4: //Taking pic in outer zone
			api.setPositionTarget(newBrakingPos);
			mathVecSubtract(facing,POI,me,3);
			mathVecNormalize(facing,3);
			api.setAttitudeTarget(facing);
            game.takePic(POIID);

			if (picNum > 1) {
				DEBUG(("%d picture(s) have been taken\n", picNum));
				uploadCalc(uploadPos,me);
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

				mathVecSubtract(facing,POI,me,3);
				mathVecNormalize(facing,3);
				api.setAttitudeTarget(facing);

				api.setPositionTarget(uploadPos);
				game.takePic(POIID);
			}

			break;
	}
	*/
	
	switch (state) {

		case 0:
			
			setPositionTarget(target1);

			if (AreWeThereYet(target1,0.05,0.05)) {
				state = 1;
			}

			//break;

		case 1:
			timei1 = api.getTime();
			fueli1 = game.getFuelRemaining();

			setPositionTarget(shadow);

			if(AreWeThereYet(shadow,0.05,0.05)) {
				timef1 = api.getTime();
				fuelf1 = game.getFuelRemaining();
				DEBUG(("TIME TOOK = %f \n FUEL TOOK = %f",timef1-timei1, fuelf1-fueli1));
			state = 2;
			}

		case 2:
			timei = api.getTime();
			fueli = game.getFuelRemaining();

			setPositionTarget(target2);

			if(AreWeThereYet(target2,0.05,0.05)) {
				timef = api.getTime();
				fuelf1 = game.getFuelRemaining();
				DEBUG(("TIME TOOK = %f \n FUEL TOOK = %f",timef1-timei1, fuelf1-fueli1));

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

void uploadCalc(float uploadPos[], float POI[]){
	mathVecNormalize(me,3);
	for (int i = 0; i < 3; i++) {
		uploadPos[i] = me[i] * 0.55;
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

	if (minDistanceFromAsteroid(target) > 0.30) {
		api.setPositionTarget(target);
		DEBUG(("JUST GO!!!"));
	}
	
	else if (meMag >= 0.22 && meMag <= 0.32) {
		for (int i = 0; i < 3; i++) {
			myPos[i] = myPos[i] * 1.6;
		}
		
		api.setPositionTarget(myPos);
		DEBUG(("TOO CLOSE"));
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
			mePrep[i] = (mePrep[i] * 0.32 * meMag) / (sqrtf(meMag*meMag - 0.32*0.32));
		}
		
		mathVecSubtract(path,mePrep,myPos,3);
		
		for (int i = 0; i < 3; i++) {
			path[i] = path[i] * 2;
		}
		
		mathVecAdd(temp,myPos,path,3);

		api.setPositionTarget(temp);
		
		DEBUG(("TAKING THE TANGENT"));
	}
}

int AreWeThereYet(float target[3], float maxDis, float maxSpeed) {
	ZRState me;
	api.getMyZRState(me);
	return (distance(me,target) < maxDis) && (velocity(me) < maxSpeed);
}

float minDistanceFromOrigin(float target[]) {
	float proj[3], meToTarget[3], testPoint[3];
    if ((mathVecMagnitude(me, 3) * mathVecMagnitude(me, 3) +
    mathVecMagnitude(meToTarget, 3) * mathVecMagnitude(meToTarget, 3) -
    mathVecMagnitude(target, 3) * mathVecMagnitude(target, 3)) /
    (2 * mathVecMagnitude(me, 3) * mathVecMagnitude(ToTarget, 3)) < 0 || 
	(mathVecMagnitude(target, 3) * mathVecMagnitude(target, 3) +
    mathVecMagnitude(meToTarget, 3) * mathVecMagnitude(meToTarget, 3) -
    mathVecMagnitude(me, 3) * mathVecMagnitude(me, 3)) /
    (2 * mathVecMagnitude(target, 3) * mathVecMagnitude(meToTarget, 3)) < 0)
		return 10;    // if ∠ target me origin or ∠ me target origin is obtuse, return 10
    mathVecSubtract(meToTarget, target, me, 3);
    mathVecProject(proj, me, meToTarget, 3);
    mathVecSubtract(testPoint, me, proj, 3);
    return mathVecMagnitude(testPoint,3);
}
