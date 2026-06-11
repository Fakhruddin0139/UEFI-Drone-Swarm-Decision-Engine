// ============================================================
//   DRONE SWARM SIMULATOR
//   File: DroneSwarm.c
//   Note: Included directly by UEFICalculator.c
//         Do not compile standalone
// ============================================================

// ── Calculate drone's combat value score ─────────────────────
// Hybrid formula: Health(38) + Weapons(30) + Fuel(22) + Priority(35)
INTN CalculateMissionValue(SWARM_DRONE *Drone)
{
    if (!Drone->IsAlive)
        return 0;

    return (Drone->Health   * 38) +
           (Drone->Weapons  * 30) +
           (Drone->Fuel     * 22) +
           (Drone->Priority * 35);
}

// ── Detect damage from motor current / drag anomaly ──────────
VOID DetectDamage(SWARM_DRONE *Drone, INTN MotorCurrent, INTN DragValue)
{
    if (!Drone->IsAlive)
        return;

    INTN CurrentChange = MotorCurrent - Drone->BaseMotorCurrent;
    INTN DragChange    = DragValue    - Drone->BaseDrag;

    // Major hit
    if (CurrentChange > 25 || DragChange > 30)
    {
        Drone->Health = Clamp(Drone->Health - 35, 0, 100);
        Drone->DamageDetected = TRUE;
    }
    // Minor hit
    else if (CurrentChange > 12 || DragChange > 15)
    {
        Drone->Health = Clamp(Drone->Health - 18, 0, 100);
        Drone->DamageDetected = TRUE;
    }

    // Auto-kill if health reaches zero
    if (Drone->Health <= 0)
    {
        Drone->Health        = 0;
        Drone->IsAlive       = FALSE;
        Drone->SacrificeFlag = FALSE;
    }
}

// ── Update heartbeat for a drone ─────────────────────────────
VOID UpdateHeartbeat(SWARM_DRONE *Drone, INTN GlobalTick)
{
    if (Drone->IsAlive)
        Drone->LastHeartbeatTick = GlobalTick;
}

// ── Check all drones for heartbeat timeout ───────────────────
VOID CheckHeartbeats(SWARM_DRONE Swarm[], INTN SwarmSize, INTN GlobalTick)
{
    for (INTN i = 0; i < SwarmSize; i++)
    {
        if (!Swarm[i].IsAlive)
            continue;

        if (GlobalTick - Swarm[i].LastHeartbeatTick > 3)
        {
            Swarm[i].IsAlive       = FALSE;
            Swarm[i].Health        = 0;
            Swarm[i].SacrificeFlag = FALSE;
        }
    }
}

// ── Flatten GPS coordinates to single integer (THM) ──────────
INTN FlattenToTHM(INTN Deg, INTN Min, INTN Sec)
{
    return (Deg * 60 * 100) + (Min * 100) + Sec;
}

// ── Calculate distance to target waypoint in meters ──────────
INTN CalculateDistanceToTarget(
    SWARM_DRONE *Drone,
    INTN TargetLatTHM,
    INTN TargetLonTHM,
    INTN LonMultiplier)
{
    if (!Drone->IsAlive)
        return 9999999;

    INTN CurrentLatTHM = FlattenToTHM(
        Drone->Position.LatDeg,
        Drone->Position.LatMin,
        Drone->Position.LatSec);

    INTN CurrentLonTHM = FlattenToTHM(
        Drone->Position.LonDeg,
        Drone->Position.LonMin,
        Drone->Position.LonSec);

    INTN DeltaLat = CurrentLatTHM - TargetLatTHM;
    INTN DeltaLon = CurrentLonTHM - TargetLonTHM;

    INTN DeltaY = DeltaLat * 18;
    INTN DeltaX = DeltaLon * LonMultiplier;

    return SquareRoot((DeltaX * DeltaX) + (DeltaY * DeltaY));
}

