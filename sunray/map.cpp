// Ardumower Sunray 
// Copyright (c) 2013-2020 by Alexander Grau, Grau GmbH
// Licensed GPLv3 for open source use
// or Grau GmbH Commercial License for commercial use (http://grauonline.de/cms2/?page_id=153)


// TODO:  
// + obstacle avoidance:
//    https://wiki.ardumower.de/index.php?title=Ardumower_Sunray#How_the_obstacle_avoidance_works

#include "map.h"
#include <Arduino.h>
#include "config.h"
#include "helper.h"


void Map::begin(){
  wayMode = WAY_MOW;
  mowingPointIdx = 0;
  mowPointsCount = 0;
  perimeterPointsCount = 0;
  exclusionPointsCount = 0;
  dockPointsCount = 0;
  freePointsCount = 0;
  storeIdx = 0;
  for (int i=0; i < MAX_POINTS; i++){
    points[i].x=0;
    points[i].y=0;
  }  
}

void Map::dump(){
  CONSOLE.println("map dump");
  CONSOLE.print("perimeter: ");
  CONSOLE.println(perimeterPointsCount);
  CONSOLE.print("exclusions: ");
  CONSOLE.println(exclusionPointsCount);  
  CONSOLE.print("dock: ");
  CONSOLE.println(dockPointsCount);
  CONSOLE.print("mow: ");  
  CONSOLE.println(mowPointsCount);
  CONSOLE.print("first mow point:");
  CONSOLE.print(points[mowStartIdx].x);
  CONSOLE.print(",");
  CONSOLE.println(points[mowStartIdx].y);
  CONSOLE.print("free: ");
  CONSOLE.println(freePointsCount);
}

bool Map::setPoint(int idx, float x, float y){  
  if (idx >= MAX_POINTS) return false;  
  if ((idx != 0) && (idx != (storeIdx+1))) return false;
  mowingPointIdx = 0;  
  points[idx].x = x;
  points[idx].y = y;
  storeIdx = idx;
  return true;
}


bool Map::setWayCount(WayType type, int count){
  switch (type){
    case WAY_PERIMETER:      
      perimeterPointsCount = count;      
      break;
    case WAY_EXCLUSION:      
      exclusionPointsCount = count;      
      break;
    case WAY_DOCK:    
      dockPointsCount = count;      
    case WAY_MOW:      
      mowPointsCount = count;            
      break;    
    case WAY_FREE:
      freePointsCount = count;
      break;
    default: 
      return false;       
  }
  dockStartIdx = perimeterPointsCount + exclusionPointsCount;
  mowStartIdx = dockStartIdx + dockPointsCount;
  freeStartIdx = mowStartIdx + mowPointsCount;
}


bool Map::setExclusionLength(int idx, int len){
  if (idx >= MAX_EXCLUSIONS) return false;
  int prevStartIdx = perimeterPointsCount;
  if (idx > 0) prevStartIdx = exclusionStartIdx[idx-1];          
  exclusionCount = idx + 1;
  exclusionStartIdx[idx] = prevStartIdx + len;            
  exclusionLength[idx] = len;
  //CONSOLE.print("exclusion ");
  //CONSOLE.print(idx);
  //CONSOLE.print(": ");
  //CONSOLE.println(exclusionLength[idx]);   
  return true;
}


// 1.0 = 100%
void Map::setMowingPointPercent(float perc){
  mowingPointIdx = (int)( ((float)mowPointsCount) * perc);
  if (mowingPointIdx >= mowPointsCount) {
    mowingPointIdx = mowPointsCount-1;
  }
}

void Map::run(){
  targetPoint = points[mowStartIdx + mowingPointIdx];  
  percentCompleted = (((float)mowingPointIdx) / ((float)mowPointsCount) * 100.0);
}

float Map::distanceToTargetPoint(float stateX, float stateY){  
  float dX = targetPoint.x - stateX;
  float dY = targetPoint.y - stateY;
  float targetDist = sqrt( sq(dX) + sq(dY) );    
  return targetDist;
}

bool Map::nextPointIsStraight(){
  if (mowingPointIdx+1 >= mowPointsCount) return false;     
  pt_t nextPt = points[mowStartIdx + mowingPointIdx+1];  
  float angleCurr = pointsAngle(lastTargetPoint.x, lastTargetPoint.y, targetPoint.x, targetPoint.y);
  float angleNext = pointsAngle(targetPoint.x, targetPoint.y, nextPt.x, nextPt.y);
  angleNext = scalePIangles(angleNext, angleCurr);                    
  float diffDelta = distancePI(angleCurr, angleNext);                 
  //CONSOLE.println(fabs(diffDelta)/PI*180.0);
  return ((fabs(diffDelta)/PI*180.0) <= 45);
}

bool Map::nextPointAvailable(){
  return (mowingPointIdx < mowPointsCount);
}

bool Map::nextPoint(){
  if (mowingPointIdx+1 < mowPointsCount){
    // next waypoint
    lastTargetPoint = targetPoint;
    mowingPointIdx++;
    return true;
  } else {
    // finish        
    mowingPointIdx=0;    
    return false;
  }       
}

void Map::setLastTargetPoint(float stateX, float stateY){
  lastTargetPoint.x = stateX; 
  lastTargetPoint.y = stateY;
}

