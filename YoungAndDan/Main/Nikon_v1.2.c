ZRState me;
int state, tempstate, POIID, picNum;
float POI[3],otherPOI[3],uploadPos[3], facing[3];
float target[3], origin[3];

float originalVecBetween[3], waypoint[3],vecBetween[3],tempTarget[3];
int gTT;

void init() {
    for(int i = 0; i < 3; i++){ 
		origin[i] = 0.00;
	}
    state = 0;
    gTT = 0;
}

void loop() {
	
	api.getMyZRState(me);
	picNum = game.getMemoryFilled();
		        
	switch (state) {
		case 0: // POI Selection
			game.getPOILoc(POI,0);
			game.getPOILoc(otherPOI,1);
			if (distance(me,otherPOI) <= distance(me,POI)) {
				POIID = 1;
				float temp[3];
				for (int i = 0; i < 3; i++) temp[i] = POI[i];
				for (int i = 0; i < 3; i++) POI[i] = otherPOI[i];
				for (int i = 0; i < 3; i++) otherPOI[i] = temp[i];
			}
			else{
			    POIID = 0;
			} 
			
			DEBUG(("POI Coors = %f,%f,%f\n",POI[0],POI[1],POI[2]));
			
			state = 1;
			break;

		case 1: //set target to outer
		    for(int i = 0; i < 3; i++){ 
		        target[i] = POI[i]*0.46/mathVecMagnitude(POI,3);
		    }
			toTarget();
			for(int i = 0; i < 3; i++){ 
		        facing[i] = -1*me[i]; 
		    }
		    DEBUG(("Outer Zone Coors = %f,%f,%f\n",target[0],target[1],target[2]));
			mathVecNormalize(facing,3);
			api.setAttitudeTarget(facing);
			if(pathToTarget(me,target,waypoint)){
	                setWaypoint(waypoint,originalVecBetween);
	                tempstate = 2;
	                state = 5;
	                for (int i = 0; i < 3; i++) tempTarget[i] = target[i];
	        }
	        else{
	            state = 2;
	        }
			break;

		case 2: // First Pic in Outer Zone
		    for(int i = 0; i < 3; i++){ 
    		        facing[i] = -1*me[i]; 
    		    }
		    mathVecSubtract(facing,POI,me,3);
		    api.setAttitudeTarget(facing);
		    toTarget();
		    if(game.alignLine(POIID)){
		        if(distance(me,origin)<0.53){
		            game.takePic(POIID);
		        }
		    }
            if(picNum > 0){
                DEBUG(("PICTURE IN OUTER ZONE TAKEN"));
                state = 3;
            }
			break;
			
        case 3: //set target to inner zone
		    for(int i = 0; i < 3; i++){ 
		        target[i] = POI[i]*0.38/mathVecMagnitude(POI,3);
		    }
			toTarget();
			for(int i = 0; i < 3; i++){ 
		        facing[i] = -1*me[i]; 
		    }
			mathVecNormalize(facing,3);
			api.setAttitudeTarget(facing);
			if(pathToTarget(me,target,waypoint)){
	                setWaypoint(waypoint,originalVecBetween);
	                tempstate = 4;
	                state = 5;
	                for (int i = 0; i < 3; i++) tempTarget[i] = target[i];
	        }
	        else{
	            state = 4;
	        }
			break;
			
		case 4: //take picture in inner zone
		    for(int i = 0; i < 3; i++){ 
		        facing[i] = -1*me[i]; 
		    }
			mathVecNormalize(facing,3);
			api.setAttitudeTarget(facing);
		    toTarget();
		    if(game.alignLine(POIID)){
		        if(distance(me,origin)<0.42){
		            game.takePic(POIID);
		        }
		    }
            if(picNum > 1){
                DEBUG(("PICTURE IN INNER ZONE TAKEN"));
                state = 6;
            }
			break;
			
		case 5: //goToTarget Seperate Case
		    if(goToWaypoint(target,waypoint,tempTarget,originalVecBetween)){
		        goToWaypoint(target,waypoint,tempTarget,originalVecBetween);
		    }
		    else{
		        state = tempstate;
		    }
		    break;
		  
		case 6:  //upload picture and reset 
		    for(int i = 0; i < 3; i++){ 
		        uploadPos[i] = me[i] / mathVecMagnitude(me, 3) * 0.65;
		    }
            if(distance(me,origin)>0.54){
                api.setPositionTarget(me);
                game.uploadPic();
            }
            else{
                api.setPositionTarget(uploadPos);
            }
            if(picNum == 0){
                DEBUG(("LOOKING FOR POI"));
                if(POIID){
                    POIID = 0;
                }
                else{
                    POIID = 1;
                }
                for (int i = 0; i < 3; i++) POI[i] = otherPOI[i];
                state = 1;
            }
		    break;
	}
}



