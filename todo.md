
# Must Do
Test audio on other devices, bumping buffer to 512 fixed my machine - may just be debug build issue

Shadow wonkiness

Trigger win/lose condition
  maybe remove movement controller and just pop a window (with Win/Lose & NewGame/Quit) for mvp

Add some interaction between vehicle and infected trees
  controls + purifier spawning added, still need to change collision mechanic to time-based

Improve movement/camera
  improve general feel and steering control
    - tough stuff without proper vehicle physics; kind of interesting to use capsule colliders for back cars.
      need to restrict PB freedom though. this gives a sort of springy effect that's pretty worm-like, but
      would need more fiddling to make fun rather than annoying
  make sure follow distance looks good (requires rough to-scale assets for testing)
  X jump isn't infinite
  X max velocity

Keep vehicle upright, or force back somehow
  - can we restrict PhysicsBody freedom?
    - is in world space, so must do x and z... its not the worst looking. could use as fallback, or maybe set to like 0.1f?

Visual representation of healthy/infected tree count (UI, but...) [not sure if 'must' or 'nice to have']


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
 - Ground
 - Poison particle

**Sound List**
- Main Theme
- Bus Engine
- Assorted Collision
- Infect/Tree Morph
- Disinfect in progress
- Disinfect complete

# Nice to have
Window Icon
  - need small + big, bring in stb to engine for loading

Script intro cut scene in GameplayManager

Dialog fwd/back buttons - might be annoying to have prev dialog removed on event

Particle alpha blending?
  overlap of transparent particle textures is wonk

Main Menu Scene

ImGui font

UI styling (just using engine defaults now)


**Asset List**

 - Miscellaneous filler detail objects
 - Worm Close up
 - Worm Scoot animation
  

# Future Work
Editor->AddEntity with layer/flag options

Color utilities, espectially for setting point light props

PhysicsBody::useGravity checkbox in editor broken
default camera has funny aspect ratio problem?
Update usage of Registry to EcsInterface

# Done
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

