#include "Gardien.h"
#include "math.h"

void Gardien::update (void){
	double dx = -sin(nextAngle * M_PI /180.0) * 1;
	double dy = cos(nextAngle * M_PI /180.0) * 1;
	if(move(dx, dy)) _angle = nextAngle;
	else nextAngle = (rand() / (double) RAND_MAX) * 360;
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