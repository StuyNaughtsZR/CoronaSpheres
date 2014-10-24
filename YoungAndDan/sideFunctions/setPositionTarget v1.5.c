ZRState me;
float originalVecBetween[3], vecBetween[3], tempTarget[3];
int gTT;


float shadow1[3];


void init(){
	shadow1[0] = -0.23;    
	shadow1[1] = -0.23;
	shadow1[2] = -0.325;
	
	gTT = 0;
}

void loop(){
	setPositionTarget(shadow1);
}

void setPositionTarget(float target[]){
    float waypoint[3];
    api.getMyZRState(me);
    switch (gTT){
        case 0: //initializing
            for(int i = 0; i < 3; i++){
	            tempTarget[i] = target[i];
	        }
    	    if(pathToTarget(me,target,waypoint)){
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
    	        gTT = 1;
    	    }
    	    else{
    	        gTT = 2;
    	    }
    	    
    	    break;
	   
	
	    case 1:
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
	            DEBUG(("%f",mathVecMagnitude(originalVecBetween,3)));
	        }
	        else{
	            DEBUG(("going to target"));
	            api.setPositionTarget(target);
	            gTT = 2;
	        }
	        break;
	    
	    case 2:
	        if(distance(me,target)>0.08){
	            haulAssTowardTarget(target,2);
	            DEBUG(("Going to Target haulAss"));
	        }
	        else{
	            api.setPositionTarget(target);
	        }
	        break;
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
        DEBUG(("setting waypoint"));
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
//pt1 is starting point (anchor)
//pt2 is second point along the line
//dilation will scale out from pt2 away or towards pt1
//Sets dst to be destination/target
    for(int i=0; i < 3; i++){
        dst[i] = dilation * (pt1[i] - pt2[i]) + pt1[i];
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
