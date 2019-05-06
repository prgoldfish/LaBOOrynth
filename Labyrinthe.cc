#include "Labyrinthe.h"
#include "Chasseur.h"
#include "Gardien.h"

Sound*	Chasseur::_hunter_fire;
Sound*	Chasseur::_hunter_hit;
Sound*	Chasseur::_wall_hit;

Environnement* Environnement::init (char* filename)
{
	return new Labyrinthe (filename);
}

Labyrinthe::Labyrinthe (char* filename) // Charge le fichier filename et construit le labyrinthe avec ses données
{ 
	std::vector<std::string> lines = getLines(filename); // On lit le fichier
	eraseComments(&lines); // On supprime les commentaires
	std::map<char, std::string> vars = getVars(&lines); // Lit les variables du fichier
	std::vector<std::vector<char>> labData = getLabData(&lines); // Lit les données du labyrinthe
	createSolVide(); // Initialise à EMPTY l'occupation du sol
	createWalls(&labData); // Création des murs
	createWallsHitbox(); // Création de la hitbox des murs
	createAffiche(&labData, &vars); // Création des affiche
	createBoites(&labData); // Création des caisses
	createTresor(&labData); // Création du trésor
	createMovers(&labData); // Création du chasseur et des gardiens
	setTresorDistance(); // On met à chaque case la distance  par rapport au trésor
}

std::vector<std::string> Labyrinthe::getLines(char* file) // Lit le fichier file et le stocke dans un vecteur de string
{
	std::vector<std::string> lines;
	std::ifstream laby(file);
	if (laby.is_open())
	{
		std::string line;
		while (std::getline(laby,line) ) // On lit chaque ligne du fichier qu'on stacke dans un string
		{
			lines.push_back(std::string(line));
		}
		laby.close();
	}
	return lines;
}

void Labyrinthe::eraseComments(std::vector<std::string> *lines) // Efface les commentaires
{
    if(lines)
    {
        for(auto&& line : *lines)
        {
            std::size_t commentLoc = line.find('#'); //Dès qu'on rencontre un #, on efface le reste de la ligne
            if(commentLoc != line.npos)
            {
                line.erase(commentLoc, line.npos);
            }   
        }
        
    }
}

std::map<char, std::string> Labyrinthe::getVars(std::vector<std::string> *lines) // Cherche les variables pour les images des affiches
{
	std::map<char, std::string> vars; // Associe le nom de la variable (une lettre) a un nom de fichier
	if(lines)
	{
		for(auto&& line : *lines)
		{
			if(line.empty())
			{
				continue;
			}
			char firstChar = line.at(0); 
			if(firstChar == '+') //Si on arrive aux données du labyrinthe, on arrete
			{
				return vars;
			}
			else if((firstChar >= 'a' && firstChar <= 'z') || (firstChar >= 'A' && firstChar <= 'Z')) // Si la ligne commence par une lettre
			{
				std::string val;
				bool readStart = false;
				for(auto&& c : line.substr(1, line.npos)) // On lit le reste de la ligne en ignorant les espaces
				{
					if(c == ' ' || c == '\t') //Si on lit un caractère blanc
					{
						if (readStart) //Si on a déjà commencé à lire la valeur de la variable 
						{
							break; //On a fini de lire la valeur
						}
					}
					else
					{
						readStart = true;
						val.push_back(c);
					}
				}
				vars[firstChar] = std::string(val);
				
			}
		}
	}
	return vars;	
}		


std::vector<std::vector<char>> Labyrinthe::getLabData(std::vector<std::string> *lines) // Lit les données du labyrinthe. Est stocké dans un vecteur de vecteur de char
{
	bool toData = false;
	std::vector<std::vector<char>> labData;
	std::size_t maxLineSize = 0;
	if(lines)
	{
		for(auto&& line : *lines)
		{
			if(line.empty())
			{
				continue;
			}
			if(line.at(0) == '+') //On arrive au labyrinthe
			{
				toData = true;
			}
			if(toData) // Lorsqu'on arrive aux données du labyrinthe
			{
				maxLineSize = maxLineSize < line.size() ? line.size() : maxLineSize; //Détermination de la largeur du labyrinthe
				std::vector<char> l;
				for(auto&& c : line)
				{
					l.push_back(c);					
				}
				
				labData.push_back(l);
			}
		}
	}
	lab_width = labData.size();
	lab_height = maxLineSize;
	return labData;
}

