//Declare any variables shared between functions here
int state, POIID, ydist0, ydist1, ydist2;
float POI[3], POI0[3], POI1[3], POI2[3],TOPProj[3], facing[3];
ZRState me;

void init(){
    
	state = 0;
	POIID = 0;
	
    for (int i = 0 ; i < 3 ; i++) {
        POI[i] = 0;
        POI0[i] = 0;
        POI1[i] = 0;
        POI2[i] = 0;
        facing[i] = 0;
    }
    
    facing[0] = -0.05;
    
    TOPProj[0] = -0.1;
    TOPProj[1] = 0;
    TOPProj[2] = -0.3;
}

void loop(){
    
    DEBUG(("%d",state));
    
    api.getMyZRState(me);
    
	switch(state) {
	    
	    case 0: // chosing the POI an go to the top of the asteroid
	        
	        game.getPOILoc(POI0, 0);
	        game.getPOILoc(POI1, 1);
	        game.getPOILoc(POI2, 2);
	        
	        if (POI0[1] == 0) {
	            POIID = 0;
	            for (int i = 0 ; i < 3 ; i++) {
	                POI[i] = POI0[i];
	            }
	        }
	        else if (POI1[1] == 0) {
	            POIID = 1;
	            for (int i = 0 ; i < 3 ; i++) {
	                POI[i] = POI1[i];
	            }
	        }
	        else if (POI2[1] == 0) {
	            POIID = 2;
	            for (int i = 0 ; i < 3 ; i++) {
	                POI[i] = POI2[i];
	            }
	        }
	        
	        api.setPositionTarget(TOPProj);
			mathVecSubtract(facing,POI,me,3);
			mathVecNormalize(facing,3);
			api.setAttitudeTarget(facing);
	        
	        if (distance(me,TOPProj) < 0.1) {
	            state = 1;
	        }
	        
	        break;
	    
	    case 1: // TRY to take pic
	        if (game.alignLine(POIID)) {
	            game.takePic(POIID);
	            DEBUG(("I DID IT!!!"));
	        }
	        
	        else {
	            api.setPositionTarget(TOPProj);
			    mathVecSubtract(facing,POI,me,3);
			    mathVecNormalize(facing,3);
			    api.setAttitudeTarget(facing);
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

void mathVecProject(float c[], float a[], float b[], int n) {
    // finds the projection of a onto b, puts the result in c
    if (mathVecMagnitude(b,3) * mathVecMagnitude(b,3) / 10 == 0) {
        DEBUG(("DIVISION BY ZERO WHILE PROJECTING!"));
    }
    for (int i = 0; i < n; i++) {
        c[i] = (mathVecInner(a,b,3) * b[i]) / (mathVecMagnitude(b,3) * mathVecMagnitude(b,3));
    }
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

void setPositionTarget(float target[3], float multiplier) {
	api.getMyZRState(me);
	
	float myPos[3],meMag;
	
	for(int i = 0; i < 3; i++) {
		myPos[i] = me[i];
	}
	
	meMag = mathVecMagnitude(myPos,3);
	
	if (minDistanceFromOrigin(target) > 0.31) {
		if (distance(me, target) < 0.4) { // Save braking distance
			api.setPositionTarget(target);
		}

		else { // Or haul ass towards target
			float temp[3];

			mathVecSubtract(temp,target,me,3);
			
			for (int i = 0 ; i < 3 ; i++) {
				temp[i] = me[i] + temp[i] * multiplier;
			}

			api.setPositionTarget(temp);
		}

		DEBUG(("GOING STRAIGHT\n"));
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
			path[i] = path[i] * multiplier;
		}
		
		mathVecAdd(temp,myPos,path,3);

		api.setPositionTarget(temp);
		
		DEBUG(("TAKING THE TANGENT\n"));
	}
}