//SIDE FUNCTIONS!!!!



bool goToWaypoint(float target[],float waypoint[],float tempTarget[], float originalVecBetween[]){
            mathVecSubtract(vecBetween, waypoint, me, 3);
	        float temp[3];
	        //for(int i = 0; i < 3; i++){
    	    //        temp[i] = 1.5*me[i+2];
    	    //}
    	    if(angleBetween(me,target) > 130 *PI / 180){
    	        dilateValue(waypoint,tempTarget, -1.2,tempTarget);
    	    }
    	    else{
    	        dilateValue(waypoint,tempTarget, -1.1,tempTarget);
    	    }
	        if(pathToTarget(me,tempTarget,temp)){
	            api.setVelocityTarget(originalVecBetween); 
	                                   //magnitude stays the same as beginning magnitude
	            DEBUG(("Going to waypoint"));
	            return true; //going to waypoint still
	        }
	        else{
	            return false; //not going to waypoint anymore
	        }
}
void setWaypoint(float waypoint[],float originalVecBetween[]){
		            float temp[3];
    	            while(pathToTarget(me,waypoint, temp)){
    	                for(int i = 0; i < 3; i++){
    	                    if(waypoint[i] >= 0){
    	                        waypoint[i] += 0.01;
    	                    }
    	                    else{
    	                        waypoint[i] -= 0.01;
    	                    }
    	                }
    	            }
    	            for(int i = 0; i < 3; i++){
    	                if(waypoint[i] >= 0){
    	                        waypoint[i] += 0.03;
    	                    }
    	                    else{
    	                        waypoint[i] -= 0.03;
    	                    }
    	            }
    	            mathVecSubtract(originalVecBetween, waypoint, me, 3); 
}
void toTarget(){
	        if(distance(me,target)>0.08){
	            haulAssTowardTarget(target,2);
	            DEBUG(("Going to Target"));
	        }
	        else{
	            api.setPositionTarget(target);
	        }
}
bool pathToTarget(float pos[], float target[],float waypoint[]){ 
    //returns true if there is something in the way
    float V1[3], V2[3], V3[3], V1p[3];
    api.getMyZRState(pos);
    mathVecSubtract(V1, target, pos, 3);
    for(int i = 0; i < 3; i++){
        V2[i] = pos[i];
    }
    for(int i = 0; i < 3; i++){
        V1p[i] = V1[i] * mathVecInner(V2,V1,3) / mathVecInner(V1,V1,3);  
    }
    mathVecSubtract(V3, V2, V1p, 3);
    if(mathVecMagnitude(V3,3) < 0.31){
        for(int i = 0; i < 3; i++){
            if(V3[i] >= 0){
                waypoint[i] = V3[i] * (0.31 / mathVecMagnitude(V3,3)) + 0.05;
            }
            else{
                waypoint[i] = V3[i] * (0.31 / mathVecMagnitude(V3,3)) - 0.05;
            }
            
        }  
        return true;
    }
    else{
        return false;
    }
}
void dilateValue(float pt1[3], float pt2[3], float dilation , float dst[3]){
    for(int i=0; i < 3; i++){
        dst[i] = dilation * (pt1[i] - pt2[i]) + pt1[i];
    }
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
float distance(float p1[], float p2[]){
    float d=0;
    for(int i = 0; i<3; i++){
        d += (p2[i]-p1[i])*(p2[i]-p1[i]);
    }
    return sqrtf(d);
}
