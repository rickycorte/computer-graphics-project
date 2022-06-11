# Missile Simulator
Design a simulator of a point-to-point missile in a 3D landscape. The point of view must be in third-person. The
missile must follow a parabolic-like trajectory and its orientation must be interpolated accordingly

## Features
- Missile parabolic trajectory simulation
- Third person camera that rotates around the missile
- Topdown aim mode to move missle destination
- Simple terrain collision detection
- Skybox
- Two render pipelines, lit (global lights) + unlit
- Global direction light
- Hemispheric ambient light
- Spot light that emulates the missile engine lighting
- Animated point light that emulates a navigation light on top of the missile

## Controls
Buttons | Action
--- | --- 
Arrow Up/Down/Left/Right | Rotate around missile/Move missile destination in aim mode
Mouse 1 + Mouse Drag |  Rotate around missile
M | Toggle aim mode
Space | Start/Reset simulation
