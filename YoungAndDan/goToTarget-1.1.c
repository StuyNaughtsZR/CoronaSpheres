ZRState me;
float target[3], origin[3], originalVecBetween[3], vecBetween[3], myPos[3], shadow1[3];
float V1[3], V1p[3], V2[3], V3[3], waypoint[3], originalVecToTarget[3];
int state;
float originalDistance, distanceBetweenWS, distanceBetweenTS, lambda;
float tempTarget[3];


bool testing;
float startPos[3];

void init(){
	origin[0] = 0.0;
	origin[1] = 0.0;
	origin[2] = 0.0;
	
	shadow1[0] = 0.4;    
	shadow1[1] = 0.1;
	shadow1[2] = 0.1;
	
	state = 0;
}

void loop(){
    api.getMyZRState(me);
	goToTarget(shadow1);
}

void goToTarget(float target[]){
    api.getMyZRState(me);
    for(int i = 0; i < 3; i++){
        myPos[i] = me[i];
    }
    switch (state){
        case 0: //initializing
	
    	    for(int i = 0; i < 3; i++){
                tempTarget[i] = target[i];
            }
    	    mathVecSubtract(V1, target, myPos, 3);
    	    mathVecSubtract(V2, myPos, origin, 3);
    	    lambda = mathVecInner(V2,V1,3) / mathVecInner(V1,V1,3);
    	    for(int i = 0; i < 3; i++){
    	        V1p[i] = V1[i] * lambda;
    	    }
    	    mathVecSubtract(V3, V2, V1p, 3); 
    	    if(mathVecMagnitude(V3,3) < 0.31){
    	        DEBUG(("setting waypoint"));
    	        for(int i = 0; i < 3; i++){
        	        if(V3[i] >= 0){
        	            waypoint[i] = V3[i] * (0.31 / mathVecMagnitude(V3,3)) + 0.05;
        	        }
        	        else{
        	            waypoint[i] = V3[i] * (0.31 / mathVecMagnitude(V3,3)) - 0.05;
        	        }
    	        }                  //waypoint=V3 scaled to outside of danger zone
    	        mathVecSubtract(originalVecBetween, waypoint, myPos, 3);
    	        state = 1;
    	    }
    	    else{
    	        state = 2;
    	    }
    	    
    	    break;
	   
	
	    case 1:
	    
	        mathVecSubtract(vecBetween, waypoint, myPos, 3);
	        distanceBetweenWS = mathVecMagnitude(vecBetween, 3);
	        if(distanceBetweenWS > mathVecMagnitude(originalVecBetween,3) / 3.1){ 
	                                   //when to stop using velocity target is arbitrary
	            api.setVelocityTarget(originalVecBetween); 
	                                   //magnitude stays the same as beginning magnitude
	            DEBUG(("Going to waypoint"));
	            DEBUG(("%f",mathVecMagnitude(originalVecBetween,3)));
	        }
	        else{
	            DEBUG(("going to target"));
	            api.setPositionTarget(target);
	            mathVecSubtract(originalVecToTarget, target, myPos, 3);
	            mathVecNormalize(originalVecToTarget, 3);
	            for(int i = 0; i < 3; i++){
                    originalVecToTarget[i] *= 0.05;
                }
	            state = 2;
	        }
	        break;
	    
	    case 2:
	        if(distance(tempTarget, target) > 0.45){
	            api.setPositionTarget(target);
	            DEBUG(("Stop"));
	        }
	        else{
	            for(int i = 0; i < 3; i++){
                    tempTarget[i] += originalVecToTarget[i];
                }
	            api.setPositionTarget(tempTarget);
    	        DEBUG(("Going to ahead of target"));
	        }
	        break;
	}
}
