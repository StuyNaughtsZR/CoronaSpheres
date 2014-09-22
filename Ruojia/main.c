/*I would like to effect a few changes to this code when I have more time.
Currently there is no drift correction. The easiest solution is probably to combine states 0 and 1. In addition, I would like to make outerAlign and innerAlign one function that takes the radius as a parameter.

I am getting the compilation error "‘takePic’ was not declared in this scope".
*/

float poi0[3];
float poi1[3];
float poi[3];
int poiID;
float inner[3];
float outer[3];
int state;
int mem;
float mySphere[12];

void init(){
    mem = 0;
	state = 0;
}

void loop(){
	api.getMyZRState(mySphere);
    //selects POI
    game.getPOILoc(poi0,0);
    game.getPOILoc(poi1,1);
    if ((distance(mySphere,poi0)) <= (distance(mySphere,poi1))){
        for (int i = 0; i < 3; i++){ 
            poiID = 0;
            poi[i] = poi0[i];
        }
    }else{
        for (int i = 0; i < 3; i++){
            poiID = 1;
            poi[i] = poi1[i];
        }
    }
    if (state == 0){
        //innerAlign();
        outerAlign();
    }
    if (state == 1){
        takePic();
    }
}

//moves the sphere to a location aligned with the POI in the inner zone
void innerAlign(){
    for (int i = 0; i < 3; i++){ 
        inner[i] = 1.5*poi[i];
    }
    api.setPositionTarget(inner);
    if ((distance(mySphere,inner)) < 0.05){
        state = 1;
    }
}

//moves the sphere to a location aligned with the POI in the outer zone
void outerAlign(){
    for (int i =0; i < 3; i++){
        outer[i] = 2*poi[i];
    }
    api.setPositionTarget(outer);
    if ((distance(mySphere,outer)) < 0.05){
        state = 1;
    }
}

//sets attitude, checks if aligned, and takes picture
void pic(){
    api.setAttitudeTarget(poi);
    if (game.alignLine() == true){
        DEBUG(("aligned!"));
        game.takePic(poiID);
        if ((game.getMemoryFilled()) > mem){
            DEBUG(("picture taken!"));
            mem = (game.getMemoryFilled());
            state = 2;
        }else{
            DEBUG(("picture not taken"));
        }
    DEBUG(("not aligned"));
    }
}
	
float distance(float* p1, float* p2){
	float d = 0;
	for(int i = 0; i < 3; i ++){
		d += (p2[i]-p1[i])*(p2[i]-p1[i]);
	}
	return sqrtf(d);
}
