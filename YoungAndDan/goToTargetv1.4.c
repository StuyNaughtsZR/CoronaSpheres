ZRState me;
float target[3], origin[3], originalVecBetween[3], vecBetween[3], shadow1[3];
float V1[3], V1p[3], V2[3], V3[3], waypoint[3], originalVecToTarget[3];
int state;
float originalDistance, distanceBetweenTS;
float tempTarget[3];


void init(){
	origin[0] = 0.0;
	origin[1] = 0.0;
	origin[2] = 0.0;
	
	shadow1[0] = 0.35;    
	shadow1[1] = -.1;
	shadow1[2] = 0.0;
	
	state = 0;
}

void loop(){
	goToTarget(shadow1);
}

void goToTarget(float target[]){
    api.getMyZRState(me);
    switch (state){
        case 0: //initializing
	
    	    if(pathToTarget(target,waypoint)){
    	        mathVecSubtract(originalVecBetween, waypoint, me, 3);
    	        state = 1;
    	    }
    	    else{
    	        state = 2;
    	    }
    	    
    	    break;
	   
	
	    case 1:
	        mathVecSubtract(vecBetween, waypoint, me, 3);
	        if(mathVecMagnitude(vecBetween, 3)/mathVecMagnitude(originalVecBetween,3) > .48){ 
	                                   //when to stop using velocity target is arbitrary
	            api.setVelocityTarget(originalVecBetween); 
	                                   //magnitude stays the same as beginning magnitude
	            DEBUG(("Going to waypoint"));
	            DEBUG(("%f",mathVecMagnitude(originalVecBetween,3)));
	        }
	        else{
	            DEBUG(("going to target"));
	            api.setPositionTarget(origin);
	            state = 2;
	        }
	        break;
	    
	    case 2:
	        if(distance(me,target)>0.08){
	            haulAssTowardTarget(target,2.25);
	            DEBUG(("Going to Target haulAss"));
	        }
	        else{
	            api.setPositionTarget(target);
	        }
	        break;
	}
}
bool pathToTarget(float target[],float waypoint[]){ //returns true if there is something in the way
    float V1[3], V2[3], V1p[3];
    api.getMyZRState(me);
    mathVecSubtract(V1, target, me, 3);
    mathVecSubtract(V2, me, origin, 3);
    for(int i = 0; i < 3; i++){
        V1p[i] = V1[i] * mathVecInner(V2,V1,3) / mathVecInner(V1,V1,3);  
    }
    mathVecSubtract(V3, V2, V1p, 3);
    if(mathVecMagnitude(V3,3) < 0.31){
        DEBUG(("setting waypoint"));
        for(int i = 0; i < 3; i++){
            if(V3[i] >= 0){
                waypoint[i] = V3[i] * (0.31 / mathVecMagnitude(V3,3)) + 0.15;
                }
                else{
                    waypoint[i] = V3[i] * (0.31 / mathVecMagnitude(V3,3)) - 0.15;
                    }
            
            }  
            return true;
    }
    else{
        return false;
    }
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
