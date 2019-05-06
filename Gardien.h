#ifndef GARDIEN_H
#define GARDIEN_H

#include "Mover.h"

#include <time.h>
#include <stdlib.h>

// Points de santé par défaut
#define GARDIEN_HP	5
// Cooldown entre deux boules de feu
#define GARDIEN_COOLDOWN	100

class Labyrinthe;

class Gardien : public Mover {
private:
	// Détermine si le gardien est en mode défense
	bool defense;
	// Prochain angle de déplacement
	int nextAngle;
	// Chasseur vu récemment, continuer de le suivre si vrai
	bool vu;
	// Empêche le gardien de tirer
	int coolDown;
	// Permet de tester les collisions avec les gardiens
	int collisionGuards(double x, double y);
	// Potentiel de defense de tous les gardiens
	double defTotale();
public:

	Gardien (Labyrinthe* l, const char* modele) : Mover (120, 80, l, modele) // Création du gardien
	{
		hp = GARDIEN_HP;
		defense = false;
		vu = false;
		srand(time(0));
		nextAngle = (rand() / (double) RAND_MAX) * 360;
		coolDown = GARDIEN_COOLDOWN;
	}

	// Le gardien tente de se déplacer
	void update (void);
	// Déplacement
	bool move (double dx, double dy);
	// Détermine s'il peut voir le chasseur
	bool voitChasseur();
	// Potentiel de defense du gardien
	double pdef();

	// Tente de tirer sur un ennemi.
	void fire (int angle_vertical);
	// Fait bouger la boule de feu
	bool process_fireball (float dx, float dy);
	
	// Points de santé
	int hp;
	// Si touché par une attaque
	void touche();
};

// Distance carrée entre deux points
float sqDistance(float x1, float x2, float y1, float y2);

#endif
