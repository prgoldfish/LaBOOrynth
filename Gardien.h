#ifndef GARDIEN_H
#define GARDIEN_H

#include "Mover.h"

#include <time.h>
#include <stdlib.h>

//points de santé par défaut
#define GARDIEN_HP	5

class Labyrinthe;

class Gardien : public Mover {
private:
	// prochain angle de déplacement
	int nextAngle;
	// permet de tester les collisions avec les gardiens
	int collisionGuards(double x, double y);
public:
	//points de santé
	int hp;

	Gardien (Labyrinthe* l, const char* modele) : Mover (120, 80, l, modele)
	{
		hp = GARDIEN_HP;
		srand(time(0));
		nextAngle = (rand() / (double) RAND_MAX) * 360;
	}

	// le gardien tente de se déplacer
	void update (void);
	// déplacement
	bool move (double dx, double dy);

	bool voitChasseur();

	// ne sait pas tirer sur un ennemi.
	void fire (int angle_vertical) {}
	// quand a faire bouger la boule de feu...
	bool process_fireball (float dx, float dy) { return false; }
	
};

float sqDistance(float x1, float x2, float y1, float y2);

#endif
