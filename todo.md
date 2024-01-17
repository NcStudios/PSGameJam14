
# Must Do
Fix moving static, but non-kinematic physics bodies in engine (or just don't have PB?)

Level design

Dialog

Test audio on other devices, bumping buffer to 512 fixed my machine - may just be debug build issue

Shadow wonkiness

Add some interaction between vehicle and infected trees
  controls + purifier spawning added, still need to change collision mechanic to time-based

Improve movement/camera
  not perfect, but in a 'tweakable' state
  improve general feel and steering control
  make sure follow distance looks good (requires rough to-scale assets for testing)
  X jump isn't infinite

Keep vehicle upright, or force back somehow
  - can we restrict PhysicsBody freedom?
    - is in world space, so must do x and z... its not the worst looking. could use as fallback, or maybe set to like 0.1f?

Visual representation of healthy/infected tree count (UI, but...) [not sure if 'must' or 'nice to have']

Purify
  update to only emit when key held, shape is probably a long box instead
  make particle
  X make sfx

Healthy -> Infected
  make particle
  X make sfx

Infected -> Healthy
  make sfx
  X make particle

**Asset List**

 - Front Bus
 - Bus Car
 - Samsquantch (animated)
 - Samsquantch Walk/Scitter animation
 - Steve Samsquantch
 - Healthy Tree 1
 - Healthy Tree 2
 - Healthy Tree 3
 - Sick Tree 1
 - Sick Tree 2
 - Sick Tree 3
 - Poison particle

**Sound List**
- Main Theme
- Win/lose
- Infect/Tree Morph
- Disinfect in progress
- Disinfect complete

# Nice to have
Window Icon
  - need small + big, bring in stb to engine for loading

Script intro cut scene in GameplayManager

Particle color transition, or at least fade out

Particle alpha blending?
  overlap of transparent particle textures is wonk

Main Menu Scene

ImGui font

UI styling (just using engine defaults now)


**Asset List**

 - Miscellaneous filler detail objects
 - Worm Close up
 - Worm Scoot animation
  
**Sound List**
- Bus Engine
- Assorted Collision (requires a several sounds + velocity checking so it isn't obnoxious every time you touch anything)

# Future Work
Concave colliders initially interact with other static colliders
  we just put an `if static -> continue` check at the top of BspTree::FindPairs

Editor->AddEntity with layer/flag options

Color utilities, espectially for setting point light props

PhysicsBody::useGravity checkbox in editor broken
default camera has funny aspect ratio problem?
Update usage of Registry to EcsInterface

# Done
Dialog fwd/back buttons - might be annoying to have prev dialog removed on event

Animated dialog, should be fairly simple to 'type' out one character/word at time

Trigger win/lose condition
  maybe remove movement controller and just pop a window (with Win/Lose & NewGame/Quit) for mvp

VSC++ runtime/static link

Split tree and character creation into two steps
  tree: base trees will be created from editor, with logic added after the fact
  vehicle: probably want to create the camera separately for the intro, and create
           the vehicle and update camera target after intro

Infection particle spread effect

Menu 'New Game' option

Add character controller parts

Menu with resume, restart, and quit options

Tree tracking (how many infected/healthy)

update do not serialize layer to be a flag

won't fix: Enable SkeletalAnimator

NcGraphics Registry fwd decl

improve ParticleEmitter widget
  will need set funcs
do we have default particle? ParticleInfo should use it

Terrain

Ground