// ── Main Decision Engine ─────────────────────────────────────
INTN EvaluateSwarmSacrifice(
    SWARM_DRONE Swarm[],
    INTN SwarmSize,
    INTN TargetLatTHM,
    INTN TargetLonTHM,
    INTN LonMultiplier)
{
    INTN LowestValue      = 9999999;
    INTN SacrificialIndex = -1;

    // Step 1: Reset flags, update distance + MissionValue
    for (INTN i = 0; i < SwarmSize; i++)
    {
        Swarm[i].SacrificeFlag = FALSE;

        if (!Swarm[i].IsAlive)
            continue;

        Swarm[i].DistanceToTarget = CalculateDistanceToTarget(
            &Swarm[i], TargetLatTHM, TargetLonTHM, LonMultiplier);

        Swarm[i].MissionValue = CalculateMissionValue(&Swarm[i]);
    }

    // Step 2: Find most expendable drone
    for (INTN i = 0; i < SwarmSize; i++)
    {
        if (!Swarm[i].IsAlive)
            continue;

        INTN CurrentValue = Swarm[i].MissionValue;

        if (CurrentValue < LowestValue)
        {
            LowestValue      = CurrentValue;
            SacrificialIndex = i;
        }
        else if (CurrentValue == LowestValue && SacrificialIndex >= 0)
        {
            if (Swarm[i].DistanceToTarget < Swarm[SacrificialIndex].DistanceToTarget)
                SacrificialIndex = i;
        }
    }

    // Step 3: Issue sacrifice order
    if (SacrificialIndex >= 0)
        Swarm[SacrificialIndex].SacrificeFlag = TRUE;

    return SacrificialIndex;
}

// ── Display complete swarm status table ──────────────────────
VOID DisplaySwarmStatus(SWARM_DRONE Swarm[], INTN SwarmSize, INTN SacrificialIndex)
{
    gST->ConOut->ClearScreen(gST->ConOut);

    Print(L"+=======================================================+\r\n");
    Print(L"|             DRONE SWARM STATUS MONITOR                |\r\n");
    Print(L"+=======================================================+\r\n\n");

    Print(L"+--------+--------+------+-------+----------+----------+\r\n");
    Print(L"| Drone  | Health | Fuel | Weap  | Priority | Status   |\r\n");
    Print(L"+--------+--------+------+-------+----------+----------+\r\n");

    for (INTN i = 0; i < SwarmSize; i++)
    {
        SWARM_DRONE *D = &Swarm[i];
        CHAR16 *StatusText;

        if (!D->IsAlive)
            StatusText = L"DEAD";
        else if (D->SacrificeFlag)
            StatusText = L"** SACRIFICE **";
        else if (D->DamageDetected)
            StatusText = L"DAMAGED";
        else
            StatusText = L"OK";

        Print(L"| %-6s | %3d%%   | %3d%% | %5d | %8d | %-10s |\r\n",
            D->DroneID,
            D->Health,
            D->Fuel,
            D->Weapons,
            D->Priority,
            StatusText);
    }

    Print(L"+--------+--------+------+-------+----------+----------+\r\n\n");

    Print(L"+--------+---------------+------------------+\r\n");
    Print(L"| Drone  | MissionValue  | Distance(m)      |\r\n");
    Print(L"+--------+---------------+------------------+\r\n");

    for (INTN i = 0; i < SwarmSize; i++)
    {
        SWARM_DRONE *D = &Swarm[i];

        if (!D->IsAlive)
            Print(L"| %-6s |     DEAD      |      ---         |\r\n", D->DroneID);
        else
            Print(L"| %-6s | %13d | %16d |\r\n",
                D->DroneID,
                D->MissionValue,
                D->DistanceToTarget);
    }

    Print(L"+--------+---------------+------------------+\r\n\n");

    if (SacrificialIndex < 0)
    {
        Print(L"   NO THREAT DETECTED -- All drones operating normally\r\n");
    }
    else
    {
        SWARM_DRONE *S = &Swarm[SacrificialIndex];
        Print(L"   !!! SACRIFICE ORDER ISSUED !!!\r\n");
        Print(L"   Drone     : %s\r\n",  S->DroneID);
        Print(L"   MissionVal: %d\r\n",  S->MissionValue);
        Print(L"   Distance  : %d meters\r\n", S->DistanceToTarget);
        Print(L"   Action    : Breaking formation to intercept threat\r\n");
    }

    Print(L"\r\nPress any key to continue...\r\n");
}

