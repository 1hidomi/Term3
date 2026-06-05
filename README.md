# Term3 Arduino GIGA Robot

Autonomous Arduino GIGA R1 WiFi robot software for the Term 3 arena challenge.
The current integrated controller combines black-line navigation, IMU-guided
turning, RFID checkpoint handling, MiniMessenger server communication, ultrasonic
obstacle awareness, wall following, seed planting, and emergency return behavior.

The codebase is developed as a PlatformIO project. The public GitHub submission
also contains exported top-level source files, diagrams, and testing evidence for
review.

## Current Source Layout

GitHub submission snapshot:

```text
.
|-- main.cpp                         # Integrated competition controller
|-- Field_planting_seed.cpp          # Standalone field planting controller
|-- Return_Base.cpp                  # Standalone emergency return test
|-- platform_io                      # PlatformIO configuration used for export
|-- secrets.h                        # Local credentials file; do not publish real values
|-- software_overview_diagram.png    # Software architecture diagram
|-- flowcharts/
|   |-- mechanical_kill_switch.png
|   |-- RFID_planting.png
|   `-- line_following.png
`-- Testing_Calibration_Evidence/    # Logs, screenshots, and field-test videos
```

Local PlatformIO development workspace:

```text
.
|-- platformio.ini
|-- src/
|   |-- main.cpp                     # Default integrated controller
|   |-- pre_emergency_planting.cpp   # Standalone planting environment
|   |-- back_to_base.cpp             # Standalone return-to-base environment
|   `-- Candidate_Code/              # Focused experiments and diagnostics
|-- calibration_evidence/            # Evidence collection guide
|-- data/                            # Historical failed-run notes
|-- saved_versions/                  # Dated source snapshots
`-- tools/                           # Small helper scripts
```

## Main Controller Behavior

- Starts in a stopped safety state and waits for the mechanical D45 run toggle.
- Registers with the challenge server through MiniMessenger when online mode is enabled.
- Continuously services server messages, buttons, LEDs, encoders, IMU yaw, RFID, servo, and ultrasonic sensors.
- Follows the black line using five reflectance outputs and PID steering.
- Detects corners and T-junctions, then performs IMU-guided 90 degree or 180 degree turns.
- Uses short search turns, yaw-hold driving, or tunnel wall following when the line is lost.
- Treats the first RFID as the airlock checkpoint and later RFID tags as fertility queries.
- Plants only after a positive server fertility reply and queues a `seedPlanted` report after the servo cycle completes.
- On emergency, stops the current action and plans a return route toward grid exit `(9,7)`.

## Required Libraries

PlatformIO installs these from `lib_deps`:

- `Motoron` from Pololu for dual motor shield control.
- `MFRC522_I2C` for the WS1850S / MFRC522-compatible RFID module.
- `SparkFun 9DoF IMU Breakout - ICM 20948 - Arduino Library`.
- `ArduinoMqttClient`.
- `MiniMessenger`.
- `Servo`.
- Built-in `Arduino` and `Wire`.

## Hardware Map

### I2C Devices

| Device | Bus | Address / Notes |
|---|---|---|
| ICM-20948 IMU | `Wire` / `SDA` + `SCL` | Tries `0x68` and `0x69` |
| Motoron three-motor shield | `Wire1` / `SDA1` + `SCL1` | `0x11`, drives RF/RR/LR |
| Motoron one-motor shield | `Wire1` / `SDA1` + `SCL1` | `0x10`, drives LF |
| RFID reader | `Wire1` / `SDA1` + `SCL1` | `0x28` |

The current robot stack uses the rebuilt shield order from the code: lower
logical shield at `0x11`, upper logical shield at `0x10`.

### Motors And Encoders

| Wheel | Motoron channel | Encoder A | Encoder B |
|---|---:|---:|---:|
| RF | lower M1 | D18 | D19 |
| RR | lower M2 | D2 | D3 |
| LR | lower M3 | D4 | D5 |
| LF | upper M1 | D8 | D9 |

### Reflectance Sensors

The line follower uses five selected outputs from the reflectance board:

| Logical position | Board output | Arduino pin |
|---|---|---:|
| Right | OUT1 | D22 |
| Slight right | OUT3 | D24 |
| Middle | OUT5 | D26 |
| Slight left | OUT7 | D28 |
| Left | OUT9 | D31 |

OUT2/OUT4/OUT6/OUT8 are not used by the main controller.

### Ultrasonic, Buttons, LEDs, Servo

