ZRState me;
float target[3], origin[3], originalVecBetween[3], vecBetween[3], myPos[3], shadow1[3];
float V1[3], V1p[3], V2[3], V3[3], waypoint[3], originalVecToTarget[3],vecToTarget[3];
int state;
bool afterWaypoint;
float originalDistance, distanceBetweenWS, distanceBetweenTS, lambda;

void init(){
	origin[0] = 0.0;
	origin[1] = 0.0;
	origin[2] = 0.0;
	
	shadow1[0] = 0.32;    
	shadow1[1] = -0.19;
	shadow1[2] = 0.15;

	afterWaypoint = false;
	
	state = 0;
}

void loop(){
	goToTarget(shadow1);
}

void goToTarget(float target[]){
    api.getMyZRState(me);
    for(int i = 0; i < 3; i++){
        myPos[i] = me[i];
    }
    switch (state){
        case 0: //initializing
	
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
    	    }
    	    else{
    	        state = 2;
    	    }
    	    state = 1;
    	    break;
	   
	
	    case 1:
	    
	        mathVecSubtract(vecBetween, waypoint, myPos, 3);
	        distanceBetweenWS = mathVecMagnitude(vecBetween, 3);
	        if(distanceBetweenWS > mathVecMagnitude(originalVecBetween,3) / 3.3){ 
	                                   //when to stop using velocity target is arbitrary
	            api.setVelocityTarget(originalVecBetween); 
	                                   //magnitude stays the same as beginning magnitude
	            DEBUG(("Going to waypoint"));
	        }
	        else{
	            DEBUG(("going to target"));
	            api.setPositionTarget(target);
	            mathVecSubtract(originalVecToTarget, target, myPos, 3);
	            state = 2;
	        }
	        break;
	    
	    case 2:
	        mathVecSubtract(vecToTarget, target, myPos, 3);
	        distanceBetweenTS = mathVecMagnitude(vecToTarget, 3);
	        
	        if(!afterWaypoint){
    	        if(distanceBetweenTS > mathVecMagnitude(originalVecToTarget,3) / 1.1){ 
    	            api.setVelocityTarget(originalVecToTarget); 
    	            DEBUG(("Fast to target"));
    	        }
    	        else{
    	            api.setPositionTarget(target);
    	            DEBUG(("Slowing down to target"));
    	            afterWaypoint = true;
    	        }
	        }
	        else{
	            api.setPositionTarget(target);
    	        DEBUG(("Slowing down to target"));
	        }
	        break;
	}
}
