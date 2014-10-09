#ifndef ZRCORELIB_H_
#define ZRCORELIB_H_

//Universal commands
float dist(float p1[3], float p2[3]);
void dilateValue(float pt1[3], float pt2[3], float dilation, float dst[3]);
void goToLoc(ZRState state, float dest[3], float smoothness);

//CoronaSpheres Specific commands
void goToNoCollision(ZRState start , float end[3] , float currSpeed[3] , float leniency);
#endif
