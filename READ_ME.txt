Assets:
- Entities in the game will be redered using various Textures and Animations
  which we will be calling Assets (along with Fonts)
- Assets are loaded once at the beginning of the porgram and stored in the 
  Assets class, which is stored by the c_Game_engine class
- All Assets are defined in the assets.txt, with the syntax defined below

--------------------------------------------------------------------------------
Assets File Specification
--------------------------------------------------------------------------------

There will be three different line types in the Assets file, each of which
correspond to a different type of Asset. They are as follows:

Texture Asset Specification:
Texture N P
  Texture Name	   	N	    std::string (it will have no spaces)
  Texture FilePath 	P	    std::string (it will have no spaces)

Animation Asset Specification:
Animation N T F S
  Animation Name 	N	std::string (it will have no spaces)
  Texture Name   	T	std::string (refers to an existing texture)
  Frame Count    	F	int (number of frames in the Animation)
  Anim Speed	 	S	int (number of game frames between anim frames)

Font Asset Specification:
Font N P
  Font Name	 	N	std::string (it will have no spaces)
  Font File Path 	P	std::string (it will have no spaces)

---------------------------------------------------------------------------------
Level Specification File
---------------------------------------------------------------------------------

Important Note:
  All (GX, GY) positions given in the level specification file are given in
  'grid' coordinates. The 'grid' cells are of size 64 by 64 pixels, and the
  entity should be positioned such that the bottom left corner of its texture
  is aligned with the bottom left corner of the given grid coordinates. The
  grid starts at (0,0) in the bottom left of the screen, and can be seen by
  pressing the 'G' key while the game is running

Tile Entity Specification:
Tile N GX GY
  Animation Name 	N 	std::string (Animation asset name for this tile)
  GX Grid X Pos 	GX 	float
  GY Grid Y Pos 	GY 	float

Decoration Entity Specification:
Dec N X Y
  Animation Name 	N 	std::string(Animation asset name for this tile)
  X Position 		X	float
  Y Position 		Y 	float

Player Specification
Player GX GY CW CH SX SY SM GY B
  GX, GY Grid Pos	X, Y	float, float (starting position of player)
  BoundingBox W/H	CW, CH	float, float
  Left/Right Speed	SX	float
  Jump Speed		SY	float
  Max Speed		SM	float
  Gravity		GY	float
  Bullet Animation 	B	std::string (Animation asset to use for bullets)

-----------------------------------------------------------------------------------
Project Approach
-----------------------------------------------------------------------------------

-  Rendering system:
   You can press the T key to toggle drawing textures
   You can press the C key to toggle drawing bounding boxes
   You can press the G key to toggle drawing the grid

-  You can implement Animation::update() and Animation::has_ended() at any
   time, it will not affect the gameplay mechanics whatsoever, just animation

-  Implement Scene_Play::load_level()
   Since rendering is already completed, once you correctly read in the 
   diffrent types of entities, add them to the Entity_Manager and they should
   automatically be drawn to the screen. Add the correct bounding boxes
   to Tile entities, and no bounding boxes to the Dec entities. Remember
   you can toggle debuf viewing of bounding boxes with the T and C keys

   As part of this step, implement the Scene_Play::grid_to_mid_pixel()
   function, which takes in as parameters a grid x,y position and an Entity,
   and returns the c_Vec2 position of the CENTER of that Entity. You must
   use the Animation size of the Entity to determine where its center point
   should be. Keep in mind that this means your Entity MUST have its
   c_Animation component added first, so that it can be used to calculate the
   midpoint in this function.

-  Implement Scene_Play::spawn_player()
   Read the player configuration from the level file and spawn the player
   This is where the player should restart when they die.

-  Implement some basic WASD u/l/d/r movement for the player entity so that
   you can use this to help test collisions in the future. Remember that 
   you must use register_action to register a new action for the scene. See
   the actions already registered for you, and the s_do_action() function for
   syntax on how to perform actions. (ALREADY IMPLEMENTED)

-  Implement Scene_Play::spawn_bullet()
   Bullet should shoot when Space is pressed in the same direction the player is 
   facing. Holding down the space button should not continuously fire bullets. A
   new bullet can only be fired after the space key has been released. Use
   the entity's c_Input.can_shoot variable to implement this

-  Implement Physics::get_overlap()
   This function should return the overlap dimensions between the bounding
   boxes of two enitities. This is the same as the purple rectangle in notes.
   get_previous_overlap() should be a copy/paste of this solution except using
   the previous positions instead of the current positions of the entity.
   If either input entity has no bounding box, then return c_Vec2(0,0)

-  Implement collision checking with bullets / brick tiles such that the brick
   is destroyed when a bullet collides with it. Remember, a collision occurs 
   when the overlap is non-zero in both the X and Y component. Bullets should
   always be destoryed when they collide with any non-decorative tile

-  Implement collision resolution such that when the player collides with a 
   non-decorative tile, the player cannot enter it or overlap it. When the 
   player collides with a tile from below, its y-velocity should be set to
   zero so that it falls back downward and doesn't 'hover' below the tile.
   
-  Implement a way of detecting which side the player collided with the tile

-  Change the controls such that they are the proper left/right/jump style
   Note: All movement logic should be in the movement system. The s_do_action()
   system is ONLY used to set the proper c_Input variables. Modifying the player's
   speed or position anywhere inside the s_do_action() system is considered unsafe

-  Implement gravity such that the player falls toward the bottom of the screen
   and lands on tiles when it collides with a tile from above. Note that when
   the player lands on a tile from above, you should set its vertical speed to
   zero so that gravity does not continue to accelerate the player downward