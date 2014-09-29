ZRState me;
float pic1[3], pic2[3], target[3];
int POI, counter;
float origin[3];
float vectorBetween[3];
float temp[12];
float rotation[3];
float start[3];
float out[3];
int state;
int total_pics;

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
}

void loop(){
    if(api.getTime() % 60 == 0){
        //TY DENNIS-YEECH FOR BEAUTIFUL CODE :D 
        //resets when POIs reset.
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
    	    api.getMyZRState(me);
    	    if(distance(me,target)>0.23){
    	        mathVecSubtract(vectorBetween,target,me,3);
    	        mathVecNormalize(vectorBetween,3);
    	        api.setPositionTarget(target);
    	        api.setAttitudeTarget(vectorBetween);
    	    }
    	    else{
    	        for (int i = 0; i < 12; i++){
                    temp[i] = me[i];
                }
                DEBUG(("Temp"));
    	        state = 2;
    	    }
    	    break;
    	    
    	    
    	case 2:
    	   api.setPositionTarget(temp);
    	   mathVecSubtract(vectorBetween,target,me,3);
    	   mathVecNormalize(vectorBetween,3);
    	   api.setAttitudeTarget(vectorBetween);
    	   game.takePic(POI);
    	   total_pics = game.getMemoryFilled();
    	   if(total_pics>0){
                DEBUG(("a picture was taken!\n"));
                state = 3;
                counter = 0;
                nearest_out(out, me[0], me[1],.58);
                DEBUG(("%f,%f,%f",out[0],out[1],out[2]));
    	   }
    	   break;
    	   
        case 3:
           api.getMyZRState(me);
           if(distance(me,out)> 0.05){
        	   api.setPositionTarget(out);
           }
           else{
               game.uploadPic();
               DEBUG(("AttemptUpload\n"));
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

void nearest_out(float cors[],float x, float y, float dist){
    if(x>0){
        cors[0]=dist/(sqrtf(1+(y/x)*(y/x)));
        cors[1]=(y*cors[0])/x;
    }
    else{
        cors[0]=-1*dist/(sqrtf(1+(y/x)*(y/x)));
        cors[1]=(y*cors[0])/x;
    }
    cors[2] = 0;
}