// ── Initialize swarm with 5 drones ───────────────────────────
VOID InitializeSwarm(SWARM_DRONE Swarm[], INTN *SwarmSize)
{
    *SwarmSize = 5;

    // DRONE-A: Payload Carrier
    StrCpyS(Swarm[0].DroneID, 9, L"DRONE-A");
    Swarm[0].Health = 85; Swarm[0].Fuel = 80; Swarm[0].Weapons = 5;
    Swarm[0].Priority = PRIORITY_PAYLOAD;
    Swarm[0].BaseMotorCurrent = 100; Swarm[0].CurrentMotorCurrent = 100;
    Swarm[0].BaseDrag = 50; Swarm[0].CurrentDrag = 50;
    Swarm[0].LastHeartbeatTick = 0; Swarm[0].DamageDetected = FALSE;
    Swarm[0].IsAlive = TRUE; Swarm[0].SacrificeFlag = FALSE;
    Swarm[0].MissionValue = 0; Swarm[0].DistanceToTarget = 0;
    Swarm[0].Position.LatDeg = 22; Swarm[0].Position.LatMin = 32;
    Swarm[0].Position.LatSec = 60; Swarm[0].Position.LatDir = 'N';
    Swarm[0].Position.LonDeg = 88; Swarm[0].Position.LonMin = 22;
    Swarm[0].Position.LonSec = 70; Swarm[0].Position.LonDir = 'E';
    Swarm[0].Position.Valid  = TRUE;

    // DRONE-B: Escort
    StrCpyS(Swarm[1].DroneID, 9, L"DRONE-B");
    Swarm[1].Health = 80; Swarm[1].Fuel = 75; Swarm[1].Weapons = 4;
    Swarm[1].Priority = PRIORITY_ESCORT;
    Swarm[1].BaseMotorCurrent = 100; Swarm[1].CurrentMotorCurrent = 100;
    Swarm[1].BaseDrag = 50; Swarm[1].CurrentDrag = 50;
    Swarm[1].LastHeartbeatTick = 0; Swarm[1].DamageDetected = FALSE;
    Swarm[1].IsAlive = TRUE; Swarm[1].SacrificeFlag = FALSE;
    Swarm[1].MissionValue = 0; Swarm[1].DistanceToTarget = 0;
    Swarm[1].Position.LatDeg = 22; Swarm[1].Position.LatMin = 32;
    Swarm[1].Position.LatSec = 55; Swarm[1].Position.LatDir = 'N';
    Swarm[1].Position.LonDeg = 88; Swarm[1].Position.LonMin = 22;
    Swarm[1].Position.LonSec = 65; Swarm[1].Position.LonDir = 'E';
    Swarm[1].Position.Valid  = TRUE;

    // DRONE-C: Damaged Escort
    StrCpyS(Swarm[2].DroneID, 9, L"DRONE-C");
    Swarm[2].Health = 35; Swarm[2].Fuel = 40; Swarm[2].Weapons = 2;
    Swarm[2].Priority = PRIORITY_ESCORT;
    Swarm[2].BaseMotorCurrent = 100; Swarm[2].CurrentMotorCurrent = 125;
    Swarm[2].BaseDrag = 50; Swarm[2].CurrentDrag = 68;
    Swarm[2].LastHeartbeatTick = 0; Swarm[2].DamageDetected = TRUE;
    Swarm[2].IsAlive = TRUE; Swarm[2].SacrificeFlag = FALSE;
    Swarm[2].MissionValue = 0; Swarm[2].DistanceToTarget = 0;
    Swarm[2].Position.LatDeg = 22; Swarm[2].Position.LatMin = 32;
    Swarm[2].Position.LatSec = 65; Swarm[2].Position.LatDir = 'N';
    Swarm[2].Position.LonDeg = 88; Swarm[2].Position.LonMin = 22;
    Swarm[2].Position.LonSec = 75; Swarm[2].Position.LonDir = 'E';
    Swarm[2].Position.Valid  = TRUE;

    // DRONE-D: Scout
    StrCpyS(Swarm[3].DroneID, 9, L"DRONE-D");
    Swarm[3].Health = 70; Swarm[3].Fuel = 65; Swarm[3].Weapons = 2;
    Swarm[3].Priority = PRIORITY_SCOUT;
    Swarm[3].BaseMotorCurrent = 100; Swarm[3].CurrentMotorCurrent = 100;
    Swarm[3].BaseDrag = 50; Swarm[3].CurrentDrag = 50;
    Swarm[3].LastHeartbeatTick = 0; Swarm[3].DamageDetected = FALSE;
    Swarm[3].IsAlive = TRUE; Swarm[3].SacrificeFlag = FALSE;
    Swarm[3].MissionValue = 0; Swarm[3].DistanceToTarget = 0;
    Swarm[3].Position.LatDeg = 22; Swarm[3].Position.LatMin = 32;
    Swarm[3].Position.LatSec = 50; Swarm[3].Position.LatDir = 'N';
    Swarm[3].Position.LonDeg = 88; Swarm[3].Position.LonMin = 22;
    Swarm[3].Position.LonSec = 60; Swarm[3].Position.LonDir = 'E';
    Swarm[3].Position.Valid  = TRUE;

    // DRONE-E: Critical Scout
    StrCpyS(Swarm[4].DroneID, 9, L"DRONE-E");
    Swarm[4].Health = 25; Swarm[4].Fuel = 30; Swarm[4].Weapons = 1;
    Swarm[4].Priority = PRIORITY_SCOUT;
    Swarm[4].BaseMotorCurrent = 100; Swarm[4].CurrentMotorCurrent = 100;
    Swarm[4].BaseDrag = 50; Swarm[4].CurrentDrag = 50;
    Swarm[4].LastHeartbeatTick = 0; Swarm[4].DamageDetected = FALSE;
    Swarm[4].IsAlive = TRUE; Swarm[4].SacrificeFlag = FALSE;
    Swarm[4].MissionValue = 0; Swarm[4].DistanceToTarget = 0;
    Swarm[4].Position.LatDeg = 22; Swarm[4].Position.LatMin = 32;
    Swarm[4].Position.LatSec = 70; Swarm[4].Position.LatDir = 'N';
    Swarm[4].Position.LonDeg = 88; Swarm[4].Position.LonMin = 22;
    Swarm[4].Position.LonSec = 80; Swarm[4].Position.LonDir = 'E';
    Swarm[4].Position.Valid  = TRUE;
}