| Part | Pin / Wiring |
|---|---|
| Front ultrasonic | Echo D37, Trig D39 |
| Left ultrasonic | Echo D52, Trig D44 |
| Right ultrasonic | Echo D46, Trig D48 |
| Revive button | D32 to GND, input pullup |
| Mechanical kill/run toggle | D45 to GND, input pullup |
| Red status LED | D36 |
| Green status LED | D38 |
| Seed planter servo | Signal D33, 5V, GND |

## Secrets Configuration

Create `src/secrets.h` for local PlatformIO development. For Arduino IDE export,
keep `secrets.h` in the same sketch folder as the final `.ino` file.

Do not commit real Wi-Fi, broker, team, or board credentials. Use a private local
file like this:

```cpp
#pragma once

#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define BROKER_HOST "YOUR_BROKER_HOST_OR_IP"
#define BROKER_PORT 1883
#define GROUP_ID "YOUR_TEAM_OR_GROUP_ID"
#define BOARD_ID "YOUR_BOARD_ID"
```

If real credentials were previously published in the repository, rotate them
before using the network again.

## Build And Upload

Default integrated controller:

```bash
pio run
pio run -t upload
pio device monitor -e giga_r1_m7 --port /dev/cu.usbmodem101 --baud 115200
```

If `pio` is not on the shell path, use the PlatformIO virtualenv executable:

```bash
/Users/admin/.platformio/penv/bin/platformio run
```

Useful PlatformIO environments:

| Environment | Purpose |
|---|---|
| `giga_r1_m7` | Default integrated controller from `src/main.cpp` |
| `pre_emergency_planting` | Standalone seed planting and field exploration |
| `back_to_base` | Standalone emergency return-to-base test |
| `follow_line` / `fast_pid_line` | Line follower experiments |
| `wall_following` | Tunnel wall-following experiment |
| `obstacle_avoid` | Ultrasonic obstacle behavior experiment |
| `encoder_speed_diagnostic` | Encoder speed and wheel response diagnostic |

Example:

```bash
pio run -e back_to_base
pio run -e pre_emergency_planting -t upload
```

## Arduino IDE Export

PlatformIO is the preferred development path. For a final Arduino IDE submission:

1. Copy the validated integrated source into a sketch file such as `final.ino`.
2. Put a private `secrets.h` tab in the same sketch folder.
3. Install the required libraries listed above.
4. Select `Arduino Mbed OS GIGA Boards -> Arduino GIGA R1`.
5. Verify, upload, and open Serial Monitor at `115200` baud.

## Operating Checklist

1. Put the robot on a safe test surface with wheels clear until ready.
2. Connect USB-C and open Serial Monitor at `115200`.
3. Keep the robot still during IMU gyro and tilt calibration.
4. Confirm startup logs for IMU, Motoron addresses `0x10` and `0x11`, RFID `0x28`, and ultrasonic pins.
5. Confirm the red LED startup wait. Press D45 once to allow motion.
6. Use D45 to toggle movement stop/run during local testing.
7. Use D32 for revive/green status indication after manual intervention.
8. When online, keep the laptop/debug device on the lab network and verify server dashboard enable/disable behavior.

## Server Message Flow

Outgoing messages:

- `type=register`
- `type=openAirlock`
- `type=isFertile`
- `type=seedPlanted`

Incoming messages handled by the robot:

- `type=heartbeat enable=...`
- `type=disable enabled=...`
- `type=emergency enabled=...`
- `type=openAirlockReply accepted=...`
- `type=isFertileReply fertile=... planted=... x=... y=...`

The main loop must continue calling `serverMessenger.loop()` so emergency,
disable, and RFID replies are processed promptly.

## Testing And Evidence

Run a compile before submitting code changes:

```bash
pio run
```

Capture serial evidence with:

```bash
pio device monitor -e giga_r1_m7 --port /dev/cu.usbmodem101 --baud 115200
```

Recommended evidence:

- PlatformIO compile success.
- Startup log showing IMU, Motoron, RFID, ultrasonic, and sensor initialization.
- Reflectance calibration on black line and floor.
- Line following and junction-turn logs.
- RFID fertility/planting log.
- Emergency disable and return behavior log.
- Short videos for line following, U-turns, slope/tunnel behavior, and planting.

## Notes And Risks

- Pin assignments, Motoron addresses, and motor polarity are hardware-specific.
- Reflectance thresholds are tuned for the lowered sensor board and may need recalibration on a different surface.
- Encoder speed governing assumes all four encoder channels are connected and clean.
- The robot intentionally starts stopped; verify D45 before expecting motion.
- Keep the robot still during startup IMU calibration, or yaw-based turns and return behavior will drift.