void Labyrinthe::createWalls(const std::vector<std::vector<char>> *labData) // Recherche et placement des murs
{
	std::vector<Corner> cornerList = getCornerList(labData); // On cherche tousles coins de murs
	_walls = new Wall[_nwall];
	int wallNb = 0;
	for(auto&& corner : cornerList) //pour chaque coin de mur
	{
		if(corner.hasDown) // Si le mur continue vers le bas
		{
			for(std::size_t i = corner.x + 1; i < labData->size(); i++) // On cherche le prochain + vers le bas
			{
				if(labData->at(i).at(corner.y) == '+') // Quand on le trouve, on crée le mur
				{
					_walls[wallNb]._x1 = corner.x;
					_walls[wallNb]._x2 = i;
					_walls[wallNb]._y1 = corner.y;
					_walls[wallNb]._y2 = corner.y;
					_walls[wallNb]._ntex = 0;
					wallNb++;
					break;
				}
			}
		}	
		if(corner.hasRight) // Si le mur continue vers la droite
		{
			for(std::size_t i = corner.y + 1; i < labData->at(corner.x).size(); i++) // On cherche le prochain + vers la droite
			{
				if(labData->at(corner.x).at(i) == '+') // Quand on le trouve, on crée le mur
				{
					_walls[wallNb]._x1 = corner.x;
					_walls[wallNb]._x2 = corner.x;
					_walls[wallNb]._y1 = corner.y;
					_walls[wallNb]._y2 = i;
					_walls[wallNb]._ntex = 0;
					wallNb++;
					break;
				}
			}
		}
	}
}

std::vector<Corner> Labyrinthe::getCornerList(const std::vector<std::vector<char>> *labData) // Cherche tous les coins de mur
{
	std::vector<Corner> cornerList;
	_nwall = 0;
	if(labData)
	{
		for(std::size_t i = 0; i < labData->size(); i++)
		{
			for(std::size_t j = 0; j < labData->at(i).size(); j++) // Pour chaque caractère du labyrinthe
			{
				if(labData->at(i).at(j) == '+') // Si c'est un coin
				{
					Corner w;
					w.x = i;
					w.y = j;
					w.hasDown = false;
					w.hasRight = false;
					if(i != labData->size() - 1) // on cherche si le mur continue vers le bas
					{
						char c = labData->at(i + 1).at(j);
						if(isWall(c))
						{
							w.hasDown = true;
							_nwall++;
						}
					}
					if(j != labData->at(i).size() - 1)  // on cherche si le mur continue vers la droite
					{
						char c = labData->at(i).at(j + 1);
						if(isWall(c))
						{
							w.hasRight = true;
							_nwall++;
						}
					}
					cornerList.push_back(w);
				}
			}
			
		}
	}
	return cornerList;
}

bool isWall(char c) // Est un mur tout ce qui n'est pas une case vide, un gardien, un trésor, un chasseur ou une caisse
{
	switch (c)
	{
		case ' ':
		case 'G':
		case 'T':
		case 'C':
		case 'X':
			return false;
				
		default:
			return true;
	}
}

void Labyrinthe::createSolVide() // initialisation du tableau d'occupation à EMPTY
{
	_data = new char*[lab_width];
	for (int i = 0; i < lab_width; ++i)
	{
		_data[i] = new char[lab_height];
	}

	// initialisation du tableau d'occupation du sol.
	for (int i = 0; i < lab_width; ++i)
	{
		for (int j = 0; j < lab_height; ++j) 
		{
			_data[i][j] = EMPTY;
		}
	}
		
}

void Labyrinthe::createWallsHitbox() // Crée la hitbox des murs
{
	for(int i = 0; i < _nwall; i++) // Pour chaque mur
	{
		Wall w = _walls[i];
		if (w._x1 == w._x2) // Mur vertical
		{
			for(int j = w._y1; j <= w._y2; j++) 
			{
				_data[w._x1][j] = 1; // On met les cases correspondantes à 1
			}
			
		}
		else // Mur horizontal
		{
			for(int j = w._x1; j <= w._x2; j++)
			{
				_data[j][w._y1] = 1; // On met les cases correspondantes à 1
			}
		}
		
	}
	
}

