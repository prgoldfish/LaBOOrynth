#ifndef CHASSEUR_H
#define CHASSEUR_H

#include <stdio.h>
#include "Mover.h"
#include "Sound.h"

#define CHASSEUR_HP	5

class Labyrinthe;

class Chasseur : public Mover {
private:
	//points de santé
	int hp;
	// accepte ou non un deplacement.
	bool move_aux (double dx, double dy);
	// permet de tester les collisions avec les gardiens
	int collisionGuards(double x, double y);
public:
	/*
	 *	Le son...
	 */
	static Sound*	_hunter_fire;	// bruit de l'arme du chasseur.
	static Sound*	_hunter_hit;	// cri du chasseur touch�.
	static Sound*	_wall_hit;		// on a tap� un mur.

	Chasseur (Labyrinthe* l);
	// ne bouger que dans une case vide (on 'glisse' le long des obstacles)
	bool move (double dx, double dy) {
		return move_aux (dx, dy) || move_aux (dx, 0.0) || move_aux (0.0, dy);
	}
	// le chasseur ne pense pas!
	void update (void) {};
	// fait bouger la boule de feu
	bool process_fireball (float dx, float dy);
	// tire sur un ennemi (clic gauche).
	void fire (int angle_vertical);
	// clic droit.
	void right_click (bool shift, bool control);
	
	// touché par une attaque
	void touche();
};

#endif
