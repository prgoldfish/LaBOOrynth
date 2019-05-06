#ifndef CHASSEUR_H
#define CHASSEUR_H

#include <stdio.h>
#include "Mover.h"
#include "Sound.h"

#define CHASSEUR_HP	5

class Labyrinthe;

class Chasseur : public Mover {
	private:
		// Points de santé
		int hp;
		// Accepte ou non un deplacement.
		bool move_aux (double dx, double dy);
		// Permet de tester les collisions avec les gardiens
		int collisionGuards(double x, double y);
	
	public:
		/*
		*	Le son...
		*/
		static Sound*	_hunter_fire;	// Bruit de l'arme du chasseur.
		static Sound*	_hunter_hit;	// Cri du chasseur touché.
		static Sound*	_wall_hit;		// Quand on tape un mur.

		Chasseur (Labyrinthe* l);
		
		// Ne bouger que dans une case vide. On 'glisse' le long des obstacles
		bool move (double dx, double dy) {
			return move_aux (dx, dy) || move_aux (dx, 0.0) || move_aux (0.0, dy);
		}
		// Le chasseur ne pense pas!
		void update (void) {};
		// Fait bouger la boule de feu
		bool process_fireball (float dx, float dy);
		// Tire sur un ennemi (clic gauche).
		void fire (int angle_vertical);
		// Clic droit.
		void right_click (bool shift, bool control);
		
		// Quad on est touché par une attaque
		void touche();
};

#endif
