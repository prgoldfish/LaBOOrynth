#ifndef LABYRINTHE_H
#define LABYRINTHE_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <dirent.h>

#include "Environnement.h"

struct Corner { // Décrit un coin de mur
	int	x, y;		// Emplacement du coin de mur.
	bool hasRight, hasDown;		// Indique si le mur continue en bas et à droite.
};

struct Affiche { // Décrit une affiche
	int x, y;	//Emplacement de l'affiche
	std::string imagePath;	// Image utilisée pour l'affiche
	bool orientation; 	// Indique le sens de l'affiche (vertical ou horizontal)
};



bool isAffiche(char c, std::map<char, std::string> *vars);
bool isWall(char c);
const char* randomGuard(std::vector<std::string> ennemis);
std::vector<std::string> initEnnemisList();

class Labyrinthe : public Environnement {
private:
	char	                        **_data;	// Indique si la case est libre ou occupee.
	int		                        **_dist;	// Indique la distance entre cette case et le trésor.
	int								_dist_max;	// Indique la plus grande distance au trésor dans le labyrinthe
	int		                        lab_width;	// Dimensions du rectangle
	int		                        lab_height;	// englobant le labyrinthe.
    std::vector<std::string>        getLines(char* file); 
    void                            eraseComments(std::vector<std::string> *lines);
    std::map<char, std::string>     getVars(std::vector<std::string> *lines);
    std::vector<std::vector<char>>  getLabData(std::vector<std::string> *lines);
    void                            createWalls(const std::vector<std::vector<char>> *labData);
    std::vector<Corner>             getCornerList(const std::vector<std::vector<char>> *labData);
	void 							createSolVide();
	void 							createWallsHitbox();
	void							createAffiche(const std::vector<std::vector<char>> *labData, std::map<char, std::string> *vars);
	void 							createBoites(const std::vector<std::vector<char>> *labData);
	void 							createTresor(const std::vector<std::vector<char>> *labData);
	void 							createMovers(const std::vector<std::vector<char>> *labData);
	void 							setTresorDistance();
	void 							setDistance(int x, int y, int value);

public:
	Labyrinthe (char*);
	int width () { return lab_width;}	// Retourne la largeur du labyrinthe.
	int height () { return lab_height;}	// Retourne la longueur du labyrinthe.
	int dist_max () { return _dist_max;} // Retourne la plus grande distance au trésor dans le labyrinthe
	
	// Retourne l'état d'occupation de la case (i, j).
	char data (int i, int j)
	{
		return _data [i][j];
	}

	// Retourne la distance au trésor à la case (i, j).
	int distance(int x, int y)
	{
		return _dist[x][y];
	}
};

#endif
