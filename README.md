<div align="center">

#  UEFI Drone Swarm Decision Engine

**Autonomous multi-drone sacrifice logic running at the firmware layer — before any OS loads.**

[![Language](https://img.shields.io/badge/Language-C%20(Pure)-blue?style=flat-square)](https://github.com/Fakhruddin0139/UEFI-Drone-Swarm-Decision-Engine)
[![Framework](https://img.shields.io/badge/Framework-EDK2-orange?style=flat-square)](https://github.com/tianocore/edk2)
[![Platform](https://img.shields.io/badge/Platform-UEFI%20x86__64-lightgrey?style=flat-square)]()
[![License](https://img.shields.io/badge/License-MIT-green?style=flat-square)](LICENSE)
[![Build](https://img.shields.io/badge/Build-VS2022%20%7C%20Windows%2011-purple?style=flat-square)]()
[![Tested On](https://img.shields.io/badge/Tested%20On-QEMU%20%2B%20OVMF-yellow?style=flat-square)]()



</div>

---

##  What Is This?

Most swarm decision systems run on Linux, ROS2, or Python.  
This one runs **before the bootloader**.

The UEFI Drone Swarm Decision Engine is a firmware-layer autonomous swarm simulator built entirely in pure C using the **EDK2 framework**. It operates at UEFI runtime — no OS, no stdlib, no floating-point hardware, no `malloc`.

A formation of **5 drones** moves toward a target waypoint. When a missile threat is detected, the engine evaluates every drone's real-time combat value and mathematically selects the most expendable unit to intercept the threat — **no human command needed**.

> ⚠️ **Educational simulation only.** Runs in QEMU with hardcoded dummy telemetry data. No real hardware, weapons, or classified information involved.

---

##  Why UEFI?

UEFI firmware is the lowest software layer on any x86 machine — it runs before Windows, before Linux, before the bootloader. Writing decision logic here means:

- **No OS abstractions** — every data structure, every algorithm is yours
- **No standard library** — `sqrt()`, `memcpy()`, `abs()` must all be written from scratch
- **No floating-point** — all math done in fixed-point integer arithmetic
- **No dynamic memory** — stack allocation only, no heap
- **Deterministic execution** — no scheduler, no interrupts stealing CPU cycles

For aerospace and defence systems, firmware-layer logic means the decision engine survives even if the OS layer is compromised or hasn't loaded yet. This project demonstrates that a complete swarm AI decision loop is achievable under those extreme constraints.

---

##  Features

| Feature | Description |
|---|---|
| **MissionValue Scoring** | Weighted integer formula ranking each drone's combat worth |
| **Sacrifice Engine** | O(n) algorithm selects optimal sacrifice drone in real time |
| **Tie-Break Logic** | Equal MissionValue → drone closest to threat is sacrificed |
| **Sensor-Based Damage Detection** | Motor current + drag anomaly detection without explicit hit signals |
| **Heartbeat Watchdog** | Tick-delta based liveness monitor (same pattern as ZooKeeper/K8s) |
| **GNSS Telemetry** | Integer-based equirectangular distance computation from raw NMEA coords |
| **Formation Roles** | Payload Carrier, Escort, Scout — each with distinct priority weights |
| **Protected Asset Logic** | Payload carrier shielded unless critically damaged |

---

##  Live Demo — Running in UEFI Shell

The entire engine runs inside QEMU with OVMF firmware. No real hardware needed.

**Swarm Status Monitor**
> Shows live health, fuel, weapons count, and computed MissionValue for all 5 drones.

<img width="972" height="645" alt="swarm_status" src="https://github.com/user-attachments/assets/c5a3f744-1b38-4ca5-8057-013df236b710" />
`

**Bullet Hit Simulation**
> Simulates sensor anomaly from projectile impact — motor current spike + drag increase triggers health reduction.

<img width="708" height="398" alt="bullet_hit" src="https://github.com/user-attachments/assets/84b31d9c-4515-4cb6-9f9a-f03047bf5aa9" />


**Sacrifice Order Issued**
> Missile threat detected. Engine runs full evaluation. DRONE-E selected. Sacrifice flag set.

<img width="846" height="658" alt="sacrifice_order" src="https://github.com/user-attachments/assets/22c5c495-9922-4576-9f96-bac33c7523d4" />


**Swarm Reset**
> Formation restored to initial state. All drones alive. Ready for next cycle.

<img width="770" height="475" alt="reset" src="https://github.com/user-attachments/assets/1d954212-b528-407f-abb2-0576f266ac0f" />


---

##  Five Drone Formation

```
         [TARGET WAYPOINT]
               ↑
          [DRONE-A]  ← Payload Carrier (PROTECTED)
         /          \
   [DRONE-B]      [DRONE-C]   ← Escorts
      /                \
 [DRONE-D]          [DRONE-E]  ← Scouts (SACRIFICED FIRST)
```

| Drone   | Role            | Health | Fuel | Weapons | Priority | MissionValue |
|---------|-----------------|--------|------|---------|----------|--------------|
| DRONE-A | Payload Carrier | 85%    | 80%  | 5       | 90       | **8290** ← Protected |
| DRONE-B | Escort          | 80%    | 75%  | 4       | 50       | 6460 |
| DRONE-C | Damaged Escort  | 35%    | 40%  | 2       | 50       | 3150 |
| DRONE-D | Scout           | 70%    | 65%  | 2       | 20       | 4383 |
| DRONE-E | Critical Scout  | 25%    | 30%  | 1       | 20       | **2340** ← Sacrificed |

DRONE-E always sacrifices first. DRONE-A is always protected. **The math decides. Not the code.**

---

##  Technical Architecture

### MissionValue Scoring Formula

```
MissionValue = (Health × 38) + (Weapons × 30) + (Fuel × 22) + (Priority × 35)
```

**Why these weights?**

| Parameter | Weight | Rationale |
|-----------|--------|-----------|
| Health | ×38 | Heaviest — a damaged drone is the first candidate |
| Priority | ×35 | Second heaviest — protects key assets without immortalizing them |
| Weapons | ×30 | Armed drones retained for mission continuation |
| Fuel | ×22 | Lightest — fuel alone doesn't save a damaged drone |

A critically damaged Payload Carrier (Health = 10%) can still be sacrificed over a healthy Scout. The weights create a spectrum, not hard rules.

---

### Distance Calculation (Equirectangular Approximation)

No `math.h`. No `cos()`. No floats. All GNSS coordinates processed as integers:

```
THM = (Degrees × 6000) + (Minutes × 100) + Seconds
      1 THM ≈ 18.52 meters physical distance

ΔY = ΔLatTHM × 18          ← latitude is constant everywhere
ΔX = ΔLonTHM × 17          ← longitude at ~24°N (Central India)

Distance = √(ΔX² + ΔY²)    ← integer Newton-Raphson sqrt
```

Squaring both deltas handles negatives automatically — no `abs()` needed.  
For swarm operations under 50 km, equirectangular error is under 0.3%. Haversine is overkill and requires trig.

---

### Damage Detection (Sensor Anomaly Method)

Real bullet/shrapnel impact changes two measurable physical quantities — motor current and aerodynamic drag — without requiring a dedicated "hit" sensor:

```
ΔCurrent = CurrentMotorCurrent − BaseMotorCurrent
ΔDrag    = CurrentDrag − BaseDrag

ΔCurrent > 25  OR  ΔDrag > 30  →  Major hit  →  Health −= 35
ΔCurrent > 12  OR  ΔDrag > 15  →  Minor hit  →  Health −= 18
Health ≤ 0                      →  IsAlive = FALSE
```

This approach mirrors real telemetry analysis — the anomaly pattern, not the event, triggers the classification.

---

### Heartbeat Watchdog Timer

```c
// Every cycle: each drone writes its tick
Drone->LastHeartbeatTick = GlobalTick;

// Monitor checks for silence
if (GlobalTick - Drone->LastHeartbeatTick > 3) {
    Drone->IsAlive = FALSE;   // declared dead
}
```

The same pattern powers Apache ZooKeeper session timeouts, Kubernetes pod liveness probes, and military mesh radio dead-node detection. When ported to real hardware — replace `GlobalTick` with a UART packet timestamp. Core logic unchanged.

---

### Sacrifice Selection Algorithm

```
Step 1: Reset all SacrificeFlags
Step 2: Recompute DistanceToTarget for all alive drones
Step 3: Recompute MissionValue for all alive drones
Step 4: Find alive drone with minimum MissionValue
Step 5: Tie-break — equal MissionValue → drone closest to threat is sacrificed
Step 6: Set SacrificeFlag = TRUE on selected drone
```

**Complexity: O(n). Single pass. Real-time capable.**

Dead drones return `MissionValue = 9999999` — guaranteed never selected.

---

##  Bare-Metal Constraints & Solutions

Running before any OS means zero access to standard libraries. Every primitive was built from scratch:

| Unavailable | Replacement | Implementation |
|---|---|---|
| `max()` / `min()` | `GetMax()` / `GetMin()` | Ternary expression |
| `abs()` | `AbsVal()` | Ternary expression |
| `sqrt()` | `IntSqrt()` | Newton-Raphson integer method |
| `memcpy()` / `memset()` | Custom stubs | Byte loop |
| `strcpy()` | `StrCpyS()` | EDK2 safe string API |
| `malloc()` / heap | Stack allocation | Fixed-size struct arrays |
| `float` / `double` | Fixed-point integers | Scale factors × 10, × 100 |
| `printf()` | `Print()` | EDK2 Unicode print |

Additionally: MSVC (used by EDK2 on Windows) requires `INTN` not `UINTN` for signed arithmetic — type mismatches cause silent build failures. All arithmetic uses `INTN` consistently.

---

##  How to Build

### Prerequisites

| Tool | Version | Location |
|---|---|---|
| Windows | 11 | — |
| EDK2 | Latest | `C:\edk2` (short path avoids Windows MAX_PATH issues) |
| Visual Studio | 2022 with C++ | Default install |
| NASM | Latest | `C:\nasm` |
| Python | 3.x | In PATH |
| QEMU | Latest | Default install |

### Build Steps

```bat
:: 1. Open VS2022 x64 Native Tools Command Prompt
:: 2. Navigate to EDK2 root
cd C:\edk2

:: 3. Initialize EDK2 build environment
edksetup.bat

:: 4. Build the application
build -p MyApps/MyApps.dsc -a X64 -t VS2022 -b DEBUG
```

**Output EFI binary:**
```
C:\edk2\Build\MyApps\DEBUG_VS2022\X64\UEFICalculator.efi
```

---

##  How to Run

### Launch QEMU with OVMF

```bat
"C:\Program Files\qemu\qemu-system-x86_64.exe" ^
  -drive if=pflash,format=raw,readonly=on,file="C:\Program Files\qemu\share\edk2-x86_64-code.fd" ^
  -drive file=fat:rw:C:\edk2\Build\MyApps\DEBUG_VS2022\X64,format=raw
```

### Inside UEFI Shell

```
Shell> UEFICalculator.efi
```

From the main menu:

```
[7] Drone Swarm Simulator  ← select this
  [1] Show Swarm Status       — Live health, fuel, weapons, MissionValue
  [2] Simulate Bullet Hit     — Damage detection via sensor anomaly
  [3] Simulate Missile Threat — Full sacrifice decision engine
  [4] Run Full Cycle          — Heartbeat + damage + sacrifice together
  [5] Reset Swarm             — Restore formation to initial state
  [6] Back to Main Menu
```

---

##  File Structure

```
UEFI-Drone-Swarm-Decision-Engine/
├── DroneSwarm/
│   ├── DroneSwarm.c        ← Modularized drone swarm engine (Phase 3+)
│   └── DroneSwarm.h        ← Struct definitions, constants, function prototypes
├── UEFICalculator.c        ← Complete firmware source (~3170 lines, all menus)
├── UEFICalculator.inf      ← EDK2 module definition (build metadata)
└── README.md
```

### Navigation Map for UEFICalculator.c

The file is long by design — it's a multi-menu firmware application. Here's exactly where everything lives:

```
Lines 1    – 23     → memcpy / memset stubs (bare-metal baseline)
Lines 24   – 200    → Core helpers: GetChoice, GetNumber, WaitForKey
Lines 201  – 600    → Menus 1–5: Calculator, Sorting, Encryption, etc.
Lines 601  – 1848   → System Information and remaining menus
Lines 1848 – 2540   → Menu 6: GNSS Telemetry Parser
Lines 2540 – 3097   → Menu 7: Drone Swarm Decision Engine  ← YOU ARE HERE
Lines 3098 – 3170   → UefiMain() entry point
```

Jump directly to **line 2540** for the drone swarm engine.

---

## 🗺️ Development Roadmap

```
Phase 1 → GNSS Telemetry Parser          ✅ Complete
           Raw NMEA → integer coordinates → range computation
           github.com/Fakhruddin0139/UEFI-GNSS-Telemetry-Parser

Phase 2 → Drone Swarm Decision Engine    ✅ Complete  ← This repo
           5-drone formation, MissionValue scoring,
           sacrifice engine, watchdog, damage detection

Phase 3 → Movement Simulation            🔄 In Progress
           Drones advance toward waypoint each cycle
           Formation gap filled after sacrifice
           Position updates reflected in live distance calculations

Phase 4 → Leader Selection Engine        📋 Planned
           When lead drone is destroyed or sacrificed:
           remaining drones evaluated by MissionValue
           highest-value unit promoted to lead
           formation restructures around new leader

Phase 5 → Death Rattle Protocol          📋 Planned
           Dying drone broadcasts final state to swarm
           Swarm reconfigures formation around fallen unit

Phase 6 → Threat Response Engine         📋 Planned
           Classify inbound threat by speed, altitude, signal strength
           Select cost-proportional response: missile vs long-range gun
           Proportionality scoring to avoid wasting resources
```

---

## 🧩 Architecture Decision Log

Key technical decisions made during development — and why:

| Decision | Options Considered | Chosen | Reason |
|---|---|---|---|
| Distance math | Haversine vs Equirectangular | Equirectangular | No trig available in UEFI; error <0.3% under 50 km |
| Priority weight | ×50 vs ×25 vs ×35 | ×35 hybrid | Protects key assets without making them immortal |
| Dead drone MissionValue | 0 vs MAX | 9999999 (MAX) | Prevents tie-breaker from selecting dead units |
| DroneID type | CHAR8 vs CHAR16 | CHAR16 | Safe printing with EDK2 `Print()` which requires Unicode |
| Longitude multiplier | Hardcoded vs parameter | Parameter | Works at any latitude, not just Central India |
| Sqrt method | Lookup table vs Newton-Raphson | Newton-Raphson | No ROM budget concern in UEFI; cleaner, accurate |

---

## 🔗 Related Work

| Repo | Description |
|---|---|
| [UEFI-GNSS-Telemetry-Parser](https://github.com/Fakhruddin0139/UEFI-GNSS-Telemetry-Parser) | Phase 1 — raw NMEA sentence parsing at firmware layer |

---

## 👤 About

Built by **MD Fakhruddin** — B.Tech CSE '26, completed internship at **DRDO's Telemetry Division, ITR Chandipur**

If you've worked on swarm systems, autonomous decision-making, or embedded firmware — I'd genuinely like to hear from you.

If you're learning embedded systems and something here is unclear — reach out. Happy to explain.

[![LinkedIn](https://img.shields.io/badge/LinkedIn-Connect-blue?style=flat-square&logo=linkedin)](https://linkedin.com/in/fakhruddin3a)
[![GitHub](https://img.shields.io/badge/GitHub-Follow-black?style=flat-square&logo=github)](https://github.com/Fakhruddin0139)
[![GNSS Repo](https://img.shields.io/badge/Phase%201-GNSS%20Parser-green?style=flat-square)](https://github.com/Fakhruddin0139/UEFI-GNSS-Telemetry-Parser)

---

<div align="center">

*The firmware knows where they are.*  
*The algorithm decides what they do.*

</div>
