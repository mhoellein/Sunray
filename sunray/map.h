// Ardumower Sunray 
// Copyright (c) 2013-2020 by Alexander Grau, Grau GmbH
// Licensed GPLv3 for open source use
// or Grau GmbH Commercial License for commercial use (http://grauonline.de/cms2/?page_id=153)
/*
  mapping
*/

#ifndef MAP_H
#define MAP_H

#include <Arduino.h>

#define MAX_POINTS 5000
#define MAX_EXCLUSIONS 100


// waypoint type
enum WayType {WAY_PERIMETER, WAY_EXCLUSION, WAY_DOCK, WAY_MOW, WAY_FREE};
typedef enum WayType WayType;


struct pt_t {
  float x; 
  float y;  
};

typedef struct pt_t pt_t;


class Map
{
  public:    
    // current way mode
    WayType wayMode;
    // the line defined by (lastTargetPoint, targetPoint) is the current line to mow
    pt_t targetPoint; // target point
    pt_t lastTargetPoint; // last target point
    int mowingPointIdx; // next mowing point in waypoint list
    int percentCompleted; 
    
    int perimeterPointsCount;    
    int exclusionPointsCount;        
    int dockPointsCount;
    int mowPointsCount;    
    int freePointsCount;
        
    short perimeterStartIdx; // perimeter start index into points    
    short exclusionCount;    // number exclusions      
    short exclusionLength[MAX_EXCLUSIONS]; // number points in exclusion
    short exclusionStartIdx[MAX_EXCLUSIONS]; // exclusion start index into points        
    short dockStartIdx;  // docking start index into points
    short mowStartIdx; // mowing start index into points    
    short freeStartIdx;  // free points start index into points
    pt_t points[MAX_POINTS]; // points list in this order: ( perimeter, exclusions, docking, mowing, free )
    short storeIdx; 
    
    void begin();    
    void run();    
    // set waypoint coordinate
    bool setPoint(int idx, float x, float y);    
    // set number points for waytype
    bool setWayCount(WayType type, int count);
    // set number points for exclusion 
    bool setExclusionLength(int idx, int len);
    // choose mowing index point (0..100%) from waypoint list
    void setMowingPointPercent(float perc);
    // set last target point
    void setLastTargetPoint(float stateX, float stateY);
    // distance to target waypoint
    float distanceToTargetPoint(float stateX, float stateY);    
    // go to next waypoint
    bool nextPoint();
    // next waypoint available?
    bool nextPointAvailable();
    // next point is straight and not a sharp curve?
    bool nextPointIsStraight();
    void dump();
  private:
    
};



#endif
