#include "Gardien.h"
#include "math.h"

void Gardien::update (void){
	double dx = -sin(nextAngle * M_PI /180.0) * 1;
	double dy = cos(nextAngle * M_PI /180.0) * 1;
	if(move(dx, dy)) _angle = nextAngle;
	else nextAngle = (rand() / (double) RAND_MAX) * 360;
};

bool Gardien::move (double dx, double dy){
	int destX = (int)((_x + dx) / Environnement::scale);
	int destY = (int)((_y + dy) / Environnement::scale);
	if ((destX == (int)_x/Environnement::scale && destY == (int) _y/Environnement::scale) //S'il reste sur la même case, pas besoin de vérifier
		|| EMPTY == _l -> data (destX, destY)) //S'il change de case, on vérifie si elle est vide
	{
		_x += dx;
		_y += dy;
		return true;
	}
	return false;
}