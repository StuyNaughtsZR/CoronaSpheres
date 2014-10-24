ZRState me;
float target[3];
float pic1[3], pic2[3];
int POI, counter;
float origin[3];
float vectorBetween[3];
float temp[12];
float rotation[3];
float start[3];
int state;
int total_pics;
bool ready;

void init(){
	origin[0]=0.00;
	origin[1]=0.00;
	origin[2]=0.00;
	
	rotation[0] = 0;
	rotation[1] = 0;
	rotation[2] = -0.6;
	
	start[0] = 0;
	start[1] = 0.6;
	start[2] = 0;
	
	state = 0;
	total_pics = 0;
	counter = 0;
	//DEBUG(("init"));
}

void loop(){
    DEBUG(("%f,%f",pic1[0],pic1[1]));
    if(api.getTime() % 60 == 0){
        DEBUG(("reset\n"));
        state = 0;
    }
    switch (state) {
        
    	case 0:
    	    //DEBUG(("state0"));
    	    api.getMyZRState(me);
        	game.getPOILoc(pic1, 0);
        	game.getPOILoc(pic2, 1);
        	if(distance(me,pic1) < distance(me,pic2)){
        	    target[0]= pic1[0];
        	    target[1]= pic1[1];
        	    target[2]= pic1[2];
        	    POI = 0;
        	}
        	else{
        	    target[0]= pic2[0];
        	    target[1]= pic2[1];
        	    target[2]= pic2[2];
        	    POI = 1;
        	}
        	state = 1;
        	break;
    	
    	case 1:
    	    //DEBUG(("state1\n"));
    	    api.getMyZRState(me);
    	    if(distance(me,target)>0.3){
    	        mathVecSubtract(vectorBetween,target,me,3);
    	        mathVecNormalize(vectorBetween,3);
    	        api.setPositionTarget(target);
    	        api.setAttitudeTarget(vectorBetween);
    	    }
    	    else{
    	        for (int i = 0; i < 12; i++){
                    temp[i] = me[i];
                }
    	        state = 2;
    	    }
    	    break;
    	
    	case 2:
    	    api.getMyZRState(me);
    	    mathVecSubtract(vectorBetween,target,me,3);
    	    mathVecNormalize(vectorBetween,3);
    	    api.setPositionTarget(target);
    	    api.setAttitudeTarget(vectorBetween);
    	    if(counter<3){
    	        counter++;
    	    }
    	    else{
    	        state = 3;
    	    }
    	    break;
    	    
    	case 3:
    	   api.setPositionTarget(temp);
    	   mathVecSubtract(vectorBetween,target,me,3);
    	   mathVecNormalize(vectorBetween,3);
    	   api.setAttitudeTarget(vectorBetween);
    	   //DEBUG(("pics\n"));
    	   game.takePic(POI);
    	   total_pics = game.getMemoryFilled();
    	   if(total_pics>0){
                DEBUG(("a picture was taken!\n"));
                DEBUG(("%f",total_pics));
                state = 5;
                counter = 0;
    	   }
    	   break;
    	   
        case 4:
           api.getMyZRState(me);
           if(distance(me,origin)<0.6){
               mathVecSubtract(vectorBetween,origin,me,3);
        	   mathVecNormalize(vectorBetween,3);
        	   for (int i = 0; i < 3; i++){
                    temp[i] = me[i] + vectorBetween[i];
                }
        	   api.setPositionTarget(temp);
           }
           else{
               //game.uploadPicture();
               state = 0;
           }
           break;
        case 5:
           if(counter<6){
                counter++;
                api.setPositionTarget(start);
           }
           else{
               game.uploadPic();
           }
           break;
    }
}

float distance(float p1[], float p2[]){
    float d=0;
    for(int i = 0; i<3; i++){
        d += (p2[i]-p1[i])*(p2[i]-p1[i]);
    }
    return sqrtf(d);
}