// ── Drone Swarm Simulator — Main Menu ────────────────────────
VOID DroneSwarmMenu()
{
    SWARM_DRONE Swarm[MAX_SWARM_SIZE];
    INTN SwarmSize  = 0;
    INTN GlobalTick = 0;
    BOOLEAN Running = TRUE;

    InitializeSwarm(Swarm, &SwarmSize);

    while (Running)
    {
        gST->ConOut->ClearScreen(gST->ConOut);

        Print(L"+=====================================================+\n");
        Print(L"|           DRONE SWARM SIMULATOR                    |\n");
        Print(L"+=====================================================+\n\n");

        Print(L"  [1] Show Swarm Status\n");
        Print(L"  [2] Simulate Bullet Hit\n");
        Print(L"  [3] Simulate Missile Threat\n");
        Print(L"  [4] Run Full Simulation Cycle\n");
        Print(L"  [5] Reset Swarm\n");
        Print(L"  [6] Back to Main Menu\n\n");

        Print(L"  Select option: ");

        UINTN Choice = GetChoice();
        Print(L"%d\n", Choice);

        switch (Choice)
        {
        case 1:
        {
            (VOID)EvaluateSwarmSacrifice(
                Swarm, SwarmSize,
                TARGET_LAT_THM, TARGET_LON_THM, LON_MULTIPLIER);
            DisplaySwarmStatus(Swarm, SwarmSize, -1);
            WaitForKey();
            break;
        }
        case 2:
        {
            gST->ConOut->ClearScreen(gST->ConOut);
            Print(L"\n  Select drone to hit (1=DRONE-A, 2=B, 3=C, 4=D, 5=E): ");
            UINTN Target = GetChoice();

            if (Target >= 1 && Target <= (UINTN)SwarmSize)
            {
                INTN Idx = (INTN)Target - 1;
                if (Swarm[Idx].IsAlive)
                {
                    Swarm[Idx].CurrentMotorCurrent = 130;
                    Swarm[Idx].CurrentDrag         = 85;
                    DetectDamage(&Swarm[Idx], 130, 85);
                    Print(L"\n  DRONE-%c hit! Health now: %d%%\n",
                        (CHAR16)(L'A' + Idx),
                        Swarm[Idx].Health);
                }
                else
                    Print(L"\n  Drone already destroyed!\n");
            }
            WaitForKey();
            break;
        }
        case 3:
        {
            gST->ConOut->ClearScreen(gST->ConOut);
            Print(L"\n  !! MISSILE THREAT DETECTED !!\n");
            Print(L"  Running sacrifice decision engine...\n");
            gBS->Stall(1500000);

            INTN Result = EvaluateSwarmSacrifice(
                Swarm, SwarmSize,
                TARGET_LAT_THM, TARGET_LON_THM, LON_MULTIPLIER);

            DisplaySwarmStatus(Swarm, SwarmSize, Result);
            WaitForKey();
            break;
        }
        case 4:
        {
            GlobalTick++;
            CheckHeartbeats(Swarm, SwarmSize, GlobalTick);

            for (INTN i = 0; i < SwarmSize; i++)
                UpdateHeartbeat(&Swarm[i], GlobalTick);

            INTN Result = EvaluateSwarmSacrifice(
                Swarm, SwarmSize,
                TARGET_LAT_THM, TARGET_LON_THM, LON_MULTIPLIER);

            DisplaySwarmStatus(Swarm, SwarmSize, Result);
            WaitForKey();
            break;
        }
        case 5:
        {
            InitializeSwarm(Swarm, &SwarmSize);
            GlobalTick = 0;
            Print(L"\n  Swarm reset to initial state.\n");
            WaitForKey();
            break;
        }
        case 6:
            Running = FALSE;
            break;
        default:
            Print(L"\n  Invalid choice!\n");
            WaitForKey();
        }
    }
}