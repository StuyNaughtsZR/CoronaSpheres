#ifndef ZRCORELIB_H_
#define ZRCORELIB_H_

float dist(float p1[3], float p2[3]);
void dilateValue(float pt1[3], float pt2[3], float dilation, float dst[3]);
void goToLoc(ZRState state, float dest[3], float smoothness);
#endif
