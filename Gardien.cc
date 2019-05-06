#include "Gardien.h"
#include "Chasseur.h"
#include "Labyrinthe.h"
#include "math.h"
#include <cstdio>

void Gardien::update (void) // Met à jour le gardien
{
	if(hp > 0){ // Ne peut agir que s'il est vivant
		//On diminue le cooldown
		if(coolDown > 0) coolDown--;
		if(voitChasseur()) // Si le gardien voit le chasseur
		{
			// Le gardien se tourne vers le chasseur
			nextAngle = -atan2f(_l -> _guards[0] -> _x - _x, _l -> _guards[0] -> _y - _y) * 180.0 / M_PI;
			_angle = nextAngle;
			// Il tente d'attaquer le chasseur
			fire(0);
			// Il passe en mode attaque
			defense = false;
			// Il se souvient d'avir vu le chasseur
			vu = true;
		}
		else
		{
			if(!vu)
			{
				defense = (defTotale() < ((Labyrinthe*)_l)->dist_max() / (_l -> _nguards / 2.)); // On détermine si le gardien passe en mode défense ou attaque
			}
			if(defense)
			{
				// Il se dirige vers le trésor
				int intX = _x / Environnement::scale;
				int intY = _y / Environnement::scale;
				double dx = 0;
				double dy = 0;

				// Détermination de quelle direction rapproche le plus le gardien du trésor
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
				move(dx, dy) || move(dx, 0.0) || move(0.0, dy); // On bouge dans cette direction
				nextAngle = -atan2f(dx, dy) * 180.0 / M_PI; // Détermination de l'orientation du gardien
				_angle = nextAngle;
			}
			else // mode attaque
			{
				// Avance en ligne droite jusqu'au mur
				double dx = -sin(nextAngle * M_PI /180.0) * 1;
				double dy = cos(nextAngle * M_PI /180.0) * 1;
				if(move(dx, dy)) _angle = nextAngle;
				else // S'il atteint un mur
				{
					nextAngle = (rand() / (double) RAND_MAX) * 360; // On change de sens au hasard
					vu = false; // Le chasseur est perdu de vue
				}
			}
		}
	}
};

bool Gardien::move (double dx, double dy) // Fait bouger le gardien
{
	// Il bouge seulement s'il ne rentre pas dans un mur ou dans un autre gardien
	if (EMPTY == _l -> data ((int)((_x + dx) / Environnement::scale),
							 (int)((_y + dy) / Environnement::scale))
		&& collisionGuards(_x + dx, _y + dy) == -1)
	{
		_x += dx;
		_y += dy;
		return true;
	}
	return false;
}

int Gardien::collisionGuards(double dx, double dy) // Indique s'il rentre en collision avec un autre gardien ou le chasseur
{
	for(int g = 0; g < _l -> _nguards; g++)
	{
		if(_l -> _guards[g] != this &&
			_l -> _guards[g] -> _x > dx - Environnement::scale && _l -> _guards[g] -> _x < dx + Environnement::scale &&
			_l -> _guards[g] -> _y > dy - Environnement::scale && _l -> _guards[g] -> _y < dy + Environnement::scale) // Si le déplacement met les 2 gardiens sur la même case
		{
			if(g == 0 || ((Gardien*)_l -> _guards[g]) -> hp > 0) // Et que le second gardien est encore vivant
			{
				return g;
			}
		}
	}
	return -1;
}

bool Gardien::voitChasseur() // Indique si le gardien voit le chasseur
{
	float chasseurX =  (float)_l->_guards[0]->_x;
	float chasseurY =  (float)_l->_guards[0]->_y;
	float gardienX = (float)_x;
	float gardienY = (float)_y;

	if(gardienX == chasseurX && gardienY == chasseurY) // S'il sont au même endroit
	{
		return true;
	}
	else // On lance un rayon imaginaire du gardien vers le chasseur. 
	{
		float sqDistanceChasseurGardien = sqDistance(gardienX, chasseurX, gardienY, chasseurY);
		float angle = atan2f(chasseurY - gardienY, chasseurX - gardienX);
		float sqDist = 0;
		float curX = 0, curY = 0;
		while(sqDist < sqDistanceChasseurGardien) // On progresse petit à petit jusqu'à rencontrer un obstacle ou dépasser le gardien
		{
			curX += cos(angle) * Environnement::scale;
			curY += sin(angle) * Environnement::scale;
			int labX = (gardienX + curX) / Environnement::scale;
			int labY = (gardienY + curY) / Environnement::scale;
			if(_l->data(labX, labY)) // Si on rencontre un mur, une boite...
			{
				return false;
			}
			else
			{
				sqDist = sqDistance(0, curX, 0, curY);
			}
		}

		// On a rencontré aucun obstacle
		return true;
		
	}
}


// Calcul du potentiel de defense du gardien
double Gardien::pdef()
{
	if(hp > 0)
		return ((Labyrinthe*)_l)->dist_max() / ((Labyrinthe*)_l)->distance(_x / Environnement::scale, _y / Environnement::scale); // La distance maximum divisée par sa propre distance au trésor
	else 
		return 0;
}

// Calcul du potentiel de defense de tous les gardiens
double Gardien::defTotale()
{
	double dt = 0;
	for(int g = 1; g < _l -> _nguards; g++){
		dt += ((Gardien*)_l -> _guards[g]) -> pdef();
	}
	return dt;
}

// Tente de tirer sur un ennemi.
void Gardien::fire(int angle_vertical) 
{
	if(coolDown == 0)
	{
		// Empêche de tirer une dexième fois
		coolDown = -1;
		_fb -> init (/* position initiale de la boule */ _x, _y, 10.,
					/* angles de visée */ angle_vertical, -_angle);
	}
}


bool Gardien::process_fireball (float dx, float dy) // Met à jour la boule de feu
{
	// On ne bouge que dans le vide!
	if (EMPTY == _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),
							 (int)((_fb -> get_y () + dy) / Environnement::scale)))
	{
		int g = collisionGuards(_fb -> get_x () + dx, _fb -> get_y () + dy); // On vérifie si ça touche un autre gardien ou chasseur
		if(g == 0) // Ne fait des dégats qu'au chasseur
		{
			((Chasseur*) _l -> _guards[g]) -> touche();
		}
		else
		{
			return true;
		}
	}
	coolDown = GARDIEN_COOLDOWN;
	return false;
}

void Gardien::touche() // Si le gardien se fait toucher
{
	if(hp > 0){ // Ne peut pas mourir plus
		hp--;
		if(hp > 0) tomber(); // Blessé
		else rester_au_sol(); // Mort
	}
}

float sqDistance(float x1, float x2, float y1, float y2) // Calcule le carré de la distance entre deux points
{
	return powf(x2 - x1, 2) + powf(y2 - y1, 2);
}