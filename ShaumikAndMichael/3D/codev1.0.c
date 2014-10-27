//Rob_3_MJ

/* Notes
-UPLOADED PICTURES SUCCESSFULLYY!!! :)
-can't get to shadow zone, turns off instead


*/


//globals
ZRState Rob;
float pos[3], att[3];
float target[3], attitude[3], tempLoc[3];
float memPack0[3], memPack1[3]; 
float POILoc0[3], POILoc1[3], POILoc2[3], POILocX[3], usdPOI[3];
float szp[3];
float anglebetween, fuel;
int state, c, t, POIctr, POIid, pstate, ctrl;

//globals for mySetPositionTarget function
float nextPosition[3];
float myPosition[3];

void init(){
    
    memPack0[0] = -0.5;
    memPack0[1] = -0.6;
    memPack0[2] = 0;
    
    memPack1[0] = -0.5;
    memPack1[1] = 0.6;
    memPack1[2] = 0;
    
    for(c=0;c<3;c++) 
        usdPOI[c] = -1;
    
    POIctr = 0;
    POIid = -1;
    
    POILocX[0] = 0;
    POILocX[1] = 0;
    POILocX[2] = 0;
    
    state = 0;
    pstate = -1;
    
    t=-1; //will count seconds


	szp[0] = 0.5;
	szp[1] = 0;
	szp[2] = 0;
	
	ctrl = 0;

}

