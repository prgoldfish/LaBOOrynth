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
	//points de santé
	int hp;
	// détermine si le gardien est en mode défense
	bool defense;
	// prochain angle de déplacement
	int nextAngle;
	// empêche le gardien de tirer
	bool coolDown;
	// permet de tester les collisions avec les gardiens
	int collisionGuards(double x, double y);
public:

	Gardien (Labyrinthe* l, const char* modele) : Mover (120, 80, l, modele)
	{
		hp = GARDIEN_HP;
		defense = true;
		srand(time(0));
		nextAngle = (rand() / (double) RAND_MAX) * 360;
		coolDown = false;
	}

	// le gardien tente de se déplacer
	void update (void);
	// déplacement
	bool move (double dx, double dy);
	// détermine s'il peut voir le chasseur
	bool voitChasseur();

	// tente de tirer sur un ennemi.
	void fire (int angle_vertical);
	// fait bouger la boule de feu
	bool process_fireball (float dx, float dy);
	
	// touché par une attaque
	void touche();
};

float sqDistance(float x1, float x2, float y1, float y2);

#endif
