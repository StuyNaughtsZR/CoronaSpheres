//Yeech + Dennis = EPIC SWAG

ZRState me;
int state, POIID, picNum;
float POI[3],test0[3],test1[3],breakingPos[3],facing[3],uploadPos[3];

void init() {

	state = 0;

}

void loop() {
	
	DEBUG(("State = %d\n", state));

	api.getMyZRState(me);

	if(api.getTime() % 60 == 0) state = 0; // "CODE SIZE MATTERS" -- Dennis

	switch (state) {

		case 0: // POI Selection
			game.getPOILoc(test0,0);
			game.getPOILoc(test1,1);
			if (distance(me,test0) <= distance(me,test1)) {
				POIID = 0;
				for (int i = 0; i < 3; i++) POI[i] = test0[i];
	        		state = 1;
			}
			else {
				POIID = 1;
	    			for (int i = 0; i < 3; i++) POI[i] = test1[i];
	       			state = 1;
			}
			
			DEBUG(("POI Coors = %f,%f,%f\n",POI[0],POI[1],POI[2]));

			for (int i = 0; i < 3; i++) {
				breakingPos[i] = POI[i] * 1.5625;
			}
			break;

		case 1: // "Borrowed" from Young + some personal refinement
			if (velocity(me) < 0.001) state = 2;
			else {
				mathVecSubtract(facing,POI,me,3);
				mathVecNormalize(facing,3);
				api.setPositionTarget(breakingPos);
				api.setAttitudeTarget(facing);
			}
			break;

		case 2: // Legacy Code to me
			api.setPositionTarget(breakingPos);
			
			mathVecSubtract(facing,POI,me,3);
			mathVecNormalize(facing,3);
			api.setAttitudeTarget(facing);

			game.takePic(POIID);
			game.takePic(POIID); // Old school way of spamming
			game.takePic(POIID);
			game.takePic(POIID);
			game.takePic(POIID); // Old school way of spamming
			game.takePic(POIID);


			picNum = game.getMemoryFilled();

			if (picNum > 0) {
				DEBUG(("%d pictures have been taken"));
				uploadCalc(uploadPos,me);
				state = 3;
			}

			break;

		case 3:
			if (velocity(me) < 0.01) {
				game.uploadPic();
				
				state = 0;
			}
			else {
				api.setPositionTarget(uploadPos);
			}

			break;
			
	}
}

float distance(float p1[], float p2[]){
	float d = 0;
	for(int i=0; i < 3; i++){
		d += (p2[i]-p1[i])*(p2[i]-p1[i]);
	}
	return sqrtf(d);
}

float velocity(float p1[]){
	float d = 0;
	for(int i=3; i < 6; i++){
		d += p1[i]*p1[i];
	}
	return sqrtf(d);
}

void uploadCalc(float uploadPos[], float me[]){
	mathVecNormalize(me,3);
	for (int i = 0; i < 3; i++) {
		uploadPos[i] = me[i] * 0.5;
	}
}
