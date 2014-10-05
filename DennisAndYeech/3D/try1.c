// The Den Cheng Code
// Not to be confused with the Da Vinci Code

ZRState me;
int state, POIID, picNum, solarFlareBegin;
float POI[3],otherPOI[3],brakingPos[3],facing[3],uploadPos[3];

void init() {

	solarFlareBegin = 1000; //Just to make the solar storm evasion code neater

}

void loop() {
	
	api.getMyZRState(me);

	if(api.getTime() % 60 == 0 && state != 4) state = 0;
	
	DEBUG(("State = %d\n", state));

	picNum = game.getMemoryFilled();

	// Solar Storm Evasion

	if (api.getTime() == (solarFlareBegin - 1)) {
	    DEBUG(("I shall now reboot.\n"));
		game.turnOff();
		game.turnOn();
		state = 0;
	}
	else if (api.getTime() >= (solarFlareBegin) && api.getTime() <= (solarFlareBegin + 2)) {
	    DEBUG(("Ah shit, it's a flare!\n"));
	}
	else if (game.getNextFlare() != -1) {
	    solarFlareBegin = api.getTime() + game.getNextFlare();
	    DEBUG(("Next solar flare will occur at %ds.\n", solarFlareBegin));
	}
	else {
	    DEBUG(("I don't know when the next flare is, so stop asking.\n"));
	    solarFlareBegin = 1000; //Fixes a glitch that makes game.getNextFlare()
	    //return 30s at some random point in the beginning of the game,
	    //and from then on return -1 until the next actual flare, so that the 
	    //SPHERE reboots for no reason.
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
				brakingPos[i] = POI[i] * 2.0; //1.5625; <- let's try the outer zone
			}
			
			state = 1;
			break;

		case 1: // Orbit Function here
			if (velocity(me) < 0.001) state = 2;
			else {
				setPositionTarget(brakingPos);
			    mathVecSubtract(facing,POI,me,3);
			    mathVecNormalize(facing,3);
			    api.setAttitudeTarget(facing);
			}
			break;

		case 2: // Legacy Code to me
			setPositionTarget(brakingPos);
			mathVecSubtract(facing,POI,me,3);
			mathVecNormalize(facing,3);
			api.setAttitudeTarget(facing);
            game.takePic(POIID);

			if (picNum > 0) {
				DEBUG(("%d picture(s) have been taken\n", picNum));
				uploadCalc(uploadPos,me);
				setPositionTarget(uploadPos);
				state = 3;
			}
			break;

		case 3: // Taking pic in outer zone
			for (int i = 0; i < 3; i++) {
				brakingPos[i] = POI[i] * 2.5;
			}
			
			if (picNum > 1) {
				DEBUG(("%d picture(s) have been taken.\n", picNum));
				uploadCalc(uploadPos,me);
				setPositionTarget(uploadPos);
				state = 4;
			}
			
			else {
				api.setPositionTarget(brakingPos);
			    mathVecSubtract(facing,POI,me,3);
			    mathVecNormalize(facing,3);
			    api.setAttitudeTarget(facing);
                game.takePic(POIID);
			}

		case 4: // Upload the picture
			if (velocity(me) < 0.01 && distance(me,uploadPos) < 0.05) {
				game.uploadPic();
				DEBUG(("I just uploaded %d picture(s).\n", picNum));
				DEBUG(("I am in state %d.\n", state)); //Why the f**k does it say it's in State 3???
				state = 0;
			}
			else {

				mathVecSubtract(facing,POI,me,3);
				mathVecNormalize(facing,3);
				api.setAttitudeTarget(facing);

				api.setPositionTarget(uploadPos);
				game.takePic(POIID);
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

void mathVecProject(float c[], float a[], float b[], int n) {
    // finds the projection of a onto b, puts the result in c
    for (int i = 0; i < n; i++) {
        c[i] = mathVecInner(a, b, 3)/mathVecInner(b, b, 3) * b[i];
    }
}

void MathVecOuter(float c[][3], float a[], float b[]) {
    // the api function is stupid and returns a one-dimensional array, rather than a matrix
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            c[i][j] = a[i] * b[j];
        }
    }
}

void MathMatAdd(float c[][3], float a[][3], float b[][3]) {
    // the api function is stupid and returns a one-dimensional array, rather than a matrix
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            c[i][j] = a[i][j] + b[i][j];
        }
    }
}

void MathMatVecMult(float c[], float a[][3], float b[]) {
    // the api function is stupid and takes in two one-dimensional arrays, rather than a matrix and a vector
    for (int i = 0; i < 3; i++) c[i] = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            c[i] += a[i][j] * b[j];
        }
    }
}

void mathVecRotate(float c[][3], float a[], float theta) {
    // creates a rotation matrix about axis a by theta radians, puts the result in c
    // only for 3D
    float I[3][3] = {{1, 0, 0}, {0, 1, 0}, {1, 0, 0}}; // identity matrix
    float ax[3][3] = {{0, -a[2], a[1]}, {a[2], 0, -a[0]}, {-a[1], a[0], 0}}; // cross product matrix of a
    float A[3][3]; // tensor product of a with itself
    MathVecOuter(A, a, a);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            I[i][j] *= cosf(theta);
            ax[i][j] *= sinf(theta);
            A[i][j] *= 1 - cosf(theta);
        }
    }
    MathMatAdd(c, I, ax);
    MathMatAdd(c, c, A);
}

void setPositionTarget(float pos[]) {
    float proj[3], meToPos[3], testPoint[3];
    mathVecSubtract(meToPos, pos, me, 3);
    mathVecProject(proj, me, meToPos, 3);
    mathVecSubtract(testPoint, me, proj, 3);
    if (mathVecMagnitude(testPoint, 3) > 0.31) api.setPositionTarget(pos);
    else {
        float Me[3], normal[3], theta, rotationMatrix[3][3], targetTest1[3], targetTest2[3], newTarget[3];
        for (int i = 0; i < 3; i++) Me[i] = me[i];
        mathVecCross(normal, Me, meToPos);
        // normal to the plane containing Me, pos, and origin
        theta = acosf(0.31 / mathVecMagnitude(Me, 3));
        // angle between the position vectors to Me and either of the points of tangency
        mathVecRotate(rotationMatrix, normal, theta);
        // creates the rotation matrix about the normal by an angle of theta
        mathVecNormalize(Me, 3);
        for (int i = 0; i < 3; i++) Me[i] *= 0.315;
        // makes Me slightly over the size of the danger zone (just in case)
        MathMatVecMult(targetTest1, rotationMatrix, Me);
        // rotates Me so that it becomes the position vector to one of the points of tangency
        mathVecRotate(rotationMatrix, normal, -theta);
        // creates the rotation matrix about the normal by the opposite angle
        MathMatVecMult(targetTest2, rotationMatrix, Me);
        // rotates Me so that it becomes the position vector to the other point of tangency
        if (distance(targetTest1, pos) < distance(targetTest2, pos)) for (int i = 0; i < 3; i++) newTarget[i] = targetTest1[i];
        else for (int i = 0; i < 3; i++) newTarget[i] = targetTest2[i];
        api.setPositionTarget(newTarget);
    }
}
