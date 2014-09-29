// The Den Cheng Code
// Not to be confused with the Da Vinci Code

ZRState me;
int state, POIID, picNum, solarFlareBegin;
float POI[3],otherPOI[3],breakingPos[3],facing[3],uploadPos[3];

void init() {

	solarFlareBegin = 1000; //Just to make the solar storm evasion code neater

}

void loop() {
	
	api.getMyZRState(me);

	if(api.getTime() % 60 == 0) state = 0;
	
	DEBUG(("State = %d\n", state));

	picNum = game.getMemoryFilled();

	// Solar Storm Evasion

	if (api.getTime() == solarFlareBegin) {
	    DEBUG(("Until further notice, I am off.\n"));
		game.turnOff();
	}
	else if (api.getTime() == solarFlareBegin + 3) {
	    DEBUG(("I am now on.\n"));
		game.turnOn();
		if (state == 3) state = 0;
	}
	else if (game.getNextFlare() != -1) {
	    solarFlareBegin = api.getTime() + game.getNextFlare();
	    DEBUG(("Next solar flare will occur at %ds.\n", solarFlareBegin));
	}
	else if (api.getTime() == solarFlareBegin + 1 || api.getTime() == solarFlareBegin + 2) {
	    DEBUG(("Ah shit, it's a flare!\n"));
	}
	else {
	    DEBUG(("I don't know when the next flare is, so stop asking.\n"));
	}
	
	switch (state) {

		case 0: // POI Selection
			game.getPOILoc(POI,0);
			game.getPOILoc(otherPOI,1);
			if (distance(me,otherPOI) <= distance(me,POI)) {
				POIID = 1;
				for (int i = 0; i < 3; i++) POI[i] = otherPOI[i];
			}
			else POIID = 0;
			
			DEBUG(("POI Coors = %f,%f,%f\n",POI[0],POI[1],POI[2]));

			for (int i = 0; i < 3; i++) {
				breakingPos[i] = POI[i] * 2.0; //1.5625; <- let's try the outer zone
			}
			
			state = 1;
			
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

			if (picNum > 0) {
				DEBUG(("%d picture(s) have been taken\n", picNum));
				uploadCalc(uploadPos,me);
				api.setPositionTarget(uploadPos);
				state = 3;
			}

			break;

		case 3: // Upload the picture
			if (velocity(me) < 0.01 && distance(me,uploadPos) < 0.05) {
				game.uploadPic();
				DEBUG(("I just uploaded a picture.\n"));
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
		uploadPos[i] = me[i] * 0.6;
	}
}
