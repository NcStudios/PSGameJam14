# Fixes

Intro zoomed in on steve
<!-- Fix dave's outro pos -->
<!-- Fix Collider Pos -->
<!-- Fix Putters pos -->
<!-- Scale down Firepit -->

Update dead sasquatch pos/model

<!-- Check lights agains -->

Fix Terrain Colliders
Camera Clip plane
<!-- Elder not sitting on anything -->
Cut down config maxTransforms, PointLights, animations, and textures to actual upper bound values
Multiply-Add in shaders?
Remove 'resume' from end game menu

<!-- X Fix BoxCar positioning - still way too high up? -->


# Must Do

Infected tree texture
  - Spreader trees don't start with correct texture on Event::StartSpread
    - set in editor once texture is in (since we're using a default asset for now)

Make Steve bigger

Fix Vehicle box car mesh positioning

Intro cutscene zoomed in on steve

Finalize Cutscene focus points + positioning
Fix Dave's final position

Add level detail + reduce redundancy

Finalize SFX
  - probably need one to signal all trees are cleared, it isn't that obvious

Music
  - X intro theme
  - spread theme
  - win
  - lose

Fix collision boxes and getting on top of terrain

Test audio on other devices, bumping buffer to 512 fixed my machine - may just be debug build issue

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
 - X Poison particle

**Sound List**
- X Main Theme
- Win/lose
- Infect/Tree Morph
- X Disinfect in progress
- X Disinfect complete

# Nice to have
environment particles

skybox

Jump no longer really useful, could be a speed boost

Window Icon
  - need small + big, bring in stb to engine for loading

Script intro cut scene in GameplayManager

Update vehicle movement to be even more worm-like

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
Dialog
  - X Display Controls dialog
  - Finish Win/Lose dialog/sequence

Healthy -> Infected
  X make sfx
  X make particle

Infected -> Healthy
  X make sfx
  X make particle

Shadow wonkiness

Purify
  X update to only emit when key held, shape is probably a long box instead
  X make sfx
  X make particle

X Set Dave's wave after intro cutscene
X Make Dave's quest required

X Bug on Surface - crash in igvk64.dll when running installed build against MSVC 17.5.0. Fixed by updating to MSVC 17.8.5.0.
Frame rate drop investigations?

X Add border asset - maybe can spawn something?

Quest Triggers
  - X meet dave, trigger dialog to go to camp
  - X arrive at camp
    X start spreaders at this point
  - X update indicator to use a texture or something
  - X escape to break convo, should disable UI popping menu temporarily

Fix moving static, but non-kinematic physics bodies in engine (or just don't have PB?)

Improve movement/camera
  not perfect, but in a 'tweakable' state
  X improve general feel and steering control
  make sure follow distance looks good (requires rough to-scale assets for testing)
  X jump isn't infinite

Add some interaction between vehicle and infected trees
  X update collision volume (maybe projetile?) and particle position/velocity
  X controls + purifier spawning added

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
