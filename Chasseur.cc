#include "Chasseur.h"
#include "Gardien.h"
#include "Labyrinthe.h"

/*
 * permet de tester si une case est occupée par un des gardiens
 */

int Chasseur::collisionGuards(double dx, double dy){
	for(int g = 0; g < _l -> _nguards; g++){
		if(_l -> _guards[g] != this &&
			_l -> _guards[g] -> _x > dx - Environnement::scale && _l -> _guards[g] -> _x < dx + Environnement::scale &&
			_l -> _guards[g] -> _y > dy - Environnement::scale && _l -> _guards[g] -> _y < dy + Environnement::scale){
			if(((Gardien*)_l -> _guards[g]) -> hp > 0){
				return g;
			}
		}
	}
	return -1;
}

/*
 *	Tente un deplacement.
 */

bool Chasseur::move_aux (double dx, double dy)
{
	message("Health : %d", hp);
	//message("Distance du tresor : %d Distance max : %d", ((Labyrinthe*)_l)->distance(_x / Environnement::scale, _y / Environnement::scale), ((Labyrinthe*)_l)->dist_max());
	//si on touche le trésor, la partie est gagnée
	if ((int)((_x + dx) / Environnement::scale) == _l -> _treasor._x &&
		(int)((_y + dy) / Environnement::scale) == _l -> _treasor._y)
	{
		partie_terminee (true);
	}

	if(hp > 0){ // ne peut agir que s'il est vivant
		// tester si la place est libre
		if (EMPTY == _l -> data ((int)((_x + dx) / Environnement::scale),
								(int)((_y + dy) / Environnement::scale))
			&& collisionGuards(_x + dx, _y + dy) == -1){
			_x += dx;
			_y += dy;
			return true;
		}

	}
	return false;
}

/*
 *	Constructeur.
 */

Chasseur::Chasseur (Labyrinthe* l) : Mover (100, 80, l, 0)
{
	hp = CHASSEUR_HP;

	_hunter_fire = new Sound ("sons/hunter_fire.wav");
	_hunter_hit = new Sound ("sons/hunter_hit.wav");
	if (_wall_hit == 0)
		_wall_hit = new Sound ("sons/hit_wall.wav");
}

/*
 *	Fait bouger la boule de feu
 */

bool Chasseur::process_fireball (float dx, float dy)
{
	// calculer la distance entre le chasseur et le lieu de l'explosion.
	float	x = (_x - _fb -> get_x ()) / Environnement::scale;
	float	y = (_y - _fb -> get_y ()) / Environnement::scale;
	float	dist2 = x*x + y*y;
	// on bouge que dans le vide!
	if (EMPTY == _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),
							 (int)((_fb -> get_y () + dy) / Environnement::scale)))
	{
		int g = collisionGuards(_fb -> get_x () + dx, _fb -> get_y () + dy);
		if(g > 0){
			((Gardien*) _l -> _guards[g]) -> touche();
		}else{
			//message ("Woooshh ..... %d", (int) dist2);
			// il y a la place.
			return true;
		}
	}
	// collision...
	// calculer la distance maximum en ligne droite.
	float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
	// faire exploser la boule de feu avec un bruit fonction de la distance.
	_wall_hit -> play (1. - dist2/dmax2);
	//message ("Booom...");
	// teste si on a touch� le tr�sor: juste pour montrer un exemple de la
	// fonction � partie_terminee �.
	/*
	if ((int)((_fb -> get_x () + dx) / Environnement::scale) == _l -> _treasor._x &&
		(int)((_fb -> get_y () + dy) / Environnement::scale) == _l -> _treasor._y)
	{
		partie_terminee (true);
	}
	*/
	return false;
}

/*
 *	Tire sur un ennemi.
 */

void Chasseur::fire (int angle_vertical)
{
	//message ("Woooshh...");
	_hunter_fire -> play ();
	_fb -> init (/* position initiale de la boule */ _x, _y, 10.,
				 /* angles de vis�e */ angle_vertical, _angle);
}

/*
 *	Clic droit: par d�faut fait tomber le premier gardien.
 *
 *	Inutile dans le vrai jeu, mais c'est juste pour montrer
 *	une utilisation des fonctions � tomber � et � rester_au_sol �
 */

void Chasseur::right_click (bool shift, bool control) {
	/*
	if (shift)
		_l -> _guards [1] -> rester_au_sol ();
	else
		_l -> _guards [1] -> tomber ();
	*/
}

void Chasseur::touche(){
	if(hp > 0){ // ne peut pas mourir plus
		hp--;
		_hunter_hit -> play(1, 0.5); // son de douleur
	message("Health : %d", hp);
		if(hp == 0) partie_terminee(false); // mort
	}
}