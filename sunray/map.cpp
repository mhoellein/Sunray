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
  trackReverse = false;
  trackSlow = false;
  useGPSfloatForPosEstimation = true;
  useGPSForDeltaEstimation = true;
  useIMU = true;
  mowPointsIdx = 0;
  freePointsIdx = 0;
  dockPointsIdx = 0;
  mowPointsCount = 0;
  perimeterPointsCount = 0;
  exclusionPointsCount = 0;
  dockPointsCount = 0;
  freePointsCount = 0;
  storeIdx = 0;
  targetPointIdx = 0;
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

// set point
bool Map::setPoint(int idx, float x, float y){  
  if (idx >= MAX_POINTS) return false;  
  if ((idx != 0) && (idx != (storeIdx+1))) return false;  
  mowPointsIdx = 0;  
  dockPointsIdx = 0;
  freePointsIdx = 0;
  points[idx].x = x;
  points[idx].y = y;
  storeIdx = idx;
  return true;
}


// set number points for point type
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
  targetPointIdx = mowStartIdx;
}


// set number exclusion points for exclusion
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


// set desired progress in mowing points list
// 1.0 = 100%
void Map::setMowingPointPercent(float perc){
  mowPointsIdx = (int)( ((float)mowPointsCount) * perc);
  if (mowPointsIdx >= mowPointsCount) {
    mowPointsIdx = mowPointsCount-1;
  }
  targetPointIdx = mowStartIdx + mowPointsIdx;
}

void Map::run(){
  targetPoint = points[targetPointIdx];  
  percentCompleted = (((float)mowPointsIdx) / ((float)mowPointsCount) * 100.0);
}

float Map::distanceToTargetPoint(float stateX, float stateY){  
  float dX = targetPoint.x - stateX;
  float dY = targetPoint.y - stateY;
  float targetDist = sqrt( sq(dX) + sq(dY) );    
  return targetDist;
}

// check if path from last target to target to next target is a curve
bool Map::nextPointIsStraight(){
  if (wayMode != WAY_MOW) return false;
  if (mowPointsIdx+1 >= mowPointsCount) return false;     
  pt_t nextPt = points[mowStartIdx + mowPointsIdx+1];  
  float angleCurr = pointsAngle(lastTargetPoint.x, lastTargetPoint.y, targetPoint.x, targetPoint.y);
  float angleNext = pointsAngle(targetPoint.x, targetPoint.y, nextPt.x, nextPt.y);
  angleNext = scalePIangles(angleNext, angleCurr);                    
  float diffDelta = distancePI(angleCurr, angleNext);                 
  //CONSOLE.println(fabs(diffDelta)/PI*180.0);
  return ((fabs(diffDelta)/PI*180.0) <= 20);
}

void Map::setRobotStatePosToDockingPos(float &x, float &y, float &delta){
  if (dockPointsCount < 2) return;
  pt_t dockFinalPt = points[dockStartIdx + dockPointsCount-1];
  pt_t dockPrevPt = points[dockStartIdx + dockPointsCount-2];
  x = dockFinalPt.x;
  y = dockFinalPt.y;
  delta = pointsAngle(dockPrevPt.x, dockPrevPt.y, dockFinalPt.x, dockFinalPt.y);  
}             

// mower has been docked
void Map::setIsDocked(bool flag){
  if (dockPointsCount < 2) return;
  if (flag){
    wayMode = WAY_DOCK;
    dockPointsIdx = dockPointsCount-2;
    targetPointIdx = dockStartIdx + dockPointsIdx;                     
    trackReverse = true;             
    trackSlow = true;
    useGPSfloatForPosEstimation = false;  
    //useGPSForDeltaEstimation = false;
    useIMU = false;
  } else {
    wayMode = WAY_MOW;
    dockPointsIdx = 0;
    targetPointIdx = mowStartIdx + mowPointsIdx;                     
    trackReverse = false;             
    trackSlow = false;
    useGPSfloatForPosEstimation = true;    
    //useGPSForDeltaEstimation = true;
    useIMU = true;
  }  
}

void Map::startDocking(){
  shouldDock = true;
  shouldMow = false;
  if (dockPointsCount > 0){
    // TODO: find valid path to docking point  
    freePointsCount = 1;
    freePointsIdx = 0;
    points[freeStartIdx] = points[dockStartIdx];
  }    
}

