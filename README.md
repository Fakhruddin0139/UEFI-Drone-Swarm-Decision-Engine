# UEFI-Drone-Swarm-Decision-Engine

> A bare-metal autonomous drone swarm simulator running entirely at the 
> UEFI firmware layer — before any operating system loads.

Built in pure C using EDK2. Zero standard libraries. Zero floating-point math.
Zero operating system.
> ⚠️ **Educational simulation only.** Runs in QEMU virtual machine with hardcoded dummy data. No real hardware, weapons, or classified information involved.

---

## What This Is

This is **Menu 7** of a multi-menu UEFI firmware application.

A swarm of 5 drones — Payload Carrier, Escorts, and Scouts — operates in 
formation toward a target waypoint. When a missile threat is detected, the 
system evaluates every drone's combat value in real time and mathematically 
selects the most expendable unit to break formation and intercept the threat.

The payload carrier is never sacrificed unless critically damaged.
The weakest scout always goes first.
No human command needed. No operating system involved.



---

## Live Demo — Running in UEFI Shell

**Swarm Status Monitor:**
<img width="972" height="645" alt="swarm_status" src="https://github.com/user-attachments/assets/cf518e00-e347-41fb-83ee-dd21d155d745" />

**Bullet Hit Simulation:**
<img width="708" height="398" alt="bullet_hit" src="https://github.com/user-attachments/assets/f6714b19-dc39-47d8-8403-07dd8bcecb6e" />


**Sacrifice Order Issued:**
<img width="846" height="658" alt="sacrifice_order" src="https://github.com/user-attachments/assets/e3ecd3a2-ad7e-4a11-a945-ad422063cf1f" />


**Swarm Reset:**
<img width="770" height="475" alt="reset" src="https://github.com/user-attachments/assets/5060af7a-71cb-4086-8c42-5d32edb0b2cf" />


---

## What It Does

```
[1] Show Swarm Status       — Live health, fuel, weapons, MissionValue
[2] Simulate Bullet Hit     — Damage detection via sensor anomaly
[3] Simulate Missile Threat — Full sacrifice decision engine
[4] Run Full Cycle          — Heartbeat + damage + sacrifice together
[5] Reset Swarm             — Restore formation to initial state
[6] Back to Main Menu
```

---

## Five Drone Formation

| Drone   | Role            | Health | Fuel | Weapons | Priority |
|---------|----------------|--------|------|---------|----------|
| DRONE-A | Payload Carrier | 85%   | 80%  | 5       | 90       |
| DRONE-B | Escort          | 80%   | 75%  | 4       | 50       |
| DRONE-C | Damaged Escort  | 35%   | 40%  | 2       | 50       |
| DRONE-D | Scout           | 70%   | 65%  | 2       | 20       |
| DRONE-E | Critical Scout  | 25%   | 30%  | 1       | 20       |

DRONE-E always sacrifices first. DRONE-A is always protected.
The math decides. Not the code.

---

## Technical Architecture

### MissionValue Scoring Formula
```
MissionValue = (Health × 38) + (Weapons × 30) + (Fuel × 22) + (Priority × 35)

DRONE-A: (85×38) + (5×30)  + (80×22) + (90×35) = 8290  ← Protected
DRONE-E: (25×38) + (1×30)  + (30×22) + (20×35) = 2340  ← Sacrificed
```

Weights designed so Priority protects key assets but doesn't make
them immortal. A critically damaged payload carrier can still be
sacrificed in extreme cases.

### Distance Calculation (Equirectangular Approximation)
```
THM = (Degrees × 60 × 100) + (Minutes × 100) + Seconds
      1 THM ≈ 18.52 meters physical distance

ΔY = ΔLatTHM × 18          (latitude constant)
ΔX = ΔLonTHM × 17          (longitude at Central India ~24°N)

Distance = √(ΔX² + ΔY²)    (integer Pythagorean theorem)
```

No `math.h`. No `cos()`. No floats. Pure integer arithmetic.
Squaring handles negative deltas automatically — no `abs()` needed.

### Damage Detection
```
CurrentChange = CurrentMotorCurrent - BaseMotorCurrent
DragChange    = CurrentDrag - BaseDrag

CurrentChange > 25 OR DragChange > 30  →  Major hit: Health -= 35
CurrentChange > 12 OR DragChange > 15  →  Minor hit: Health -= 18
Health <= 0                             →  IsAlive = FALSE
```

Based on real sensor physics — bullet impact increases motor load
and disrupts aerodynamics, both measurable without explicit hit detection.

### Heartbeat Watchdog Timer
```
Every cycle: drone updates LastHeartbeatTick
If GlobalTick - LastHeartbeatTick > 3 → drone declared dead

Same pattern used in:
- Apache ZooKeeper node failure detection
- Kubernetes pod health monitoring
- Military mesh radio networks
```

When ported to real hardware — replace GlobalTick with UART
packet timestamp. Core logic unchanged.

### Sacrifice Selection Algorithm
```
Step 1: Reset all SacrificeFlags
Step 2: Recalculate DistanceToTarget for all alive drones
Step 3: Recalculate MissionValue for all alive drones
Step 4: Find drone with minimum MissionValue
Step 5: Tie-break — equal MissionValue → closest drone to threat
Step 6: Set SacrificeFlag = TRUE
```
O(n) complexity. Single pass. Real-time capable.

---

## Bare-Metal Constraints

