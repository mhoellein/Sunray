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


// there are three types of points used as waypoints:
// mowing points:     fixed and transfered by the phone
// docking points:    fixed and transfered by the phone
// free points:       dynamic, connects the above, computed by the Arduino based on the situation 
//                    (obstacles, docking, etc.)
//
// there are additional static points (not used as waypoints but for computing the free points):
// exclusion points:  fixed and transfered by the phone
// perimeter points:  fixed and transfered by the phone

// explain image:  https://wiki.ardumower.de/index.php?title=Ardumower_Sunray#What_data_is_transferred_to_the_Arduino


class Map
{
  public:    
    // current waypoint mode (dock, mow, free)
    WayType wayMode;
    
    // the line defined by (lastTargetPoint, targetPoint) is the current line to drive
    pt_t targetPoint; // target point
    pt_t lastTargetPoint; // last target point
    int targetPointIdx; // index of target point    
    bool trackReverse; // get to target in reverse?
    bool trackSlow;    // get to target slowly?
    bool useGPSfloatForPosEstimation;    // use GPS float solution for position estimation?
    bool useGPSfloatForDeltaEstimation;  // use GPS float solution for delta estimation?
    bool useIMU; // allow using IMU?
    
    // keeps track of the progress in the different point types
    int mowPointsIdx;    // next mowing point in mowing point list    
    int dockPointsIdx;   // next dock point in docking point list
    int freePointsIdx;   // next free point in free point list
    int percentCompleted; 
    
    // all points are stored in one array and we need to know the counts and start indexes
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
    
    // storing all points
    pt_t points[MAX_POINTS]; // points list in this order: ( perimeter, exclusions, docking, mowing, free )
    short storeIdx;  // index where to store next transferred point
    
    bool shouldDock;  // start docking?
    bool shouldMow;  // start mowing?
    
    void begin();    
    void run();    
    // set point coordinate
    bool setPoint(int idx, float x, float y);    
    // set number points for waytype
    bool setWayCount(WayType type, int count);
    // set number points for exclusion 
    bool setExclusionLength(int idx, int len);
    // choose progress (0..100%) in mowing point list
    void setMowingPointPercent(float perc);
    // set last target point
    void setLastTargetPoint(float stateX, float stateY);
    // distance to target waypoint
    float distanceToTargetPoint(float stateX, float stateY);    
    float distanceToLastTargetPoint(float stateX, float stateY);
    // go to next waypoint
    bool nextPoint(bool sim);
    // next point is straight and not a sharp curve?   
    bool nextPointIsStraight();
    // set robot state position to docking position
    void setRobotStatePosToDockingPos(float &x, float &y, float &delta);
    void setIsDocked(bool flag);
    void startDocking();
    void startMowing();
    void dump();
  private:
    bool nextMowPoint(bool sim);
    bool nextDockPoint(bool sim);
    bool nextFreePoint(bool sim);
    
};



#endif
