#include "Gardien.h"
#include "math.h"
#include <cstdio>

void Gardien::update (void){
	if(hp > 0){ // ne peut agir que s'il est vivant
		voitChasseur();
		// teste si touché par boule de feu
		FireBall* fb;
		for(int g = 0; g < _l -> _nguards; g++){
			fb = _l -> _guards[g] -> _fb;
			if((int)((fb -> get_x()) / Environnement::scale) == (int)(_x / Environnement::scale) && 
				(int)((fb -> get_x()) / Environnement::scale) == (int)(_x / Environnement::scale)){
					hp--;
					if(hp > 0){
						//tomber();
					}else{
						//rester_au_sol();
					}
				}
		}
		// déplacement aléatoire
		double dx = -sin(nextAngle * M_PI /180.0) * 1;
		double dy = cos(nextAngle * M_PI /180.0) * 1;
		if(move(dx, dy)) _angle = nextAngle;
		else nextAngle = (rand() / (double) RAND_MAX) * 360;
	}
};

bool Gardien::move (double dx, double dy){
	int curX = (int)_x / Environnement::scale;
	int curY = (int)_y / Environnement::scale;
	int destX = (int)((_x + dx) / Environnement::scale);
	int destY = (int)((_y + dy) / Environnement::scale);
	if ((destX == curX && destY == curY) //S'il reste sur la même case, pas besoin de vérifier
		|| (EMPTY == _l -> data (destX, destY) && !occupe(destX, destY))) //S'il change de case, on vérifie si elle est vide
	{
		_x += dx;
		_y += dy;
		return true;
	}
	else return false;
}

bool Gardien::occupe(int x, int y){
	bool o = false;
	int g = 0;
	while(!o && g < _l -> _nguards){
		o = ((int) _l -> _guards[g] -> _x / Environnement::scale == x && (int) _l -> _guards[g] -> _y / Environnement::scale == y);
		g++;
	}
	return o;
}

bool Gardien::voitChasseur()
{
	float chasseurX =  (float)_l->_guards[0]->_x;
	float chasseurY =  (float)_l->_guards[0]->_y;
	float gardienX = (float)_x;
	float gardienY = (float)_y;

	if(gardienX == chasseurX && gardienY == chasseurY)
	{
		message("Vu");
		return true;
	}
	else
	{
		float sqDistanceChasseurGardien = sqDistance(gardienX, chasseurX, gardienY, chasseurY);
		float angle = atan2f(chasseurY - gardienY, chasseurX - gardienX);
		float sqDist = 0;
		float curX = 0, curY = 0;
		while(sqDist < sqDistanceChasseurGardien){// On progresse petit à petit jusqu'à rencontrer un obstacle ou dépasser le gardien
			curX += cos(angle);
			curY += sin(angle);
			int labX = (gardienX + curX) / Environnement::scale;
			int labY = (gardienY + curY) / Environnement::scale;
			//printf("LabX : %d, LabY : %d\n", labX, labY);
			if(_l->data(labX, labY)) // Si on rencontre un mur, une boite...
			{
				message("");
				return false;
			}
			else
			{
				sqDist = sqDistance(0, curX, 0, curY);
				//printf("Distance CurXY : %f, Distance Chasseur Gardien : %f\n", sqrt(sqDist), sqrt(sqDistanceChasseurGardien));
			}
		}

		// On a rencontré aucun obstacle
		message("Vu");
		return true;
		
	}
	


	
	
	return rand()%2;
}


float sqDistance(float x1, float x2, float y1, float y2)
{
	return powf(x2 - x1, 2) + powf(y2 - y1, 2);
}