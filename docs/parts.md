# Parts list

Counted from the STEP exports in `step/`. The numbers are how many times each part is used in the
Defence Robot (DR) and Throw Robot (TR) assemblies.

Parts marked *toolbox* came from the SolidWorks Toolbox and parts marked *downloaded* are models
we got online. There is no .SLDPRT for these in `cad/` so SolidWorks may show them as missing when
opening the .SLDASM. The STEP export has all of them.

## Chassis

| Part | DR | TR |
|---|---|---|
| 750x35x35 45cut | 2 | 2 |
| 650x35x35 45cut | 2 | 2 |
| 650x26x26 | 1 | 1 |
| 342x26x26 (last 30mm bent) | 2 | 2 |
| 25x25x350 | 1 | - |
| 25x25x300 Al tube | 1 | 1 |
| 10x20x300 mm | - | 2 |
| 500x750x2mm sheet | 1 | 1 |
| 100x60x2 plate | - | 1 |
| Base block | 2 | 2 |

## Drive

| Part | DR | TR |
|---|---|---|
| 256 rpm Planetary Motor | 4 | 4 |
| Omni wheel (VEX-217-2584, downloaded) | 4 | 4 |
| 10x100 shaft | 4 | 4 |
| shaft spacer M10x10mm | 4 | 4 |

## Arm and gripper

| Part | DR | TR |
|---|---|---|
| arm base | 1 | 1 |
| arm x axis rotation | 1 | 1 |
| arm link | 1 | 1 |
| arm hand | 1 | 1 |
| universal gripper | 1 | 1 |
| M10 170mm Hollow shaft | 1 | 1 |
| Pneumatic cylinder (cylinder, piston, gland, downloaded) | 1 | 1 |

## Shooting channel

| Part | DR | TR |
|---|---|---|
| Arrow channel DR 2 | 1 | - |
| Arrow channel | - | 1 |
| pushing part | 1 | 1 |
| spring - 1 in dia | 2 | 2 |
| Pulley 2 | 1 | 1 |
| 10x80 shaft | 1 | 1 |
| Internal gear2 (2M 250T internal gear, toolbox) | 1 | 1 |
| NEMA17 Stepper | 2 | 3 |
| Arrow | 1 | 4 |

## DR only

| Part | DR |
|---|---|
| claw base | 1 |
| Rack 2M 200 mm (toolbox) | 1 |

## TR only (arrow revolver)

| Part | TR |
|---|---|
| Arrow revolver plate | 2 |
| Arrow revolver cover | 1 |
| Arrow revolver shaft | 1 |
| arro revolver front sopport | 2 |
| revolver front holder | 1 |
| flap a | 1 |
| servo link | 1 |

## Electronics

| Part | DR | TR |
|---|---|---|
| ArduinoMega | 1 | 1 |
| battery | 1 | 1 |
| electronics box | 1 | 1 |
| Ultrasonic sensor HC-SR04 (downloaded) | 1 | 1 |
| Servo MG996R | 3 | 2 |
| Сервопривод (servo, downloaded) | 1 | 2 |
| Model with a Cyrillic name that got lost in the export (downloaded) | - | 1 |

## Gears and hardware

| Part | DR | TR |
|---|---|---|
| Spur gear 2M 10T (toolbox) | 2 | 2 |
| Spur gear 2M 20T (toolbox) | 3 | 2 |
| Spur gear 2M 30T (toolbox) | 1 | 1 |
| Thrust ball bearing 10 mm (toolbox) | 8 | 8 |
| shaft spacer M8x10 | 2 | 2 |
| shaft spacer M8x5.5 | 6 | 6 |
| M8 pan head screw (40 mm on DR, 45 mm on TR, toolbox) | 2 | 2 |
| M8 hex nut (toolbox) | 2 | 2 |

## Files in cad/ that are not in the final assembly

These are in the folders but the final assembly doesn't use them.

- `cad/dr`: 100x60x2 plate, 10x20x200 mm, 10x20x300 mm, 10x60 shaft, 650x35x35, Arrow channel,
  Arrow channel DR, arrow holder, arrow holder shaft, flap a, L, pulley, pulley holder
- `cad/tr`: 10x20x200 mm, 10x60 shaft, 25x25x350, 650x35x35, Arrow channel DR, Arrow channel DR 2,
  arrow holder shaft, back support.smg, Container link, L, Motor - RS-775.step, pulley, pulley holder