Running before any OS means zero access to standard libraries.
Every helper function written from scratch.

| Unavailable  | Our Solution          |
|--------------|-----------------------|
| `max()`      | `GetMax()` ternary    |
| `min()`      | `GetMin()` ternary    |
| `abs()`      | `AbsVal()` ternary    |
| `sqrt()`     | Newton-Raphson method |
| `memcpy()`   | Custom stub           |
| `memset()`   | Custom stub           |
| `strcpy()`   | EDK2 `StrCpyS()`      |
| `malloc()`   | Stack allocation only |
| `float`      | Fixed-point integers  |

---

## How to Build

**Requirements:**
- Windows 11
- EDK2 cloned at `C:\edk2`
- Visual Studio 2022
- NASM at `C:\nasm`
- QEMU installed

**Build command** (from `C:\edk2` after `edksetup.bat`):
```bash
build -p MyApps/MyApps.dsc -a X64 -t VS2022 -b DEBUG
```

**Output EFI:**
```
C:\edk2\Build\MyApps\DEBUG_VS2022\X64\UEFICalculator.efi
```

---

## How to Run

**Launch QEMU:**
```bash
"C:\Program Files\qemu\qemu-system-x86_64.exe" ^
  -drive if=pflash,format=raw,readonly=on,^
  file="C:\Program Files\qemu\share\edk2-x86_64-code.fd" ^
  -drive file=fat:rw:C:\edk2\Build\MyApps\DEBUG_VS2022\X64,format=raw
```

**Inside UEFI shell:**
```
UEFICalculator.efi
→ Press [7] Drone Swarm Simulator
→ Press [3] Simulate Missile Threat
```

---

## File Structure

```
UEFI-Drone-Swarm-Decision-Engine/
├── UEFICalculator.c        # Complete firmware source (3170 lines)
├── UEFICalculator.inf      # EDK2 module definition
└── README.md               # This file
```

### Finding What You Need in UEFICalculator.c

This is a multi-menu application. The file is long by design.
Here is exactly where everything lives:

```
Lines 1    - 23    → memcpy / memset stubs
Lines 24   - 200   → Core helpers (GetChoice, GetNumber, WaitForKey)
Lines 201  - 600   → Menu 1-5 (Calculator, Sorting, Encryption etc.)
Lines 601  - 1848  → System Information, remaining menus
Lines 1848 - 2540  → Menu 6: GNSS Telemetry Parser
Lines 2540 - 3097  → Menu 7: Drone Swarm Decision Engine ← YOU ARE HERE
Lines 3098 - 3170  → UefiMain() entry point
```

If you're here for the drone swarm — jump to **line 2540**.

---

## What's Coming — Phase 3

```
Phase 1 → GNSS Parser              ✅ Complete
           github.com/Fakhruddin0139/UEFI-GNSS-Telemetry-Parser

Phase 2 → Drone Swarm Simulator    ✅ Complete
           This repo

Phase 3 → Movement Simulation      ← Next
           Drones moving toward waypoint each cycle
           Formation gap filled after sacrifice
           Position updates reflected in distance calculations

Phase 4 → Leader Selection Engine  ← Planned
           When lead drone is destroyed or sacrificed
           Swarm evaluates remaining units
           Highest MissionValue drone promoted to lead
           Formation restructures around new leader

Phase 5 → Death Rattle Protocol    ← Planned
           Final broadcast before drone dies
           Swarm reconfigures around fallen unit

Phase 6 → Threat Response Engine   ← Planned
           Detect target drone condition (speed, altitude, signal)
           Select cost-proportional response weapon
           Expensive missile vs long-range gun based on threat level
```

---

## Architecture Decision Log

Major technical decisions and the reasoning behind each:

| Decision | Options | Chosen | Reason |
|----------|---------|--------|--------|
| Distance math | Haversine vs Equirectangular | Equirectangular | No trig in UEFI, swarm area < 50km |
| Priority weight | ×50 vs ×25 vs ×35 | ×35 hybrid | Protects key assets without making them immortal |
| Dead drone return | 0 vs 9999999 | 9999999 | Prevents tie-breaker selecting dead units |
| DroneID type | CHAR8 vs CHAR16 | CHAR16 | Safe printing with EDK2 Print() |
| LonMultiplier | Hardcoded vs parameter | Parameter | Works at any latitude, not just India |

---

## Connect

Built after completing internship at **DRDO's Telemetry Division, ITR Chandipur** — applying concepts learned during six months of bare-metal firmware development.

If you've worked on swarm systems, autonomous decision-making,
or embedded firmware — I'd genuinely love to hear from you.

If you're learning embedded systems and something here is unclear —
reach out. Happy to explain.

If you're in the industry and see potential here —
I'm open to challenges, tasks, and collaboration.

**MD Fakhruddin**
B.Tech CSE '26 | Ex-DRDO Intern (ITR, Chandipur)

[![LinkedIn](https://img.shields.io/badge/LinkedIn-Connect-blue)](https://linkedin.com/in/fakhruddin3a)
[![GitHub](https://img.shields.io/badge/GitHub-Follow-black)](https://github.com/Fakhruddin0139)
[![GNSS Repo](https://img.shields.io/badge/Phase1-GNSS%20Parser-green)](https://github.com/Fakhruddin0139/UEFI-GNSS-Telemetry-Parser)

---

*The firmware knows where they are.*
*The algorithm decides what they do.*
