// The Dencheng Code: Epic Swag Time

/*
In my eyes, hydrochloric acid can be quite irritating.
GNUForce?  More like ... GN00BForce, amirite?
--Dennis 2k14
*/

ZRState me;
bool oldPOIIDs[3];
int state, POIID, solarFlareBegin, picNum;
float POI[3],otherPOI1[3],otherPOI2[3],destination[3],facing[3];

void init() {

	solarFlareBegin = 1000;
	//Just to make the solar storm evasion code neater
	
	for (int i = 0; i < 3; i++) oldPOIIDs[i] = false;
	//Just to make the POI selection code neater
	
}

void loop() {
	
	api.getMyZRState(me);

	if(api.getTime() % 60 == 0 && state != 3) {
	    state = 0;
	    for (int i = 0; i < 3; i++) oldPOIIDs[i] = false;
	}

	rebootIfStorm();
	
	DEBUG(("I am in state %d.\n", state));

	DEBUG(("I can still take %d photos.\n", game.getMemorySize() - game.getMemoryFilled()));
	
	if (state == 0) {// POI Selection
	
		game.getPOILoc(POI,0);
		game.getPOILoc(otherPOI1,1);
		game.getPOILoc(otherPOI2,2);
		
		if (!oldPOIIDs[0] || !oldPOIIDs[1] || !oldPOIIDs[2]) {
		    //if you haven't already been to all the POIs
		    if (oldPOIIDs[0]) for (int i = 0; i < 3; i++) POI[i] = 1000;
		    if (oldPOIIDs[1]) for (int i = 0; i < 3; i++) otherPOI1[i] = 1000;
		    if (oldPOIIDs[2]) for (int i = 0; i < 3; i++) otherPOI2[i] = 1000;
		    //make sure you don't go to a previously-visited POI
		}
		
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
	
		state = 1;
		
	}

	if (state == 1) {// Take pic in outer zone
	
		mathVecSubtract(facing,POI,me,3);
		    
    	if (mathVecMagnitude(me,3) > 0.42 && mathVecMagnitude(me,3) < 0.53 && angleBetween(me, facing) > 2.94159265359) {
    	    // in the outer zone and angle between me and POI is less than 0.2
	        picNum = game.getMemoryFilled();
		    game.takePic(POIID);
		    if (game.getMemoryFilled() - picNum == 1) {
	           DEBUG(("I just took a picture in the outer zone.\n"));
	           if (solarFlareBegin - api.getTime() > 13) state = 2;
	           else state = 3;
	        }
		}
		    
		else {
		    for (int i = 0; i < 3; i++) {
		        destination[i] = POI[i] * 0.43 / mathVecMagnitude(POI, 3);
		    }
            if (mathVecMagnitude(facing,3) > 0.5) setPositionTarget(destination, 2);
            else setPositionTarget(destination);
		}
		
		mathVecNormalize(facing,3);
        api.setAttitudeTarget(facing);
    	
	}

	if (state == 2) {// Take pic in inner zone
	
	    mathVecSubtract(facing,POI,me,3);
	
        if (mathVecMagnitude(me,3) < 0.42 && angleBetween(me, facing) > 2.74159265359) {
            //in the inner zone and angle between me and POI is less than 0.4
	        picNum = game.getMemoryFilled();
		    game.takePic(POIID);
		    if (game.getMemoryFilled() - picNum == 1) {
		        DEBUG(("I just took a picture in the inner zone.\n"));
		        state = 3;
		    }
        }
        
        else {
    	    for (int i = 0; i < 3; i++) {
    	        destination[i] = POI[i] * 0.33 / mathVecMagnitude(POI, 3);
    	    }
            if (mathVecMagnitude(facing,3) > 0.5) setPositionTarget(destination, 2);
            else setPositionTarget(destination);
    	}
    	
    	mathVecNormalize(facing,3);
        api.setAttitudeTarget(facing);
		
	}

	if (state == 3) {// Upload the picture
	
		if (mathVecMagnitude(me,3) > 0.53) {
		    picNum = game.getMemoryFilled();
			game.uploadPic();
			DEBUG(("I just uploaded %d picture(s).\n", (picNum - game.getMemoryFilled())));
			state = 0;
			oldPOIIDs[POIID] = true;
		}
	
		else {
			for (int i = 0; i < 3; i++) {
	        	destination[i] = me[i] * 0.61 / mathVecMagnitude(me,3);
	        }
	        if (solarFlareBegin - api.getTime() < 8) setPositionTarget(destination, 4);
            else if (mathVecMagnitude(me,3) < 0.5) setPositionTarget(destination, 2);
            else setPositionTarget(destination);
		}

	}
	
}

