#ifndef LABYRINTHE_H
#define LABYRINTHE_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <map>

#include "Environnement.h"

struct Corner {
	int	x, y;		// emplacement du coin de mur.
	bool hasRight, hasDown;		// Indique si le mur continue en bas et à droite.
};

class Labyrinthe : public Environnement {
private:
	char	                        **_data;	// indique si la case est libre ou occup�e.
	int		                        lab_width;	// dimensions du rectangle.
	int		                        lab_height;	// englobant le labyrinthe.
    std::vector<std::string>        getLines(char* file); 
    void                            eraseComments(std::vector<std::string> *lines);
    std::map<char, std::string>     getVars(std::vector<std::string> *lines);
    std::vector<std::vector<char>>  getLabData(std::vector<std::string> *lines);
    void                            createWalls(const std::vector<std::vector<char>> *labData);
    bool                            isWall(char c);
    std::vector<Corner>             getCornerList(const std::vector<std::vector<char>> *labData, int *nbWalls);
	void 							createSolVide(const std::vector<std::vector<char>> *labData);
	void 							createWallsHitbox();

public:
	Labyrinthe (char*);
	int width () { return lab_width;}	// retourne la largeur du labyrinthe.
	int height () { return lab_height;}	// retourne la longueur du labyrinthe.
	// retourne l'�tat (occupation) de la case (i, j).
	char data (int i, int j)
	{
		return _data [i][j];
	}
};

#endif
