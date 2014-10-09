ZRState me;
float origin[3], originalVecBetween[3], vecBetween[3], vecTempToTarget[3], shadow1[3];
float V1[3], V1p[3], V2[3], V3[3], waypoint[3], originalVecToTarget[3],vecToTarget[3];
bool afterWaypoint;
int state;
float originalDistance, distanceBetweenWS, distanceBetweenTS, lambda, angle, angComp;

void init(){
	origin[0] = 0.0;
	origin[1] = 0.0;
	origin[2] = 0.0;
	shadow1[0] = 0.32;
	shadow1[1] = -0.19;
	shadow1[2] = 0.0;
	afterWaypoint = false;
	state = 0;
}

void loop(){
	goToTarget(shadow1);
}

void goToTarget(float target[]){
    api.getMyZRState(me);
    switch (state){
        case 0: //initializing
            mathVecSubtract(V1, target, me, 3);
    	    mathVecSubtract(V2, me, origin, 3);
    	    lambda = mathVecInner(V2,V1,3) / mathVecInner(V1,V1,3);
    	    for(int i = 0; i < 3; i++){
    	        V1p[i] = V1[i] * lambda;
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
    	        }                  //waypoint=V3 scaled to outside of danger zone
    	    mathVecSubtract(originalVecBetween, waypoint, me, 3);
    	    }
    	    else{ //this case is triggered when the point you are going to has a path that doesn't intersect with the asteroid
    	        state = 2; //add stuff here
    	    }
            mathVecSubtract(vecTempToTarget, waypoint, target, 3);
            angComp = PI - acosf(mathVecInner(vecTempToTarget, originalVecBetween, 3) 
    	                      / mathVecMagnitude(originalVecBetween ,3));
    	    DEBUG(("%f", angComp*180/PI));
    	    state = 1;
	        break;
	        
	        
	   case 1:
    	   mathVecSubtract(vecBetween, target, me, 3);
           mathVecNormalize(vecBetween, 3);
    	   angle = acosf(mathVecInner(vecBetween, originalVecBetween, 3) 
    	                      / mathVecMagnitude(originalVecBetween ,3));
    	   DEBUG(("%f", angle*180/PI));
    	        if(angle < angComp * 0.3){
    	            api.setVelocityTarget(originalVecBetween); 
    	                                   //magnitude stays the same as beginning magnitude
    	            DEBUG(("Going to waypoint"));
    	        }
    	        else{
    	            state = 2;
    	        }
    	    break;
    	    
       case 2:
            api.getMyZRState(me);
    	    api.setPositionTarget(target);
    	    DEBUG(("whatismoving"));
            break;
    }
}
