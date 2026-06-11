#ifndef _DRONE_SWARM_H_
#define _DRONE_SWARM_H_

#include <Uefi.h>

// ── Drone role priority constants ────────────────────────────
#define PRIORITY_PAYLOAD   90
#define PRIORITY_ESCORT    50
#define PRIORITY_SCOUT     20

// ── Health thresholds ────────────────────────────────────────
#define HEALTH_CRITICAL    25
#define HEALTH_DAMAGED     50
#define HEALTH_GOOD        75

// ── Swarm configuration ──────────────────────────────────────
#define MAX_SWARM_SIZE     5

// ── Mission Waypoint Constants ───────────────────────────────
#define TARGET_LAT_THM     134553
#define TARGET_LON_THM     532956
#define LON_MULTIPLIER     17

// ── SWARM_DRONE Struct ───────────────────────────────────────
typedef struct {
    CHAR16     DroneID[9];
    GNSS_DATA  Position;
    INTN       Health;
    INTN       Fuel;
    INTN       Weapons;
    INTN       Priority;
    INTN       MissionValue;
    INTN       DistanceToTarget;
    INTN       BaseMotorCurrent;
    INTN       CurrentMotorCurrent;
    INTN       BaseDrag;
    INTN       CurrentDrag;
    INTN       LastHeartbeatTick;
    BOOLEAN    DamageDetected;
    BOOLEAN    IsAlive;
    BOOLEAN    SacrificeFlag;
} SWARM_DRONE;

// ── Function Declarations ────────────────────────────────────
INTN  CalculateMissionValue(SWARM_DRONE *Drone);
VOID  DetectDamage(SWARM_DRONE *Drone, INTN MotorCurrent, INTN DragValue);
VOID  UpdateHeartbeat(SWARM_DRONE *Drone, INTN GlobalTick);
VOID  CheckHeartbeats(SWARM_DRONE Swarm[], INTN SwarmSize, INTN GlobalTick);
INTN  FlattenToTHM(INTN Deg, INTN Min, INTN Sec);
INTN  CalculateDistanceToTarget(SWARM_DRONE *Drone, INTN TargetLatTHM, INTN TargetLonTHM, INTN LonMultiplier);
INTN  EvaluateSwarmSacrifice(SWARM_DRONE Swarm[], INTN SwarmSize, INTN TargetLatTHM, INTN TargetLonTHM, INTN LonMultiplier);
VOID  DisplaySwarmStatus(SWARM_DRONE Swarm[], INTN SwarmSize, INTN SacrificialIndex);
VOID  InitializeSwarm(SWARM_DRONE Swarm[], INTN *SwarmSize);
VOID  DroneSwarmMenu(VOID);

#endif