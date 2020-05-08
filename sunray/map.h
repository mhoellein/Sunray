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
enum WayType {WAY_PERIMETER, WAY_EXCLUSION, WAY_MOW, WAY_DOCK, WAY_FREE_PT};
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
    // all lines to mow are stored as a sequence of waypoints
    int targetPointIdx; // next waypoint in waypoint list
    
    int perimeterPointsCount;    
    int exclusionPointsCount;        
    int mowPointsCount;
    int dockPointsCount;
        
    short perimeterStartIdx; // perimeter start index into points    
    short exclusionStartIdx[MAX_EXCLUSIONS]; // exclusion start index into points    
    short mowStartIdx; // mowing start index into points    
    short dockStartIdx;  // docking start index into points
    pt_t points[MAX_POINTS]; // points list (perimeter, exclusions, mowing, docking)
    
    void begin();    
    void run();    
    // set waypoint coordinate
    bool setWaypoint(int idx, int count, float x, float y);    
    // choose target point (0..100%) from waypoint list
    void setTargetWaypointPercent(float perc);
    // set last target point
    void setLastTargetPoint(float stateX, float stateY);
    // distance to target waypoint
    float distanceToTargetPoint(float stateX, float stateY);    
    // go to next waypoint
    bool nextWaypoint();
    // next waypoint available?
    bool nextWaypointAvailable();
  private:
    
};



#endif