void Labyrinthe::createAffiche(const std::vector<std::vector<char>> *labData, std::map<char, std::string> *vars) // Cherche et place les affiches
{
	if(labData)
	{
		std::string tex_dir(texture_dir); // Initialisation du répertoire des textures
		std::vector<Affiche> affiches;
		for(std::size_t i = 0; i < labData->size(); i++)
		{
			for(std::size_t j = 0; j < labData->at(i).size(); j++) // Pour chaque case du labyrinthe
			{
				char c = labData->at(i).at(j);
				if(isAffiche(c, vars)) //Si la case est une affiche
				{
					Affiche a;
					a.x = i;
					a.y = j;		
					a.imagePath = vars->at(c);
					if(j != labData->at(i).size() - 1) // Si on est pas au bord droit du labyrinthe
					{
						char caseRight = labData->at(i).at(j + 1); // Case à droite de l'affiche
						a.orientation = !(caseRight == '+' || caseRight == '-'); // L'orientation change selon si l'affiche est sur un mur horizontal ou vertical				
					}
					affiches.push_back(a);
				}
			}
		}
		_picts = new Wall[affiches.size()];
		for(std::size_t i = 0; i < affiches.size(); i++) // On crée les affiches
		{
			Affiche a = affiches[i];
			_picts[i]._x1 = a.x;
			_picts[i]._y1 = a.y;
			_picts[i]._x2 = a.orientation ? a.x - 2 : a.x; // 2 cases de largeur pour un affichage correct
			_picts[i]._y2 = a.orientation ? a.y : a.y - 2;

			char tmp [128];
			sprintf(tmp, "%s/%s", texture_dir, a.imagePath.data());
			_picts[i]._ntex = wall_texture(tmp); // On charge la texture de l'affiche
			
		}
	
	_npicts = affiches.size();
		
	}
	
}

bool isAffiche(char c, std::map<char, std::string> *vars) // Est une affiche si le caractère c est dans les variables du labyrinthe
{
	if(vars && vars->count(c) > 0)
	{
		return true;
	}
	return false;
}

void Labyrinthe::createBoites(const std::vector<std::vector<char>> *labData) // Cherche et place les caisses
{
	if(labData)
	{
		std::vector<std::pair<int, int>> boites; // Vecteur de position des caisses
		for(std::size_t i = 0; i < labData->size(); i++)
		{
			for(std::size_t j = 0; j < labData->at(i).size(); j++)
			{
				if(labData->at(i).at(j) == 'X') // On cherche chaque caisse et on garde ses coordonnées
				{
					boites.push_back(std::pair<int, int>(i, j));
				}
			}
		}
		_nboxes = boites.size();
		_boxes = new Box[_nboxes];
		for(std::size_t i = 0; i < boites.size(); i++) // On crée les caisses
		{
			_boxes[i]._x = boites[i].first;
			_boxes[i]._y = boites[i].second;
			_boxes[i]._ntex = 0;
			_data [_boxes [i]._x][_boxes [i]._y] = 1; // On crée la hitbox de la caisse
		}
	}

}

void Labyrinthe::createTresor(const std::vector<std::vector<char>> *labData) // Cherche et place les caisses
{
	if(labData)
	{
		for(std::size_t i = 0; i < labData->size(); i++)
		{
			for(std::size_t j = 0; j < labData->at(i).size(); j++) 
			{
				if(labData->at(i).at(j) == 'T') // On cherche le trésor, on le crée et on met sa hitbox
				{
					_treasor._x = i;
					_treasor._y = j;
					_data[i][j] = 1;
				}
			}
		}
	}
}

void Labyrinthe::createMovers(const std::vector<std::vector<char>> *labData) // Cherche et place le gardiens et les chasseurs
{
	if(labData)
	{
		std::pair<int, int> chasseurCoords(0, 0); // Coordonnées par défaut du chasseur
		std::vector<std::pair<int, int>> gardiens; // Coordonnées de chaque gardien
		for(std::size_t i = 0; i < labData->size(); i++)
		{
			for(std::size_t j = 0; j < labData->at(i).size(); j++) // pour chaque case du labyrinthe
			{
				if(labData->at(i).at(j) == 'G') // On tombe sur un gardien
				{
					gardiens.push_back(std::pair<int, int>(i, j));
				}
				else if (labData->at(i).at(j) == 'C') // On tombe sur le chasseur
				{
					chasseurCoords.first = i;
					chasseurCoords.second = j;
				}
				
			}
		}
		_nguards = gardiens.size() + 1;
		_guards = new Mover*[_nguards];
		_guards[0] = new Chasseur(this); // On crée le chasseur
		_guards[0]->_x = chasseurCoords.first * scale; // On le place
		_guards[0]->_y = chasseurCoords.second * scale;
		std::vector<std::string> ennemis = initEnnemisList(); // On cherche tous les modèles d'ennemis dans le dossier modeles
		for(std::size_t i = 0; i < gardiens.size(); i++)
		{
			srand(time(0) + i);
			_guards[i + 1] = new Gardien(this, randomGuard(ennemis)); // Le modèle est aléatoire
			_guards[i + 1]->_x = gardiens[i].first * scale; // On le place
			_guards[i + 1]->_y = gardiens[i].second * scale;
		}
		
	}
}