void loop(){

    api.getMyZRState(Rob);
    t++;
    fuel = game.getFuelRemaining();
    
    for(c=0;c<3;c++) {
        pos[c] = Rob[c];
        myPosition[c] = pos[c]; //not necessary but...
        att[c] = Rob[c + 6];
    }
    
    DEBUG(("state:%i\npics:%i\n", state, game.getMemoryFilled()));
    
    if( game.getNextFlare()==1 ) {
        game.turnOff();
        game.turnOn();
    }
    if( t%60 == 0 ) {
        POIctr = 0;
        for(c=0;c<3;c++) 
            usdPOI[c] = -1;
        if( game.getMemoryFilled() == 0 )
            state = 0;
        else
            state = 7;
    }
    
    switch(state) {
        case 0: //choose POI
            game.getPOILoc(POILoc0, 0);
            game.getPOILoc(POILoc1, 1);
            game.getPOILoc(POILoc2, 2);
            if(POIctr==0) {
                POIid = getMin(distanceFormula(pos, POILoc0), distanceFormula(pos, POILoc1), distanceFormula(pos, POILoc2));
                if(POIid==0) {
                    for(c=0;c<3;c++)
                        POILocX[c] = POILoc0[c];
                }
                else if(POIid==1) {
                    for(c=0;c<3;c++)
                        POILocX[c] = POILoc1[c];
                }
                else {
                    for(c=0;c<3;c++)
                        POILocX[c] = POILoc2[c];
                }
            }
            else if(POIctr==1) {
                for(c=0;c<3;c++) {
                    if(usdPOI[c]==0)
                        POIid = getMin(1000, distanceFormula(pos, POILoc1), distanceFormula(pos, POILoc2));
                    else if(usdPOI[c]==1)
                        POIid = getMin(distanceFormula(pos, POILoc0), 1000, distanceFormula(pos, POILoc2));
                    else if(usdPOI[c]==2)
                        POIid = getMin(distanceFormula(pos, POILoc0), distanceFormula(pos, POILoc1), 1000);
                    else ;
                }
                if(POIid==0) {
                    for(c=0;c<3;c++)
                        POILocX[c] = POILoc0[c];
                }
                else if(POIid==1) {
                    for(c=0;c<3;c++)
                        POILocX[c] = POILoc1[c];
                }
                else {
                    for(c=0;c<3;c++)
                        POILocX[c] = POILoc2[c];
                }
            }
            else if(POIctr==2) {
                POIctr = 0;
                for(c=0;c<3;c++)
                    POIctr += usdPOI[c];
                if(POIctr==0) 
                    POIid = 2;
                else if(POIctr==1)
                    POIid = 1;
                else 
                    POIid = 0;
                POIctr = 2;
                if(POIid==0) {
                    for(c=0;c<3;c++)
                        POILocX[c] = POILoc0[c];
                }
                else if(POIid==1) {
                    for(c=0;c<3;c++)
                        POILocX[c] = POILoc1[c];
                }
                else {
                    for(c=0;c<3;c++)
                        POILocX[c] = POILoc2[c];
                }
            }
            else {
                POIid = 0;
                for(c=0;c<3;c++) {
                    POILocX[c] = POILoc0[0];
                }
            }
            state = 1;
            //break;
        case 1: //go to inner zone 
            DEBUG(("%0.2f_%0.2f_%0.2f", POILocX[0], POILocX[1], POILocX[2]));
            getTargetPoint(POILocX, 0.4);
            mathVecSubtract(attitude, POILocX, pos, 3);
            mathVecNormalize(attitude, 3);
            if( mySetPositionTarget(target, 0.01, 0.05) ) {
                state = 2;
                //DEBUG(("time:%is, fuel:%0.3f,\n", t, fuel));
            }
            api.setAttitudeTarget(attitude);
            break;
        case 2: //align angle with POI
            getTargetPoint(POILocX, 0.4);
            mathVecSubtract(attitude, POILocX, pos, 3);
            mathVecNormalize(attitude, 3);
            api.setPositionTarget(target);
            api.setAttitudeTarget(attitude);
            anglebetween = acosf( mathVecInner(att, attitude, 3)/(mathVecMagnitude(att, 3)*mathVecMagnitude(attitude, 3)) );
            if( anglebetween < 0.8 )
                state=3;
            //DEBUG(("angle:%0.5f\n", anglebetween));
            break;
        case 3: //take pic
            getTargetPoint(POILocX, 0.4);
            mathVecSubtract(attitude, POILocX, pos, 3);
            mathVecNormalize(attitude, 3);
            api.setPositionTarget(target);
            api.setAttitudeTarget(attitude);
            if( game.alignLine(POIid) ) {
                DEBUG(("aligned!!!\n"));
                game.takePic(POIid);
            }
            if( game.getMemoryFilled()==1 )
                state = 4;
            break;
        case 4: //go to outer zone
            getTargetPoint(POILocX, 0.5);
            mathVecSubtract(attitude, POILocX, pos, 3);
            mathVecNormalize(attitude, 3);
            api.setPositionTarget(target);
            api.setAttitudeTarget(attitude);
            if( AreWeThereYet(target, 0.01, 0.05) )
                state = 5;
            break;
        case 5: //align with POI
            getTargetPoint(POILocX, 0.5);
            mathVecSubtract(attitude, POILocX, pos, 3);
            mathVecNormalize(attitude, 3);
            api.setPositionTarget(target);
            api.setAttitudeTarget(attitude);
            anglebetween = acosf( mathVecInner(att, attitude, 3)/(mathVecMagnitude(att, 3)*mathVecMagnitude(attitude, 3)) );
            if( anglebetween < 0.4 )
                state=6;
            break;
        case 6: //take pic
            getTargetPoint(POILocX, 0.5);
            mathVecSubtract(attitude, POILocX, pos, 3);
            mathVecNormalize(attitude, 3);
            api.setPositionTarget(target);
            api.setAttitudeTarget(attitude);
            if( game.alignLine(POIid) ) {
                DEBUG(("aligned!!!\n"));
                game.takePic(POIid);
                state = 7;
            }
            
            break;
        case 7: //go to area to upload
            if( ctrl == 0 ) {
                for(c=0;c<3;c++)
                    tempLoc[c] = pos[c];
                ctrl = 1;
            }
            getTargetPoint(tempLoc, 0.6); //make getupload point function!!
            DEBUG(("%0.2f_%0.2f_%0.2f\n", target[0], target[1], target[2]));
            //mathVecSubtract(attitude, POILocX, pos, 3);
            //mathVecNormalize(attitude, 3);
            api.setPositionTarget(target);
            if( mathVecMagnitude(pos, 3) > 0.53 || game.getMemoryFilled() == 0 ) {
                game.uploadPic();
                usdPOI[POIctr] = POIid;
                POIctr++;
                ctrl = 0;
                state = 0;
            }
            //api.setAttitudeTarget(attitude);
            break;
        
        default: //this should never happen
            DEBUG(("Error!!\n"));
            break;
    }
    
    
}




int getMin(float a, float b, float c) {
    if( a <= b && a <= c )
        return(0);
    else if( b<=a && b<=c )
        return(1);
    else 
        return(2);
}

float degreeToRadians(float degree) {
    return( degree * PI / 180 );
}

float distanceFormula(float arr1[], float arr2[]) {
    
    float x1, y1, z1, x2, y2, z2;
    
    x1 = arr1[0];
    y1 = arr1[1];
    z1 = arr1[2];
    
    x2 = arr2[0];
    y2 = arr2[1];
    z2 = arr2[2];
    
    return( sqrtf( mathSquare(x2 - x1) + mathSquare(y2 - y1) + mathSquare(z2 - z1) ) );
    
}

int isThePathClear(float position[], float target[], float center[], float radius) {
    
    float d1, d2, d3, a, p;
    
    d1 = distanceFormula(position, center);
    d2 = distanceFormula(target, center);
    d3 = distanceFormula(position, target);
    
    a = acosf( (mathSquare(d1) + mathSquare(d3) - mathSquare(d2)) / (2*d1*d3));
    
    p = d1*sinf(a);
    
    return( p > radius );
    
}