void rebootIfStorm() {
    
    if (api.getTime() == (solarFlareBegin - 1)) {
	    DEBUG(("I shall now reboot.\n"));
        setPositionTarget(me, 20); //prevents crashing during reboot
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

float minDistanceIfDilated(float p1[], float p2[]) {
    //returns the smallest possible distance between p2 and a dilation of p1
    //this is actually just the distance from a point to a line in 3-space
    float cross[3];
    mathVecCross(cross,p2,p1);
    if (mathVecMagnitude(p1,3) / 10 == 0) {
        DEBUG(("DIVISION BY ZERO WHILE DILATING!"));
    }
    return mathVecMagnitude(cross,3) / mathVecMagnitude(p1,3);
}

void mathVecProject(float c[], float a[], float b[], int n) {
    // finds the projection of a onto b, puts the result in c
    if (mathVecMagnitude(b,3) * mathVecMagnitude(b,3) / 10 == 0) {
        DEBUG(("DIVISION BY ZERO WHILE PROJECTING!"));
    }
    for (int i = 0; i < n; i++) {
        c[i] = (mathVecInner(a,b,3) * b[i]) / (mathVecMagnitude(b,3) * mathVecMagnitude(b,3));
    }
}

float angleBetween(float a[], float b[]) {
    //returns the measure of the angle between vectors a and b
    return acosf(mathVecInner(a, b, 3) / (mathVecMagnitude(a,3) * mathVecMagnitude(b,3)));
}

float angle(float a[], float b[], float c[]) {
    //returns the measure of angle abc
    float side1[3], side2[3], cosine;
    
    for (int i = 0; i < 3; i++) side1[i] = a[i] - c[i];
    cosine = - mathVecMagnitude(side1,3) * mathVecMagnitude(side1,3);
    for (int i = 0; i < 3; i++) side1[i] = b[i] - a[i];
    for (int i = 0; i < 3; i++) side2[i] = c[i] - b[i];
    cosine += mathVecMagnitude(side1,3) * mathVecMagnitude(side1,3) + mathVecMagnitude(side2,3) * mathVecMagnitude(side2,3);
    if (mathVecMagnitude(side1,3) * mathVecMagnitude(side2,3) / 10 == 0) {
        DEBUG(("DIVISION BY ZERO WHILE FINDING ANGLE!"));
    }
    cosine /= 2 * mathVecMagnitude(side1,3) * mathVecMagnitude(side2,3);
    
    return acosf(cosine);
}

float minDistanceFromOrigin(float target[]) {
	float temp[3] = {0,0,0}; //temp is the origin
	
	if (cosf(angle(temp,me,target)) < 0) { //going away from origin
		return mathVecMagnitude(me, 3);
	}
	
	else if (cosf(angle(temp,target,me)) < 0) { //going in direction of origin
		return mathVecMagnitude(target,3);
	}
	
	else {
	    mathVecSubtract(temp,target,me,3);
	    mathVecProject(temp,me,temp,3);
		mathVecSubtract(temp,me,temp,3);
		
		return mathVecMagnitude(temp,3);
	}
}

void setPositionTarget(float target[3]) {
	float temp[3];
	
	if (minDistanceIfDilated(target,me) < 0.01) {
	    //if target is really close to me or a dilation of me
		api.setPositionTarget(target);
	    //avoids divide-by-0 error when finding angle between me and target
	}
	
	else if (minDistanceFromOrigin(target) > 0.32) {
		api.setPositionTarget(target);
	}

	else if (mathVecMagnitude(me,3) < 0.32) {
		for (int i = 0; i < 3; i++) temp[i] = me[i] * 0.6 / mathVecMagnitude(me,3);
		api.setPositionTarget(temp);
		DEBUG(("DANGER! Asteroid Collision Imminent!\n"));
	}
	
	else {
		float newTarget[3];
		
		mathVecProject(temp,target,me,3);
		mathVecSubtract(temp,target,temp,3);
		//temp is orthogonal to me and located in the plane containing me and target
		
		for (int i = 0; i < 3; i++) {
		    newTarget[i] = temp[i] / mathVecMagnitude(temp,3);
			newTarget[i] *= sqrtf(1 / (1/(0.35 * 0.35) - 1/(mathVecMagnitude(me,3) * mathVecMagnitude(me,3))));
		}
		//newTarget is temp resized so that the altitude of right triangle Me-Origin-newTarget is 0.35
		
		mathVecSubtract(temp,newTarget,me,3);
		//temp goes from me to newTarget
		
		mathVecSubtract(newTarget,target,me,3); 
		for (int i = 0; i < 3; i++) temp[i] *= mathVecMagnitude(newTarget,3);
		//temp is resized so that it is as long as the original distance from me to target
		
		mathVecAdd(newTarget,me,temp,3);
        api.setPositionTarget(newTarget);
		DEBUG(("Goddammit, there's an asteroid in the way!\n"));
	}
}

void setPositionTarget(float target[3], float multiplier) {
    float temp[3];
    
    mathVecSubtract(temp,target,me,3);
    for (int i = 0; i < 3; i++) temp[i] = me[i] + temp[i] * multiplier;
    setPositionTarget(temp);
    DEBUG(("Hauling ass! (*%.1f)\n", multiplier));
}
