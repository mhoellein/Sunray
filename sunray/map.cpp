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


void Map::begin(){
  wayMode = WAY_MOW;
  targetPointIdx = 0;
  mowPointsCount = 0;
  for (int i=0; i < MAX_POINTS; i++){
    points[i].x=0;
    points[i].y=0;
  }  
}

bool Map::setWaypoint(int idx, int count, float x, float y){
//bool Map::setWaypoint(WayType type, int idx, int subIdx, int count, float x, float y){
  /*switch (type){
    case WAY_PERIMETER:      
      break;
    case WAY_WAY_EXCLUSION:      
      break;
    case WAY_WAY_MOW:      
      break;
    case WAY_WAY_DOCK:    
      break;
  }*/   
  
  if ((idx >= MAX_POINTS) || (count > MAX_POINTS)) return false;  
  if ((idx != 0) && (idx != mowPointsCount)) return false;
  targetPointIdx = 0;  
  points[idx].x = x;
  points[idx].y = y;
  mowPointsCount = count;  
  return true;
}

// 1.0 = 100%
void Map::setTargetWaypointPercent(float perc){
  targetPointIdx = (int)( ((float)mowPointsCount) * perc);
  if (targetPointIdx >= mowPointsCount) {
    targetPointIdx = mowPointsCount-1;
  }
}

void Map::run(){
  targetPoint = points[targetPointIdx];  
}

float Map::distanceToTargetPoint(float stateX, float stateY){  
  float dX = targetPoint.x - stateX;
  float dY = targetPoint.y - stateY;
  float targetDist = sqrt( sq(dX) + sq(dY) );    
  return targetDist;
}

bool Map::nextWaypointAvailable(){
  return (targetPointIdx < mowPointsCount);
}

bool Map::nextWaypoint(){
  if (targetPointIdx+1 < mowPointsCount){
    // next waypoint
    lastTargetPoint = targetPoint;
    targetPointIdx++;
    return true;
  } else {
    // finish        
    targetPointIdx=0;    
    return false;
  }       
}

void Map::setLastTargetPoint(float stateX, float stateY){
  lastTargetPoint.x = stateX; 
  lastTargetPoint.y = stateY;
}

