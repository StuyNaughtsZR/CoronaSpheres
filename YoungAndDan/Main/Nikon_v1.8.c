ZRState me;
int state, tempstate, POIID, picNum, solarFlareBegin;
float POI[3], uploadPos[3], facing[3];
float target[3], origin[3];
int goodPOI[3]; //This array says which POIS are able to be gone to
float originalVecBetween[3], waypoint[3],vecBetween[3],tempTarget[3];

void init() {
    for(int i = 0; i < 3; i++){
        origin[i] = 0.00;
    }
    goodPOI[0] = 1;
    goodPOI[1] = 1;
    goodPOI[2] = 1;
    POIID = -1;
    solarFlareBegin = 1000;
    state = 0;
}

void loop() {
    api.getMyZRState(me);
    picNum = game.getMemoryFilled();
    DEBUG(("%d picture(s) have been taken\n", picNum));
    DEBUG(("STATE = %d\n",state));
    DEBUG(("ARRAY = %d,%d,%d\n",goodPOI[0],goodPOI[1],goodPOI[2]));
    if((api.getTime() % 60 == 0)&&(api.getTime() > 10)){
        goodPOI[0] = 1;
        goodPOI[1] = 1;
        goodPOI[2] = 1;
        state = 6;
    }
    if(distance(me,origin)<0.36){
	    DEBUG(("Too Close!"));
	    for(int i = 0; i < 3; i++){ 
		        uploadPos[i] = me[i] / mathVecMagnitude(me, 3) * 0.65;
		}
		haulAssTowardTarget(uploadPos,8);
	}
	if (api.getTime() > 226) { 
	    if((state == 3)||(state == 4)){ 
	        state = 6; 
	    } 
	}
	if (api.getTime() + 5 % 60 == 0){
	    if((state == 0)||(state == 1)||(state == 2)){
	        state = 8;
	    }
	    if(state == 3){
	        goodPOI[POIID] = 0;
	        state = 6;
	    }
	}
    flareAvoid();

    switch (state) {
        case 0: // POI Selection
        closestPOI(goodPOI,POI);
        DEBUG(("POI Coors = %f,%f,%f\n",POI[0],POI[1],POI[2]));
        state = 1;
        break;

        case 1: //set target to outer
        for(int i = 0; i < 3; i++){
            target[i] = POI[i]*0.475/mathVecMagnitude(POI,3);
        }
        toTarget();
        DEBUG(("Outer Zone Coors = %f,%f,%f\n",target[0],target[1],target[2]));
        mathVecSubtract(facing,POI,me,3);
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
            mathVecSubtract(facing,POI,me,3);
            mathVecNormalize(facing,3);
            api.setAttitudeTarget(facing);
            api.setAttitudeTarget(facing);
            toTarget();
            if(game.alignLine(POIID)&&(distance(me,origin)>0.475)&&(distance(me,origin)<0.53)){
                game.takePic(POIID);
    		}
            if(picNum > 0){
                DEBUG(("PICTURE IN OUTER ZONE TAKEN"));
                goodPOI[POIID] = 0;
                state = 3;
            }
            break;

        case 3: //set target to inner zone
            for(int i = 0; i < 3; i++){ 
		        target[i] = POI[i]*0.39/mathVecMagnitude(POI,3);
		    }		
		    target[0] += 0.008;
            toTarget();
            mathVecSubtract(facing,POI,me,3);
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
            mathVecSubtract(facing,POI,me,3);
            mathVecNormalize(facing,3);
            api.setAttitudeTarget(facing);
            if(distance(me,target)>0.06){
                haulAssTowardTarget(target,2);
            }
            else{
                api.setPositionTarget(target);
            }
            if(game.alignLine(POIID)){
		        if(distance(me,origin)<0.42){
		            game.takePic(POIID);
		        }
		    }
            if(picNum > 1){
                DEBUG(("PICTURE IN INNER ZONE TAKEN"));
                goodPOI[POIID] = 0;
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

            case 6: //upload picture and reset
                for(int i = 0; i < 3; i++){
                    uploadPos[i] = me[i] / mathVecMagnitude(me, 3) * 0.65;
                }
                if(distance(me,origin)>0.54){
                    api.setPositionTarget(me);
                    game.uploadPic();
                }
                else{
                    if(distance(me,origin)>0.51){
                        api.setPositionTarget(uploadPos);
                    }
                    else{
                        haulAssTowardTarget(uploadPos,1.8);
                    }
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
    mathVecSubtract(originalVecBetween, waypoint, me, 3);
}
bool goToWaypoint(float target[],float waypoint[],float tempTarget[], float originalVecBetween[]){
    mathVecSubtract(vecBetween, waypoint, me, 3);
    float temp[3];
    if(angleBetween(me,target) > 150 *PI / 180){
        dilateValue(waypoint,tempTarget, -1.15,tempTarget);
    }
    else{
        dilateValue(waypoint,tempTarget, -1.05,tempTarget);
    }
    if(pathToTarget(me,tempTarget,temp)){
        api.setVelocityTarget(originalVecBetween);
//magnitude stays the same as beginning magnitude
        return true; //going to waypoint still
    }
    else{
        return false; //not going to waypoint anymore
    }
}
void toTarget(){
    if(distance(me,target)>0.08){
        haulAssTowardTarget(target,2);
    }
    else{
        api.setPositionTarget(target);
    }
}
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
    float shortestdistance = 50.00; //placeholder number
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
        if(api.getTime() > (solarFlareBegin - 4)&&(picNum == 0)){
    		state = 7;
        }
        else{
            if(api.getTime() > (solarFlareBegin - 6)&&(picNum == 0)){
                state = 8;
            }
            else if(picNum == 1){
                goodPOI[POIID] = 0;
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
