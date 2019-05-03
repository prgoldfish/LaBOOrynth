#ifndef GARDIEN_H
#define GARDIEN_H

#include "Mover.h"

#include <time.h>
#include <stdlib.h>

//points de santé par défaut
#define GARDIEN_HP	5
//cooldown entre deux boules de feu
#define GARDIEN_COOLDOWN	100

class Labyrinthe;

class Gardien : public Mover {
private:
	// détermine si le gardien est en mode défense
	bool defense;
	// prochain angle de déplacement
	int nextAngle;
	// Chasseur vu récemment, continuer de le suivre si vrai
	bool vu;
	// empêche le gardien de tirer
	int coolDown;
	// permet de tester les collisions avec les gardiens
	int collisionGuards(double x, double y);
	// potentiel de defense de tous les gardiens
	double defTotale();
public:

	Gardien (Labyrinthe* l, const char* modele) : Mover (120, 80, l, modele)
	{
		hp = GARDIEN_HP;
		defense = false;
		vu = false;
		srand(time(0));
		nextAngle = (rand() / (double) RAND_MAX) * 360;
		coolDown = GARDIEN_COOLDOWN;
	}

	// le gardien tente de se déplacer
	void update (void);
	// déplacement
	bool move (double dx, double dy);
	// détermine s'il peut voir le chasseur
	bool voitChasseur();
	// potentiel de defense du gardien
	double pdef();

	// tente de tirer sur un ennemi.
	void fire (int angle_vertical);
	// fait bouger la boule de feu
	bool process_fireball (float dx, float dy);
	
	//points de santé
	int hp;
	// touché par une attaque
	void touche();
};

float sqDistance(float x1, float x2, float y1, float y2);

#endif
