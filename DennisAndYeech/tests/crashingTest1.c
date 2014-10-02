// Test for danger zone radius

ZRState me;
float dangerZoneOuterRadius[3] = {0.33, 0.0, 0.0}; // actual rad = 0.31
float insideDangerZone[3] = {0.25 , 0.0 , 0.0}
int state = 0;

void loop() {

	api.getMyZRState(me);

	DEBUG(("X-coor = %f, Fuel = %f", me[0], game.getFuelRemaining()));
	
	switch (state) {

		case 0: // moving towards the boarder
			
			api.setPositionTarget(dangerZoneOuterRadius);
	}

}