std::vector<std::string> initEnnemisList() // Cherche dans le dossier modeles tous les modeles 3D pour les ennemis
{
	std::vector<std::string> ennemis;
	DIR *dir = opendir("./modeles"); // On ouvre le dossier
	struct dirent *ent;
	if (dir) 
	{
		while ((ent = readdir (dir)))  // On itère sur chaque fichier
		{
			std::string filename(ent->d_name);
			unsigned int iDot = filename.rfind('.');

			if(iDot != std::string::npos) // Si il y a un point dans le nom du fichier
			{
				std::string ext = filename.substr(iDot+1); // On extrait l'extension
				if(ext == "md2") // Si c'est md2, on le rajoute dans la liste d'ennemis
				{
					ennemis.push_back(filename.substr(0, iDot));
				}
			}
		}
		closedir (dir);
	}
	return ennemis;
}

const char* randomGuard(std::vector<std::string> ennemis) // Renvoie un modele de gardien aléatoire
{
	int num = (int) (ennemis.size() * rand() / (double) RAND_MAX);
	return ennemis[num].data();
}

void Labyrinthe::setTresorDistance() // Indique sur chaque case la distance au trésor
{
	_dist = new int*[lab_width];
	for (int i = 0; i < lab_width; ++i)
	{
		_dist[i] = new int[lab_height];
	}
		
	// Initialisation à 0 du tableau de distance du trésor
	for (int i = 0; i < lab_width; ++i)
	{
		for (int j = 0; j < lab_height; ++j) 
		{
			_dist[i][j] = 0;
		}
	}

	// Calcul des distances à partir du trésor
	setDistance(_treasor._x, _treasor._y, 0); // On lance la fonction récursive sur la case du trésor

	// Calcul de la distance maximale
	_dist_max = 0;
	for (int i = 0; i < lab_width; ++i)
	{
		for (int j = 0; j < lab_height; ++j) 
		{
			if(_dist[i][j] > _dist_max) _dist_max = _dist[i][j];
		}
	}
}

void Labyrinthe::setDistance(int x, int y, int value) // Met la valeur value sur la case si sa valeur est nulle ou supérieure à value. Si la valeur a été modifié, lance la fonction sur les 4 cases voisines avec une valeur incrémentée
{
	if(x >= 0 && y >= 0 && x < lab_width && y < lab_height) // On vérifie qu'on ne sort pas du labyrinthe
	{
		_dist[x][y] = value;
		if(x - 1 >= 0) // Case à gauche
		{
			if((_dist[x - 1][y] > (value + 1) || (_dist[x - 1][y] == 0 && (_treasor._x != (x - 1) || _treasor._y != y))) && _data[x - 1][y] == EMPTY) // On continue dans cette direction seulement si sa valeur est plus grande, que la case ne soit pas celle du trésor et que la case ne soit pas un mur
			{
				setDistance(x - 1, y, value + 1);
			}
		}
		if(y - 1 >= 0) // Case en haut
		{
			if((_dist[x][y - 1] > (value + 1) || (_dist[x][y - 1] == 0 && (_treasor._x != (x) || _treasor._y != y - 1))) && _data[x][y - 1] == EMPTY) // On continue dans cette direction seulement si sa valeur est plus grande, que la case ne soit pas celle du trésor et que la case ne soit pas un mur
			{
				setDistance(x, y - 1, value + 1);
			}
		}
		if(x + 1 < lab_width) // Case à droite
		{
			if((_dist[x + 1][y] > (value + 1) || (_dist[x + 1][y] == 0 && (_treasor._x != (x + 1) || _treasor._y != y))) && _data[x + 1][y] == EMPTY) // On continue dans cette direction seulement si sa valeur est plus grande, que la case ne soit pas celle du trésor et que la case ne soit pas un mur
			{
				setDistance(x + 1, y, value + 1);
			}
		}
		if(y + 1 < lab_height) // Case en bas
		{
			if((_dist[x][y + 1] > (value + 1) || (_dist[x][y + 1] == 0 && (_treasor._x != (x) || _treasor._y != y + 1))) && _data[x][y + 1] == EMPTY) // On continue dans cette direction seulement si sa valeur est plus grande, que la case ne soit pas celle du trésor et que la case ne soit pas un mur
			{
				setDistance(x, y + 1, value + 1);
			}
		}
	}
}

