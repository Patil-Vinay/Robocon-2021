# Robocon 2021

SolidWorks design, Arduino code and wiring of our two robots for Robocon 2021, where the task was to
throw arrows into pots. The Defence Robot (DR) picks arrows up and passes them on. The Throw Robot
(TR) stores arrows and shoots them. Both robots are driven by hand with a Bluetooth remote.

## Defence Robot (DR)

![DR](images/dr_robot.png)

- Aluminium box section frame with a 2 mm sheet on top. 4 omni wheels, each driven by a 256 rpm
  planetary motor.
- A robotic arm with 4 degrees of freedom. Its last link is a universal gripper that is worked
  pneumatically by a 100 ml syringe.
- The arm picks arrows up from the ground. It can place an arrow in the shooting channel or drop it
  in the outer area for TR to pick up.
- The red claw sits on a vertical rack and pinion so its height can be changed.
- Same spring shooting channel as TR (see below).
- Electronics box with an Arduino Mega, battery and MG996R servos. The CAD has an HC-SR04 in it but
  the code uses a TFmini Plus LiDAR to measure the distance to the pot.

## Throw Robot (TR)

![TR](images/tr_robot.png)

TR has the same base, arm and gripper as DR. It picks arrows up from the arrow stand and the ground.

The extra part on TR is the arrow revolver. It holds 5 arrows with one more on the shooting channel.

- The arm puts an arrow in the wider top end of the revolver. The arrow slides down into the
  revolving chamber.
- The red plates rotate while the outer shell stays still.
- When an arrow reaches the lowest position a servo operated flap opens and the arrow falls onto the
  shooting channel.
- A motor at the bottom of the shooting channel pulls the grey pushing part back and stretches the
  two springs. Letting go of it shoots the arrow.
- The angle of the channel is set by a NEMA17 stepper driving the big curved gear (a section of a
  250 tooth internal gear).

## Code

Each board has one Arduino sketch:

- `code/remote/remote.ino` - the handheld remote (Arduino Nano, two joysticks, 8 buttons and an
  HC-05). It sends the stick values and buttons as a text line every 40 ms. Both robots use the
  same remote.
- `code/tr/tr.ino` - TR on an Arduino Mega. Drive, arm and gripper, the revolver feed and the
  spring shooter.
- `code/dr/dr.ino` - DR on an Arduino Mega. Drive, arm and gripper, the claw, the block pose and
  the spring shooter.

How the shooter works in the code:

- The winch stepper pulls the pusher back, the latch servo locks it and the winch lets the string
  out again. Opening the latch shoots the arrow.
- On TR the first press of B5 cocks the shooter and drops an arrow from the revolver. The next press
  shoots.
- B6, B7 and B8 aim at the Type-I, Type-II and Type-III pot. The robot reads the distance from the
  LiDAR and works out the channel angle for a high arc.

The button list for each robot is at the top of its sketch. The arm poses, latch angles and launch
speed in the sketches are starting values and need to be set on the robot.

To upload, open the sketch in the Arduino IDE, install AccelStepper from the Library Manager (Servo
and SoftwareSerial come with the IDE) and pick Arduino Mega 2560 for the robots or Arduino Nano for
the remote. The robot's HC-05 is a slave at 9600 baud and the remote's HC-05 is set as master and
bound to it.

## Wiring

Schematics for both robots and the remote are in `docs/wiring/`. The pin numbers match the sketches.

## Files

| Path | What it is |
|---|---|
| `cad/dr` | SolidWorks parts and `Defence Robot.SLDASM` |
| `cad/tr` | SolidWorks parts and `Throw Robot.SLDASM` |
| `step/` | Zipped STEP export of each full robot (about 33 MB each once unzipped) |
| `code/` | Arduino sketches for TR, DR and the remote |
| `docs/wiring/` | Wiring schematics |
| `images/` | Renders |
| `docs/parts.md` | Parts list with counts for both robots |

## Opening the files

The .SLDPRT and .SLDASM files were saved in SolidWorks 2020 so they need SolidWorks 2020 or newer.
Toolbox parts (gears, bearings, screws and nuts) and a few downloaded models (pneumatic cylinder,
HC-SR04, omni wheels, one servo) are not in `cad/`. SolidWorks will ask for them when you open an
assembly.

Without SolidWorks, unzip the file in `step/` and open the .STEP in Fusion 360, FreeCAD or any other
CAD program. The STEP has the whole robot with every part in it.
