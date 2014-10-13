// Dennis Swagtunin's Code

// What is DEBUG?
// It's like de insect, but with de mouthparts modified for piercing and sucking.

ZRState me;
int state, POIID, picNum, solarFlareBegin;
float POI[3],otherPOI[3],brakingPos[3],facing[3],uploadPos[3];

void init() {

	solarFlareBegin = 1000; //Just to make the solar storm evasion code neater
	
	// apparently mathVecCross is chill with inputs of any size
	//float i[2]= {1, 0};
	//float j[5]= {0, 1, 0, 0, 0};
	//float k[3];
	//mathVecCross(k, i, j);
	//DEBUG(("YO! k = <%f,%f,%f>\n", k[0], k[1], k[2]));
	
	// demonstration of mathVecRotate
	// creates the matrix for a rotation of pi/2 in the xy-plane
	//float blah[3][3], k[3];
	//k[0] = 0.; k[1] = 0.; k[2] = 1.;
	//mathVecRotate(blah, k, 1.57079633);
    //DEBUG(("{{%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f}}", blah[0][0], blah[0][1], blah[0][2], blah[1][0], blah[1][1], blah[1][2], blah[2][0], blah[2][1], blah[2][2]));
    
    // demonstration of calcNewTarget
    //float newTarget[3], target[3] = {-.2, -.2, .2};
    //for (int i = 0; i < 12; i++) (i == 1) ? me[i] = 0.6 : me[i] = 0.;
    //api.getMyZRState(me);
    //DEBUG(("me = {%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f}\n", me[0], me[1], me[2], me[3], me[4], me[5], me[6], me[7], me[8], me[9], me[10], me[11]));
    //DEBUG(("minDistanceFromAsteroid = %f\n", minDistanceFromAsteroid(target)));
    //calcNewTarget(newTarget, target);
    //DEBUG(("newTarget = {%f, %f, %f}\n", newTarget[0], newTarget[1], newTarget[2]));
    
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
	    solarFlareBegin = 1000; // fixes a glitch that makes game.getNextFlare()
	    //return 30s at some random point in the beginning of the game,
	    //and from then on returns -1 until the next actual flare, so that the 
	    //SPHERE reboots for no reason
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
				brakingPos[i] = POI[i] * 2.0;
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
	            for (int i = 0; i < 3; i++) {
	                uploadPos[i] = me[i] / mathVecMagnitude(me, 3) * 0.6;
	            }
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
				for (int i = 0; i < 3; i++) {
	                uploadPos[i] = me[i] / mathVecMagnitude(me, 3) * 0.6;
	            }
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
	
	
    if (api.getTime() >= (solarFlareBegin - 3)) {
        setPositionTarget(me);
        DEBUG(("Screw the mission; I'm staying here!\n"));
    } // prevents crashing during reboot
    
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

void mathVecProject(float c[], float a[], float b[], int n) {
    // finds the projection of a onto b, puts the result in c
    for (int i = 0; i < n; i++) {
        c[i] = mathVecInner(a, b, 3)/mathVecInner(b, b, 3) * b[i];
    }
}

void mathVecOuter(float c[][3], float a[], float b[]) {
    // the api function is stupid and returns a one-dimensional array, rather than a matrix
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            c[i][j] = a[i] * b[j];
        }
    }
}

void mathMatAdd(float c[][3], float a[][3], float b[][3]) {
    // the api function is stupid and returns a one-dimensional array, rather than a matrix
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            c[i][j] = a[i][j] + b[i][j];
        }
    }
}

void mathMatVecMult(float c[], float a[][3], float b[]) {
    // the api function is stupid and takes in two one-dimensional arrays, rather than a matrix and a vector
    for (int i = 0; i < 3; i++) c[i] = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            c[i] += a[i][j] * b[j];
        }
    }
}

