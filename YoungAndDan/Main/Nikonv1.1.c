ZRState me;
int state, POIID, picNum;
float POI[3],otherPOI[3],uploadPos[3], facing[3];
float target[3];

//for goToTarget
float originalVecBetween[3], vecBetween[3];
int gTT;
//

void init() {
    state = 0;
    gTT = 0;
}

void loop() {
	
	api.getMyZRState(me);
	picNum = game.getMemoryFilled();
	float origin[3];
	for(int i = 0; i < 3; i++){ 
		origin[i] = 0.00;
	}
		        
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
			goToTarget(target);
			for(int i = 0; i < 3; i++){ 
		        facing[i] = -1*me[i]; 
		    }
			mathVecNormalize(facing,3);
			api.setAttitudeTarget(facing);
			state = 2;
			break;

		case 2: // First Pic in Outer Zone
		    for(int i = 0; i < 3; i++){ 
    		        facing[i] = -1*me[i]; 
    		    }
		    if(distance(me,target)>0.08){
		        for(int i = 0; i < 3; i++){ 
    		        facing[i] += 0.05; 
    		    }
    			mathVecNormalize(facing,3);
    			api.setAttitudeTarget(facing);
		        goToTarget(target);
		    }
		    else{
		        mathVecSubtract(facing,POI,me,3);
		        api.setAttitudeTarget(facing);
		        api.setPositionTarget(target);
		    }
		    if(game.alignLine(POIID)){
		        float origin[3];
		        for(int i = 0; i < 3; i++){ 
		            origin[i] = 0.00;
		        }
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
			goToTarget(target);
			for(int i = 0; i < 3; i++){ 
		        facing[i] = -1*me[i]; 
		    }
			mathVecNormalize(facing,3);
			api.setAttitudeTarget(facing);
			state = 4;
			break;
			
		case 4: //take picture in inner zone
		    for(int i = 0; i < 3; i++){ 
		        facing[i] = -1*me[i]; 
		    }
			mathVecNormalize(facing,3);
			api.setAttitudeTarget(facing);
		    if(distance(me,target)>0.08){
		        goToTarget(target);
		    }
		    else{
		        api.setPositionTarget(origin);
		    }
		    if(game.alignLine(POIID)){
		        if(distance(me,origin)<0.42){
		            game.takePic(POIID);
		        }
		    }
            if(picNum > 1){
                DEBUG(("PICTURE IN INNER ZONE TAKEN"));
                state = 5;
            }
			break;
			
		case 5:
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
