/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: Somewhat of a demo application
Note:    Some of the code is based on GLFW 2.6 (See Readme.txt)
-----------------------------------------

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.
===========================================================================
*/

/*
==============================================================================

  ogEntity

==============================================================================
*/
class ogEntity {
public:
	ogEntity();
	virtual ~ogEntity() {}

	void			Spawn( void );

	virtual void	Think( float timeScale );

	virtual void	Draw( void );
	virtual void	DrawSelf( float x, float y );
	void			DrawImage( og::Image *img, float scale );
	void			CheckCollisions( og::Vec2 pos );
	virtual void	Collide( ogEntity *ent ) {}
	virtual void	Explode( void );

public: // fixme: protected?
	og::Vec2		origin;
	og::Vec2		velocity;
	float		radius;
	float		rotation;
	float		rotationSpeed;

	bool		doExplode;
	og::Image *	image;
	og::Vec2		imageVerts[2];
	og::Vec2		portalOffset;
	float		drawRadius;

	int			entityNumber;
	int			spawnId;
	float		popTime, popTimeFull;

	og::String	name;
	og::Dict		settings;

	DECLARE_ENTITY_TYPE()
};

/*
==============================================================================

  ogPlayer

==============================================================================
*/
class ogPlayer : public ogEntity {
public:
	void	Spawn( void );
	void	Collide( ogEntity *ent );
	void	Think( float timeScale );
	void	Draw( void );
	void	DrawSelf( float x, float y );
	void	Explode( void );
	void	Reset( void );
	void	Attack( void );
	bool	IsProtected( void ) { return spawnProtection > 0.0f; }

	float	acceleration;
	float	maxSpeed;
	int		lifes;
	float	spawnProtection;
	float	spawnProtectionFade;
	og::String blastClass;

	og::Image *	imageShield;
	og::Image *	imageThrust;
	og::Image *	imageReverse;
	og::Image *	imageLeft;
	og::Image *	imageRight;
	DECLARE_ENTITY_TYPE()
};

/*
==============================================================================

  ogBlast

==============================================================================
*/
class ogBlast : public ogEntity {
public:
	void	Spawn( void );
	void	Collide( ogEntity *ent );
	void	Think( float timeScale );

	float	timeLeft;
	
	DECLARE_ENTITY_TYPE()
};

/*
==============================================================================

  ogAsteroid

==============================================================================
*/
class ogAsteroid : public ogEntity {
public:
	void	Collide( ogEntity *ent );
	
	DECLARE_ENTITY_TYPE()
};

/*
==============================================================================

  ogSplitter

==============================================================================
*/
class ogSplitter : public ogEntity {
public:
	void	Spawn( void );
	void	Think( float timeScale );
	void	Draw( void );

	float	timeLeft, lifeTime;

	DECLARE_ENTITY_TYPE()
};

/*
==============================================================================

  ogItem

==============================================================================
*/
class ogItem : public ogEntity {
public:
	void	Spawn( void );
	void	Collide( ogEntity *ent );
	void	Think( float timeScale );

	float	timeLeft;
	int		extraLifes;

	DECLARE_ENTITY_TYPE()
};

void SpawnAsteroidsRadial( const char *classname, og::Vec2 origin, int count, float minDist, float maxDist, float minSpeed, float maxspeed, float maxRot, float spreadPct );
void SpawnAsteroidsRandom( const char *classname, og::Vec2 origin, int count, float minDist, float maxDist, float minSpeed, float maxspeed, float maxRot, float spreadPct );
