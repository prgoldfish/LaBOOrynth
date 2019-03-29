#ifndef GARDIEN_H
#define GARDIEN_H

#include "Mover.h"

#include <time.h>
#include <stdlib.h>

class Labyrinthe;

class Gardien : public Mover {
private:
	int nextAngle;
public:
	Gardien (Labyrinthe* l, const char* modele) : Mover (120, 80, l, modele)
	{
		srand(time(0));
		nextAngle = (rand() / (double) RAND_MAX) * 360;
	}

	// le gardien tente de se déplacer
	void update (void);
	// déplacement
	bool move (double dx, double dy);
	// ne sait pas tirer sur un ennemi.
	void fire (int angle_vertical) {}
	// quand a faire bouger la boule de feu...
	bool process_fireball (float dx, float dy) { return false; }
};

#endif
