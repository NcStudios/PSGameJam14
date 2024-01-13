
# Must Do

Visual representation of healthy/infected tree count (UI, but...)

Trigger win/lose condition

Add character controller parts


Improve movement/camera
  jump isn't infinite
  make sure follow distance looks good (requires rough to-scale assets for testing)

Keep vehicle upright, or force back somehow

Add some interaction between vehicle and infected trees
  like sit next to tree, hold button, wait for it to turn

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

  

# Nice to have
Particle alpha blending?
  overlap of transparent particle textures is wonk

Main Menu Scene

ImGui font

UI styling (just using engine defaults now)

Menu 'New Game' option


**Asset List**

 - Miscellaneous filler detail objects
 - Worm Close up
 - Worm Scoot animation
  

# Future Work

PhysicsBody::useGravity checkbox in editor broken
default camera has funny aspect ratio problem?
Update usage of Registry to EcsInterface

# Done
Menu with resume, restart, and quit options

Tree tracking (how many infected/healthy)

update do not serialize layer to be a flag

won't fix: Enable SkeletalAnimator

NcGraphics Registry fwd decl

improve ParticleEmitter widget
  will need set funcs
do we have default particle? ParticleInfo should use it

