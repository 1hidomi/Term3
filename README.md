Term3 Arduino GIGA Robot

Autonomous Arduino GIGA R1 WiFi robot software for the Term 3 arena challenge.
The current integrated controller combines black-line navigation, IMU-guided
turning, RFID checkpoint handling, MiniMessenger server communication, ultrasonic
obstacle awareness, wall following, seed planting, and emergency return behavior.

The codebase is developed as a PlatformIO project. The public GitHub submission
also contains exported top-level source files, diagrams, and testing evidence for
review.

|-- final.cpp                      
|-- planting.cpp                     # Standalone field planting controller
|-- return_to_base.cpp               # Standalone emergency return test
|-- platform_io                      # PlatformIO configuration used for export
|-- secrets.h                        # Header file
|-- software_overview_diagram.png    # Software architecture diagram
|-- flowcharts/
|   |-- mechanical_kill_switch.png
|   |-- RFID_planting.png
|   `-- line_following.png
`-- Testing_Calibration_Evidence/    # Logs, screenshots, and field-test videos


Main Controller Behavior

- Starts in a stopped safety state and waits for the mechanical D45 run toggle.
- Registers with the challenge server through MiniMessenger when online mode is enabled.
- Continuously services server messages, buttons, LEDs, encoders, IMU yaw, RFID, servo, and ultrasonic sensors.
- Follows the black line using five reflectance outputs and PID steering.
- Detects corners and T-junctions, then performs IMU-guided 90 degree or 180 degree turns.
- Uses short search turns, yaw-hold driving, or tunnel wall following when the line is lost.
- Treats the first RFID as the airlock checkpoint and later RFID tags as fertility queries.
- Plants only after a positive server fertility reply and queues a `seedPlanted` report after the servo cycle completes.
- On emergency, stops the current action and plans a return route toward grid exit `(9,7)`.

Required Libraries

PlatformIO installs these from `lib_deps`:

- `Motoron` from Pololu for dual motor shield control.
- `MFRC522_I2C` for the WS1850S / MFRC522-compatible RFID module.
- `SparkFun 9DoF IMU Breakout - ICM 20948 - Arduino Library`.
- `ArduinoMqttClient`.
- `MiniMessenger`.
- `Servo`.
- Built-in `Arduino` and `Wire`.

Secrets Configuration

Keep `secrets.h` in the same sketch folder as the final `.ino` file.

Do not commit real Wi-Fi, broker, team, or board credentials. Use a private local
file like this:

```
#pragma once

#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define BROKER_HOST "YOUR_BROKER_HOST_OR_IP"
#define BROKER_PORT 1883
#define GROUP_ID "YOUR_TEAM_OR_GROUP_ID"
#define BOARD_ID "YOUR_BOARD_ID"
```

Build And Upload

Default integrated controller:

```
pio run
pio run -t upload
pio device monitor -e giga_r1_m7 --port /dev/cu.usbmodem101 --baud 115200
```

If `pio` is not on the shell path, use the PlatformIO virtualenv executable:

```
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

```
pio run -e back_to_base
pio run -e pre_emergency_planting -t upload
```

Arduino IDE Export

PlatformIO is the preferred development path. For a final Arduino IDE submission:

1. Copy the validated integrated source into a sketch file such as `final.ino`.
2. Put a private `secrets.h` tab in the same sketch folder.
3. Install the required libraries listed above.
4. Select `Arduino Mbed OS GIGA Boards -> Arduino GIGA R1`.
5. Verify, upload, and open Serial Monitor at `115200` baud.

Operating Checklist

1. Put the robot on a safe test surface with wheels clear until ready.
2. Connect USB-C and open Serial Monitor at `115200`.
3. Keep the robot still during IMU gyro and tilt calibration.
4. Confirm startup logs for IMU, Motoron addresses `0x10` and `0x11`, RFID `0x28`, and ultrasonic pins.
5. Confirm the red LED startup wait. Press D45 once to allow motion.
6. Use D45 to toggle movement stop/run during local testing.
7. Use D32 for revive/green status indication after manual intervention.
8. When online, keep the laptop/debug device on the lab network and verify server dashboard enable/disable behavior.

Server Message Flow

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

Testing And Evidence

Run a compile before submitting code changes:

```
pio run
```

Capture serial evidence with:

```
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