void Map::startMowing(){
  shouldDock = false;
  shouldMow = true;    
  if (mowPointsCount > 0){
    // TODO: find valid path to mowing point
    freePointsCount = 1;
    freePointsIdx = 0;
    points[freeStartIdx] = points[mowStartIdx + mowPointsIdx];
  }  
}

// go to next point
// sim=true: only simulate (do not change data)
bool Map::nextPoint(bool sim){
  if (wayMode == WAY_DOCK){
    return (nextDockPoint(sim));
  } 
  else if (wayMode == WAY_MOW) {
    return (nextMowPoint(sim));
  } 
  else if (wayMode == WAY_FREE) {
    return (nextFreePoint(sim));
  } else return false;
}


// get next mowing point
bool Map::nextMowPoint(bool sim){  
  if (shouldMow){
    if (mowPointsIdx+1 < mowPointsCount){
      // next mowing point
      if (!sim) lastTargetPoint = targetPoint;
      if (!sim) mowPointsIdx++;
      if (!sim) targetPointIdx++;
      return true;
    } else {
      // finished mowing;
      mowPointsIdx = 0;      
      targetPointIdx = mowStartIdx;                
      return false;
    }         
  } else if ((shouldDock) && (dockPointsCount > 0)) {      
      // go docking
      if (!sim) lastTargetPoint = targetPoint;
      if (!sim) targetPointIdx = freeStartIdx; 
      if (!sim) wayMode = WAY_FREE;      
      return true;    
  } else return false;  
}

// get next docking point  
bool Map::nextDockPoint(bool sim){    
  if (shouldDock){
    // should dock  
    if (dockPointsIdx+1 < dockPointsCount){
      if (!sim) lastTargetPoint = targetPoint;
      if (!sim) dockPointsIdx++;              
      if (!sim) trackReverse = false;              
      if (!sim) trackSlow = true;
      if (!sim) useGPSfloatForPosEstimation = false;    
      if (!sim) useIMU = false;    
      if (!sim) targetPointIdx++;      
      return true;
    } else {
      // finished docking
      return false;
    } 
  } else if (shouldMow){
    // should undock
    if (dockPointsIdx > 0){
      if (!sim) lastTargetPoint = targetPoint;
      if (!sim) dockPointsIdx--;              
      if (!sim) trackReverse = true;              
      if (!sim) trackSlow = true;
      if (!sim) targetPointIdx--;      
    } else {
      // finished undocking
      if ((shouldMow) && (mowPointsCount > 0 )){
        if (!sim) lastTargetPoint = targetPoint;
        if (!sim) targetPointIdx = freeStartIdx;
        if (!sim) wayMode = WAY_FREE;      
        if (!sim) trackReverse = false;              
        if (!sim) trackSlow = false;
        if (!sim) useGPSfloatForPosEstimation = true;    
        if (!sim) useIMU = true;    
        return true;
      } else return false;        
    }  
  }
}

// get next free point  
bool Map::nextFreePoint(bool sim){
  // free points
  if (freePointsIdx+1 < freePointsCount){
    if (!sim) lastTargetPoint = targetPoint;
    if (!sim) freePointsIdx++;              
    if (!sim) targetPointIdx++;      
  } else {
    // finished free points
    if ((shouldMow) && (mowPointsCount > 0 )){
      // start mowing
      if (!sim) lastTargetPoint = targetPoint;
      if (!sim) targetPointIdx = mowStartIdx + mowPointsIdx;              
      if (!sim) wayMode = WAY_MOW;
      return false;       // TODO: return true
    } else if ((shouldDock) && (dockPointsCount > 0)){      
      // start docking
      if (!sim) lastTargetPoint = targetPoint;
      if (!sim) dockPointsIdx = 0;
      if (!sim) targetPointIdx = dockStartIdx;                
      if (!sim) wayMode = WAY_DOCK;      
      return true;
    } else return false;
  }  
}

void Map::setLastTargetPoint(float stateX, float stateY){
  lastTargetPoint.x = stateX; 
  lastTargetPoint.y = stateY;
}