/*requires globals
float target[3];
*/



/*
Function *getTargetPoint takes 2 parameters, an array of coordinates of a POI and a distance d,
and returns a pointer to an array of coordinates of a point that is on the same line as the POI
and origin and is d units away from the origin
*/

//----CONVERTED; NOW MODIFIES GLOBAL VAR INSTEAD OF RETURNING POINTER-----

//float *getTargetPoint(float poiloc[], float d) {
void getTargetPoint(float poiloc[], float d) {
    
    int ctr;
    float k;
    
    d -= mathVecMagnitude(poiloc, 3);
    
    k = (mathVecMagnitude(poiloc, 3) + d) / mathVecMagnitude(poiloc, 3);
    
    for(ctr=0; ctr<3; ctr++)
        target[ctr] = poiloc[ctr] * k;
        
    //return( &tar[0] );
    
}

/*requires following globals
float nextPosition[3]; //-leave blank
float myPosition[3]; //-put current coordinates
*/


//If the Sphere is within a distance of maxDistance from the target, 
//and is traveling at no faster than maxSpeed, this routine returns 1, otherwise 0.
int AreWeThereYet(float target[3], float maxDistance, float maxSpeed){
    
    float fmyState[12];
    
    api.getMyZRState(fmyState);
    
    float distance=mathSquare(target[0]-myPosition[0]);
    distance+=mathSquare(target[1]-myPosition[1]);
    distance+=mathSquare(target[2]-myPosition[2]);
    distance=sqrtf(distance);
    
    float speed=mathSquare(fmyState[3]);
    speed+=mathSquare(fmyState[4]);
    speed+=mathSquare(fmyState[5]);
    speed=sqrtf(speed);
    
    if(distance<maxDistance && speed<maxSpeed){
        return 1;}
    else{
        return 0;}
}

//This routine calculates and returns how close we would get to the center 
//of the asteroid (the origin) if we went from our current position to the 
//target in a straight line.
float minDistanceFromAsteroid(float target[3]){
    float distance=mathSquare(myPosition[0]);
    distance+=mathSquare(myPosition[1]);
    distance+=mathSquare(myPosition[2]);
    distance=sqrtf(distance);
    return distance;
}

//gives position of next target wehn trying to go around asteriod
void giveNextPos(float target[3], float theta){
    //header
    float magV3;
    float V3[3];
    float V4[3];
    float V5[3];
    
    //step 1 & 2
    magV3=(mathVecInner(myPosition,target,3))/(mathSquare(mathVecMagnitude(myPosition, 3)));
    V3[0]=myPosition[0]*magV3;
    V3[1]=myPosition[1]*magV3;
    V3[2]=myPosition[2]*magV3;
    
    //step 3
    V4[0]=target[0]-V3[0];
    V4[1]=target[1]-V3[1];
    V4[2]=target[2]-V3[2];
    
    //step 4
    V5[0]=V4[0]*mathVecMagnitude(myPosition, 3)/mathVecMagnitude(V4, 3);
    V5[1]=V4[1]*mathVecMagnitude(myPosition, 3)/mathVecMagnitude(V4, 3);
    V5[2]=V4[2]*mathVecMagnitude(myPosition, 3)/mathVecMagnitude(V4, 3);
    
    //step 5
    nextPosition[0]=(myPosition[0]*cosf(theta))+(V5[0]*sinf(theta));
    nextPosition[1]=(myPosition[1]*cosf(theta))+(V5[1]*sinf(theta));
    nextPosition[2]=(myPosition[2]*cosf(theta))+(V5[2]*sinf(theta));
}

int mySetPositionTarget(float ftarget[3],float max_distance,float max_velocity){
    
    float origin[3];
    origin[0]=0;
    origin[1]=0;
    origin[2]=0;
    
    
    //Default
    if(AreWeThereYet(ftarget,max_distance,max_velocity)){
        return 1;
    }
    else{
        if(isThePathClear(myPosition,ftarget,origin,0.32)){
            api.setPositionTarget(target);
            //DEBUG(("hooooo"));
            }
        else{
            giveNextPos(ftarget, 45);
            api.setPositionTarget(nextPosition);
            //DEBUG(("x:%f,y:%f,z:%f\n",nextPosition[0],nextPosition[1],nextPosition[2]));
        }
        return 0;
    }
}
/*globals required
ZRState Enmy
float epos[3], evel[3], eacc[3]
*
    
int stalkEnmy(float tar[]) {
    
    float epos[3], evel[3], eacc[3];
    int ctr;
    
    for(ctr=0;ctr<12;ctr++) {
        pEnmy[c] = Enmy[c];
    }
    
    api.getOtherZRState(Enmy);
    
    
}
*/
