void mathVecRotate(float c[][3], float axis[], float theta) {
    // creates a rotation matrix about axis by theta radians, puts the result in c
    // only for 3D
    float a[3];
    for (int i = 0; i < 3; i++) a[i] = axis[i] / mathVecMagnitude(axis, 3); // unit vector in direction of axis
    float I[3][3] = {{1, 0, 0}, {0, 1, 0}, {1, 0, 0}}; // identity matrix
    float ax[3][3] = {{0, -a[2], a[1]}, {a[2], 0, -a[0]}, {-a[1], a[0], 0}}; // cross product matrix of a
    float A[3][3]; // tensor product of a with itself
    mathVecOuter(A, a, a);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            I[i][j] *= cosf(theta);
            ax[i][j] *= sinf(theta);
            A[i][j] *= 1 - cosf(theta);
        }
    }
    mathMatAdd(c, I, ax);
    mathMatAdd(c, c, A);
}

float minDistanceFromOrigin(float target[]) {
	float proj[3], meToTarget[3], testPoint[3];
    mathVecSubtract(meToTarget, target, me, 3);
    mathVecProject(proj, me, meToTarget, 3);
    mathVecSubtract(testPoint, me, proj, 3);
    return mathVecMagnitude(testPoint,3);
}

void calcNewTarget(float newTarget[], float target[]) {
    float magMe, meToTarget[3], normal[3], magNormal, theta, rotationMatrix[3][3], test1[3], test2[3];
    magMe = mathVecMagnitude(me, 3);
    mathVecSubtract(meToTarget, target, me, 3);
    mathVecCross(normal, me, meToTarget);
    // normal to the plane containing Me, target, and origin
    magNormal = mathVecMagnitude(normal, 3);
    for (int i = 0; i < 3; i++) normal[i] /= magNormal;
    // normalize the normal
    theta = asinf(0.315 / magMe);
    // angle between the position vectors to Me and either of the points of tangency
    mathVecRotate(rotationMatrix, normal, theta);
    DEBUG(("{{%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f}}\n", 
    rotationMatrix[0][0], rotationMatrix[0][1], rotationMatrix[0][2], 
    rotationMatrix[1][0], rotationMatrix[1][1], rotationMatrix[1][2],
    rotationMatrix[2][0], rotationMatrix[2][1], rotationMatrix[2][2]));
    // creates the rotation matrix about the normal by an angle of theta
    mathMatVecMult(test1, rotationMatrix, me);
    // rotates Me so that it becomes the position vector to one of the points of tangency
    mathVecRotate(rotationMatrix, normal, -theta);
    // creates the rotation matrix about the normal by the opposite angle
    mathMatVecMult(test2, rotationMatrix, me);
    // rotates Me so that it becomes the position vector to the other point of tangency
    if (distance(test1, target) < distance(test2, target)) {
        // if test1 is closer to the target
        for (int i = 0; i < 3; i++) newTarget[i] = test1[i] / 0.315 * 0.325;
    }
    else {
        // if test2 is closer to the target
        for (int i = 0; i < 3; i++) newTarget[i] = test2[i] / 0.315 * 0.325;
    }
}

void haulAssTowardTarget(float target[], float scalar) {
	// makes you go in the direction of target, but scalar times faster
	float scaledTarget[3];
	for (int i = 0; i < 3; i++) scaledTarget[i] = me[i] + scalar * (target[i] - me[i]);
	api.setPositionTarget(scaledTarget);
}

void setPositionTarget(float Target[]) {
    float target[3];
    for (int i = 0; i < 3; i++) target[i] = Target[i]; // allows input of ZRStates
    if (distance(me, target) < 0.05) {
        api.setPositionTarget(target);
        DEBUG(("I'm here!\n"));
    } // prevents a divide-by-0 error
    else if (minDistanceFromOrigin(target) > 0.315) {
        api.setPositionTarget(target);
        DEBUG(("target = <%f, %f, %f>\n", target[0], target[1], target[2]));
    } // if it's a straight shot, go for it
    else {
        float newTarget[3];
        calcNewTarget(newTarget, target);
        api.setPositionTarget(newTarget);
        DEBUG(("target = <%f, %f, %f>\n", newTarget[0], newTarget[1], newTarget[2]));
    } 
}
