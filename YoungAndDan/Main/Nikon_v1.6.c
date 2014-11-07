//STUFFS TO DO
//IMPROVE GOTOTARGET
//OPTIMIZATION IN OUTER TO UPLOAD
//if(distance(me,origin) < 0.313)
//change attitude target when turning off
//if POI change about to happen don't go for pictures
//out of bounds emergency 

ZRState me;
ZRState enemy;
int state, tempstate, POIID, picNum, solarFlareBegin;
float POI[3], uploadPos[3], facing[3];
float target[3], origin[3];
int goodPOI[3]; //This array says which POIS are able to be gone to
bool first;
int attempts;


float originalVecBetween[3], waypoint[3],vecBetween[3],tempTarget[3];

void init() {
    for(int i = 0; i < 3; i++){ 
		origin[i] = 0.00;
	}
	goodPOI[0] = 1;
	goodPOI[1] = 1;
	goodPOI[2] = 1;
	POIID = -1;
	first = true;
	
	attempts = 0;
	solarFlareBegin = 1000;
	DEBUG(("Greetings from Stuy-Naught"));
    state = 0;
}

void loop() {
	
	api.getMyZRState(me);
	api.getMyZRState(enemy);
	picNum = game.getMemoryFilled();
	DEBUG(("%d picture(s) have been taken\n", picNum));
	DEBUG(("STATE = %d\n",state));
	DEBUG(("Attempts = %d\n", attempts));
	if(distance(me,origin)<0.36){
	    for(int i = 0; i < 3; i++){ 
		        uploadPos[i] = me[i] / mathVecMagnitude(me, 3) * 0.65;
		}
		api.setPositionTarget(uploadPos);
	}
	if((api.getTime() % 60 == 0)&&(api.getTime() > 10)){
	    goodPOI[0] = 1;
        goodPOI[1] = 1;
        goodPOI[2] = 1;
	    state = 6;
	}
	if (api.getTime() > 226) { 
	    if(state == 3){ 
	        state = 6; 
	    } 
	}
	if (api.getTime() + 5 % 60 == 0){
	    if((state == 0)||(state == 1)||(state == 2)){
	        state = 8;
	    }
	    if(state == 3){
	        state = 6;
	    }
	}
	flareAvoid();
		        
	switch (state) {
		case 0: // POI Selection
			closestPOI(goodPOI,POI);
			DEBUG(("POI Coors = %f,%f,%f\n",POI[0],POI[1],POI[2]));
			if(first){
			    state = 3;
			}
			else{
			    state = 1;
			}
			break;

		case 1: //set target to outer
		    if(first){
		        for(int i = 0; i < 3; i++){ 
    		        target[i] = POI[i]*0.48/mathVecMagnitude(POI,3);
    		    }    
		    }
		    else{
    		    for(int i = 0; i < 3; i++){ 
    		        target[i] = POI[i]*0.475/mathVecMagnitude(POI,3);
    		    }
		    }
		    target[0] += 0.002;
			setPositionTarget(target,3);
			mathVecSubtract(facing,POI,me,3);
			mathVecNormalize(facing,3);
			api.setAttitudeTarget(facing);
			setPositionTarget(target,2.5);
			state = 2;
			break;

		case 2: // First Pic in Outer Zone
		    mathVecSubtract(facing,POI,me,3);
			mathVecNormalize(facing,3);
			api.setAttitudeTarget(facing);
		    if(first){
		        if(distance(me,target)>0.04){
    	            setPositionTarget(target,3);
    	        }
    	        else{
    	            setPositionTarget(target);
    	        }
		    }
		    else{
    			if(distance(me,target)>0.05){
    	            setPositionTarget(target,2);
    	        }
    	        else{
    	            setPositionTarget(target);
    	        }
		    }
    		if(game.alignLine(POIID)){
    		    attempts++;
    		    if(first&&(distance(me,origin)>0.48)){
                    game.takePic(POIID);
    		    }
                game.takePic(POIID);
                if((distance(me,origin)<0.475)&&(!first)){
                    first = true;
                    state = 1;
                }
    		}
		    if(first){
		        if(picNum == 2){
		            attempts = 0;
                    DEBUG(("PICTURE IN OUTER TAKEN"));
                    goodPOI[POIID] = 0;
                    first = false;
                    state = 6;
                }
		    }
		    else{
                if(picNum > 0){
                    attempts = 0;
                    DEBUG(("PICTURE IN OUTER ZONE TAKEN"));
                    goodPOI[POIID] = 0;
                    state = 3;
                }
		    }
			break;
			
        case 3: //set target to inner zone
		    for(int i = 0; i < 3; i++){ 
		        target[i] = POI[i]*0.38/mathVecMagnitude(POI,3);
		    }		
		    target[0] += 0.008;
		    setPositionTarget(target);
			mathVecSubtract(facing,POI,me,3);
			mathVecNormalize(facing,3);
			api.setAttitudeTarget(facing);
			setPositionTarget(target,3);
			state = 4;
			break;
			
		case 4: //take picture in inner zone
		    mathVecSubtract(facing,POI,me,3);
			mathVecNormalize(facing,3);
			api.setAttitudeTarget(facing);
		    if(first){
		        if(distance(me,target)>0.05){
    	            setPositionTarget(target,2.5);
    	        }
    	        else{
    	            setPositionTarget(target);
    	        }
		    }
		    else{
    			if(distance(me,target)>0.02){
    	            setPositionTarget(target,4);
    	        }
    	        else{
    	            setPositionTarget(target);
    	        }
		    }
		    if(game.alignLine(POIID)){
		        attempts++;
		        if(distance(me,origin)<0.42){
		            game.takePic(POIID);
		        }
		    }
		    if(first){
		        if(picNum == 1){
		            attempts = 0;
                    DEBUG(("PICTURE IN INNER ZONE TAKEN"));
                    state = 1;
                }
		    }
		    else{
                if(picNum > 1){
                    attempts = 0;
                    DEBUG(("PICTURE IN INNER ZONE TAKEN"));
                    state = 6;
                }
		    }

			break;
		  
		case 6:  //upload picture and reset 
		    for(int i = 0; i < 3; i++){ 
		        uploadPos[i] = me[i] / mathVecMagnitude(me, 3) * 0.65;
		    }
            game.uploadPic();
                if(distance(me,origin)>0.45){
                    api.setPositionTarget(uploadPos);
                }
                else{
                    setPositionTarget(uploadPos, 8);
                }
            if(picNum == 0){
                DEBUG(("LOOKING FOR POI"));
                state = 0;
            }
		    break;
		case 7: //stop because of flare
		    api.setPositionTarget(me);
		    for(int i = 0; i < 3; i++){ 
		        facing[i] = me[i+3];
		    }
		    mathVecNormalize(facing,3);
			api.setAttitudeTarget(facing);
		    break;
		    
		case 8: //getInPosition for next cycle
		    float temp[3];
		    target[0] = -0.35;
		    target[1] = 0.00;
		    target[2] = 0.00;
		    mathVecSubtract(temp,target,me,3);
		    for(int i = 0; i < 3; i++){ 
		        temp[i] = 0.02*temp[i]/mathVecMagnitude(temp,3);
		    }
		    api.setVelocityTarget(temp);
		    break;
	}
}

