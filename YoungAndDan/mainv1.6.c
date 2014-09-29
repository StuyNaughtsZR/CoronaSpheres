#include <math.h>

ZRState me;
float pic1[3], pic2[3], target[3];
int POI;
float origin[3];
float vectorBetween[3];
float temp[12];
float out[3]; //saves location of the nearest place of upload
float solarFlareBegin;
float angle;
bool check;

int state;
int total_pics;

void init(){
	origin[0]=0.00;
	origin[1]=0.00;
	origin[2]=0.00;

	total_pics = 0;
	
	check = true;
	
	state = 0;
}

void loop(){
    //solarFlareBegin = api.getTime() + game.getNextFlare();
    //if(((api.getTime() - 15) == solarFlareBegin)&&(game.getMemoryFilled() > 0)){
	//	state = 3;
	//}
    if(api.getTime() % 60 == 0){
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
    	    nearest_out(out, target[0], target[1],0.33);
    	    if(!myAlignLine(POI)){
    	        mathVecSubtract(vectorBetween,target,me,3);
    	        mathVecNormalize(vectorBetween,3);
    	        api.setAttitudeTarget(vectorBetween);
    	        angle = rotateAround(me,0.4,angle);
    	    }
            else if(distance(me,out)> 0.02){
                mathVecSubtract(vectorBetween,target,me,3);
    	        mathVecNormalize(vectorBetween,3);
    	        api.setAttitudeTarget(vectorBetween);
        	    api.setPositionTarget(out);
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
    	   if(total_pics==2){
                DEBUG(("full\n"));
                state = 3;
                nearest_out(out, me[0], me[1],.58);
    	   }
    	   else if(total_pics == 1){
    	       if(POI == 0){
    	            target[0]= pic2[0];
        	        target[1]= pic2[1];
        	        target[2]= pic2[2];
    	       }
    	       else{
    	           target[0]= pic1[0];
        	       target[1]= pic1[1];
        	       target[2]= pic1[2];
    	       }
    	       state = 1;
    	       nearest_out(out, target[0], target[1],0.5);
    	   }
    	   break;
    	   
        case 3:
           api.getMyZRState(me);
           if(distance(me,out)> 0.05){
        	   api.setPositionTarget(out);
           }
           else{
               game.uploadPic();
               if(POI==0){
                   target[0]=pic2[0];
                   target[1]=pic2[1];
                   target[2]=pic2[2];
                   POI = 1;
               }
               else{
                   target[0]=pic1[0];
                   target[1]=pic1[1];
                   target[2]=pic1[2];
                   POI = 0;
               }
               state = 1;
           }
           break;
           
        case 4:
            DEBUG(("Case4\n"));
            api.getMyZRState(me);
            angle = rotateAround(me,0.35,angle);
            break;
    }
}
