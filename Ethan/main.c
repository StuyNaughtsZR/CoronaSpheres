#include <math.h>

float position[3];
ZRState myState;
float asteroid[3];
float safeZone[3];
float targetRate[3];
float posAnchor[3];
float negAnchor[3];
int var; //var is a value that defines the robot's state
float distFromAsteroid;
float coor1[3];
float coor2[3];
float coor3[3];
float coor4[3];
float poiLOC[3];
int poiID;

void init(){
	//Sets State Vars
	for (int i = 0; i < 3; i++){
		asteroid[i] = 0.0f;
		poiLOC[i] = 0.0f;
	}
	safeZone[0] = 0.4f;
	safeZone[1] = 0.0f;
	safeZone[2] = 0.0f;
	negAnchor = {-0.6 , 0 , 0};
	posAnchor = {0.6 , 0 , 0};
	var = 0;
	poiID = 0;
	updatePosition();
	distFromAsteroid = getDist(position , asteroid);
}

void moveToSafeZone(){
	//Tells the Robot to move to the safe zone
	if (position[1] < 0) {
		api.setPositionTarget({0.4 , position[1] , position[2]});
	}
	else {
		api.setPositionTarget({-0.4 , position[1] , position[2]});
	}
	if (position[0] > 0.2) {
		api.setPositionTarget(safeZone);
	}
}

float getDist(float start[3] , float end[3]){
	return sqrt(pow(start[0] - end[0] , 2) + pow(start[1] - end[1] , 2) + pow(start[2] - end[2] , 2));
}

void checkForPOI() {
	if (game.alignLine() == True) {
		game.getPOILoc(poiLOC , poiID);
		game.takePic(myState , poiLOC);
	}
}

void orbit(){
	checkForPOI();
	//Radius:
	distFromAsteroid = getDist(position , asteroid);
	api.setAttitudeTarget(asteroid);
	//Build coordinate list (2D Ver.)
	coor1 = {distFromAsteroid , distFromAsteroid , 0};
	coor2 = {-1 * distFromAsteroid , distFromAsteroid , 0};
	coor3 = {-1 * distFromAsteroid , -1 * distFromAsteroid , 0};
	coor4 = {distFromAsteroid , -1 * distFromAsteroid};
	//Build States
	int state;
	if (position[1] > 0) {
		if (position[0] > 0) {
			state = 0;
		}
		else {
			state = 1;
		}
	}
	else {
		if (position[0] < 0) {
			state = 3;
		}
		else {
			state = 4;
		}
	}
	//Value of tolerance is subject to tweaking
	float tolerance = 2.2;
	switch (state) {
		case 0:
			if (getDist(coor1 , position) < distFromAsteroid/tolerance) {
				api.setPositionTarget(coor2);
			}
			else {
				api.setPositionTarget(coor1);
			}
			break;
		case 1:
			if (getDist(coor2 , position) < distFromAsteroid/tolerance) {
				api.setPositionTarget(coor3);
			}
			else {
				api.setPositionTarget(coor2);
			}
			break;
		case 2:
			if (getDist(coor3 , position) < distFromAsteroid/tolerance) {
				api.setPositionTarget(coor4);
			}
			else {
				api.setPositionTarget(coor3);
			}
			break;
		case 3:
			if (getDist(coor4 , position) < distFromAsteroid/tolerance) {
				api.setPositionTarget(coor1);
			}
			else {
				api.setPositionTarget(coor4);
			}
			break;
		default:
			break;
	}
}

void moveToOuter(){
	api.setAttitudeTarget(asteroid);
	if (getDist(position , asteroid) > 0.5) {
		api.setPositionTarget(asteroid);
	}
	if (getDist(position , asteroid) < 0.42) {
		//Robot must move AWAY from asteroid now
		if (position[0] < 0) {
			api.setPositionTarget(negAnchor);
		}
		else {
			api.setPositionTarget(posAnchor);
		}
	}
}

void moveToInner(){
	api.setAttitudeTarget(asteroid);
	if (getDist(position , asteroid) > 0.33) {
		api.setPositionTarget(asteroid);
	}
	if (getDist(position , asteroid) < 0.33) {
		if (position[0] < 0) {
			api.setPositionTarget(negAnchor);
		}
		else {
			api.setPositionTarget(posAnchor);
		}
	}
}

void checkUpload(){
	if (game.getMemoryFilled() == game.getMemorySize()) {
		game.uploadPic();
	}
	if (game.getNextFlare() < 5) {
		game.uploadPic();
		moveToSafeZone();
	}
}

void updatePosition(){
	api.getMyZRState(myState);
	for (int i = 0; i < 3; i++) {
		position[i] = myState[i];
	}
}

void decision(){
	//Check var state based on position and time
	//Use switch statement?????
	if (getDist(position , asteroid) > 0.5) {
		var = 0;
	}
	else {
		var = 1;
		if (getDist(position , asteroid) < 0.255) {
			//CRASH IMMINENT! GET OUT!
			var = 2;
		}
		if (game.getNextFlare() < 15) {
			var = 3;
		}
	}
	//Decides what to do based on the value of 'var'
	switch (var) {
		case 0:
			//State where sphere is not in either of the zones
			moveToOuter();
			break;
		case 1:
			//State where sphere is in the outer or inner ring
			orbit();
			break;
		case 2:
			//Sphere in danger zone:
			moveToOuter();
			break;
		case 3:
			//Solar flare about to occur
			moveToSafeZone();
			break;
		default:
			//What to do if things aren't done
			break;
	}
}

void loop(){
	//This function is called once per second.  Use it to control the satellite.
	//Check if the photos should be uploaded
	checkUpload();
	updatePosition();
	decision();
}