//SIDECHAINS
void closestPOI(int goodPOIS[], float nextPOI[]){ 
    //nextPOI should be 3 float array
    float distances[3] = {0.00, 0.00, 0.00};
    for(int i = 0; i<3; i++){
        if(goodPOIS[i] == 0){
            distances[i] = 100.00; //absurd distance which will not appear so the already gone to POI is avoided
        }
        else{
            float POIN[3] = {0.00, 0.00, 0.00};
            game.getPOILoc(POIN, i);
            distances[i] = distance(me,POIN);
        }
    }
    float shortestdistance = 50.00;  //placeholder number
    for(int i = 0; i<3; i++){
        if (distances[i]<= shortestdistance){
            shortestdistance = distances[i];
            POIID = i;
        } 
    }
    game.getPOILoc(nextPOI,POIID);
}
void flareAvoid(){
	if((api.getTime() == (solarFlareBegin - 1))&&(picNum == 0)){
	    DEBUG(("I shall now reboot.\n"));
	    if((solarFlareBegin+1)%60 < 6){
	        goodPOI[0] = 1;
            goodPOI[1] = 1;
            goodPOI[2] = 1;
	    }
	    api.setPositionTarget(me);
		for(int i = 0; i < 3; i++){ 
		   facing[i] = me[i+3];
		}
		mathVecNormalize(facing,3);
		api.setAttitudeTarget(facing);
		game.turnOff();
		game.turnOn();
		state = 0;
	}
    else if ((api.getTime() > (solarFlareBegin - 18))&&(api.getTime() < (solarFlareBegin))){
        if(api.getTime() > (solarFlareBegin - 6)&&(picNum == 0)){
    		state = 7;
        }
        else{
            if(picNum == 0){
                state = 8;
            }
            else{
                state = 6;
            }
        }
	}
	else if (game.getNextFlare() != -1) {
	    solarFlareBegin = api.getTime() + game.getNextFlare();
	    DEBUG(("Next solar flare will occur at %ds.\n", solarFlareBegin));
	}
}

float distance(float p1[], float p2[]){
    float d=0;
    for(int i = 0; i<3; i++){
        d += (p2[i]-p1[i])*(p2[i]-p1[i]);
    }
    return sqrtf(d);
}
float angleBetween(float pt1[3], float pt2[3]){
    float dot;
    float origin[3], vectorBetweenS1[3], vectorBetweenS2[3];
    origin[0] = 0;
    origin[1] = 0;
    origin[2] = 0;
    mathVecSubtract(vectorBetweenS1,pt1, origin ,3);
    mathVecNormalize(vectorBetweenS1,3);
    mathVecSubtract(vectorBetweenS2,pt2,origin,3);
    mathVecNormalize(vectorBetweenS2,3);
    dot = mathVecInner(vectorBetweenS1, vectorBetweenS2, 3);
    return acosf(dot);
}
void haulAssTowardTarget(float target[], float scalar) {
    // makes you go in the direction of target, but scalar times faster
    float scaledTarget[3];
    for (int i = 0; i < 3; i++) scaledTarget[i] = me[i] + scalar * (target[i] - me[i]);
    api.setPositionTarget(scaledTarget);
}
void dilateValue(float pt1[3], float pt2[3], float dilation , float dst[3]){
    for(int i=0; i < 3; i++){
        dst[i] = dilation * (pt1[i] - pt2[i]) + pt1[i];
    }
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
