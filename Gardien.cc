#include "Gardien.h"
#include "Chasseur.h"
#include "Labyrinthe.h"
#include "math.h"
#include <cstdio>

void Gardien::update (void){
	if(hp > 0){ // ne peut agir que s'il est vivant
		if(voitChasseur()){
			// fait face au chasseur
			nextAngle = -atan2f(_l -> _guards[0] -> _x - _x, _l -> _guards[0] -> _y - _y) * 180.0 / M_PI;
			_angle = nextAngle;
			// tente d'attaquer le chasseur
			fire(0);
			// passe en mode attaque
			defense = false;
		}else{
			int intX = _x / Environnement::scale;
			int intY = _y / Environnement::scale;
			if(defense){
				//suivre le chemin vers le trésor
				double dx = 0;
				double dy = 0;
				if(EMPTY == _l -> data (intX + 1, intY) && ((Labyrinthe*)_l)->distance(intX + 1, intY) < ((Labyrinthe*)_l)->distance(intX, intY)){
					dx = 1;
				}
				if(EMPTY == _l -> data (intX - 1, intY) && ((Labyrinthe*)_l)->distance(intX - 1, intY) < ((Labyrinthe*)_l)->distance(intX, intY)){
					dx = -1;
				}
				if(EMPTY == _l -> data (intX, intY + 1) && ((Labyrinthe*)_l)->distance(intX, intY + 1) < ((Labyrinthe*)_l)->distance(intX, intY)){
					dy = 1;
				}
				if(EMPTY == _l -> data (intX, intY - 1) && ((Labyrinthe*)_l)->distance(intX, intY - 1) < ((Labyrinthe*)_l)->distance(intX, intY)){
					dy = -1;
				}
				move(dx, dy) || move(dx, 0.0) || move(0.0, dy);
				nextAngle = -atan2f(dx, dy) * 180.0 / M_PI;
				_angle = nextAngle;
			}else{
				// déplacement aléatoire
				double dx = -sin(nextAngle * M_PI /180.0) * 1;
				double dy = cos(nextAngle * M_PI /180.0) * 1;
				if(move(dx, dy)) _angle = nextAngle;
				else nextAngle = (rand() / (double) RAND_MAX) * 360;
			}
		}
	}
};

bool Gardien::move (double dx, double dy){
	if (EMPTY == _l -> data ((int)((_x + dx) / Environnement::scale),
							 (int)((_y + dy) / Environnement::scale))
		&& collisionGuards(_x + dx, _y + dy) == -1){
		_x += dx;
		_y += dy;
		return true;
	}
	return false;
}

int Gardien::collisionGuards(double dx, double dy){
	for(int g = 0; g < _l -> _nguards; g++){
		if(_l -> _guards[g] != this &&
			_l -> _guards[g] -> _x > dx - Environnement::scale && _l -> _guards[g] -> _x < dx + Environnement::scale &&
			_l -> _guards[g] -> _y > dy - Environnement::scale && _l -> _guards[g] -> _y < dy + Environnement::scale){
			return g;
		}
	}
	return -1;
}

bool Gardien::voitChasseur()
{
	float chasseurX =  (float)_l->_guards[0]->_x;
	float chasseurY =  (float)_l->_guards[0]->_y;
	float gardienX = (float)_x;
	float gardienY = (float)_y;

	//message("X : %d, Y : %d", (int)chasseurX / Environnement::scale, (int)chasseurY / Environnement::scale);
	if(gardienX == chasseurX && gardienY == chasseurY)
	{
		//message("Vu");
		return true;
	}
	else
	{
		float sqDistanceChasseurGardien = sqDistance(gardienX, chasseurX, gardienY, chasseurY);
		float angle = atan2f(chasseurY - gardienY, chasseurX - gardienX);
		float sqDist = 0;
		float curX = 0, curY = 0;
		while(sqDist < sqDistanceChasseurGardien){// On progresse petit à petit jusqu'à rencontrer un obstacle ou dépasser le gardien
			curX += cos(angle) * Environnement::scale;
			curY += sin(angle) * Environnement::scale;
			int labX = (gardienX + curX) / Environnement::scale;
			int labY = (gardienY + curY) / Environnement::scale;
			//printf("LabX : %d, LabY : %d\n", labX, labY);
			if(_l->data(labX, labY)) // Si on rencontre un mur, une boite...
			{
				//message("");
				return false;
			}
			else
			{
				sqDist = sqDistance(0, curX, 0, curY);
				//printf("Distance CurXY : %f, Distance Chasseur Gardien : %f\n", sqrt(sqDist), sqrt(sqDistanceChasseurGardien));
			}
		}

		// On a rencontré aucun obstacle
		//message("Vu");
		return true;
		
	}
}

// tente de tirer sur un ennemi.
void Gardien::fire (int angle_vertical) 
{
	if(coolDown > 0) coolDown --;
	if(coolDown == 0){
		// empêche de tirer une dexième fois
		coolDown = -1;
		//message ("Woooshh...");
		//_hunter_fire -> play ();
		_fb -> init (/* position initiale de la boule */ _x, _y, 10.,
					/* angles de vis�e */ angle_vertical, -_angle);
	}
}


bool Gardien::process_fireball (float dx, float dy)
{
	// calculer la distance entre le chasseur et le lieu de l'explosion.
	//float	x = (_x - _fb -> get_x ()) / Environnement::scale;
	//float	y = (_y - _fb -> get_y ()) / Environnement::scale;
	//float	dist2 = x*x + y*y;
	// on bouge que dans le vide!
	if (EMPTY == _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),
							 (int)((_fb -> get_y () + dy) / Environnement::scale)))
	{
		int g = collisionGuards(_fb -> get_x () + dx, _fb -> get_y () + dy);
		if(g == 0){
			((Chasseur*) _l -> _guards[g]) -> touche();
		}else{
			//message ("Woooshh ..... %d", (int) dist2);
			// il y a la place.
			return true;
		}
	}
	// collision...
	// calculer la distance maximum en ligne droite.
	//float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
	// faire exploser la boule de feu avec un bruit fonction de la distance.
	//_wall_hit -> play (1. - dist2/dmax2);
	//message ("Booom...");
	// peut tirer de nouveau
	coolDown = GARDIEN_COOLDOWN;
	return false;
}

void Gardien::touche(){
	if(hp > 0){ // ne peut pas mourir plus
		hp--;
		if(hp > 0) tomber(); // blessé
		else rester_au_sol(); // mort
	}
}

float sqDistance(float x1, float x2, float y1, float y2)
{
	return powf(x2 - x1, 2) + powf(y2 - y1, 2);
}