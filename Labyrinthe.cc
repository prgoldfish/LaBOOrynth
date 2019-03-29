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

/*
 *	EXEMPLE de labyrinthe.
 */

Labyrinthe::Labyrinthe (char* filename)
{ 
	std::vector<std::string> lines = getLines(filename);
	eraseComments(&lines);
	std::map<char, std::string> vars = getVars(&lines);
	std::vector<std::vector<char>> labData = getLabData(&lines);
	createWalls(&labData);
	for(auto&& line : lines)
	{
		std::cout << line << "\n";
	}
	for(auto&& kv : vars)
	{
		std::cout << kv.first << "\t" << kv.second << "\n";
	}
	
	

	// taille du labyrinthe.
	lab_height = 80;
	lab_width = 25;

	// les murs: 4 dans cet EXEMPLE!
	int	n = 0;

	_walls = new Wall [4];
	// le premier.
	_walls [n]._x1 = 0; _walls [n]._y1 = 0; 
	_walls [n]._x2 = 0; _walls [n]._y2 = lab_height-1;
	_walls [n]._ntex = 0;
	++n;
	// le deuxieme.
	_walls [n]._x1 = 0; _walls [n]._y1 = lab_height-1;
	_walls [n]._x2 = lab_width-1; _walls [n]._y2 = lab_height-1;
	_walls [n]._ntex = 0;
	++n;
	// le troisieme.
	_walls [n]._x1 = lab_width-1; _walls [n]._y1 = lab_height-1;
	_walls [n]._x2 = lab_width-1; _walls [n]._y2 = 0;
	_walls [n]._ntex = 0;
	++n;
	// le dernier.
	_walls [n]._x1 = lab_width-1; _walls [n]._y1 = 0;
	_walls [n]._x2 = 0; _walls [n]._y2 = 0;
	_walls [n]._ntex = 0;
	++n;
	_nwall = n;

	// une affiche.
	//  (attention: pour des raisons de rapport d'aspect, les affiches doivent faire 2 de long)
	n = 0;
	_picts = new Wall [2];
	// la premi�re (texture par d�faut).
	_picts [n]._ntex = 0;
	_picts [n]._x1 = 0; _picts [n]._y1 = 4;
	_picts [n]._x2 = 0; _picts [n]._y2 = 6;
	++n;

	// la deuxi�me a une texture diff�rente.
	char	tmp [128];
	sprintf (tmp, "%s/%s", texture_dir, "voiture.jpg");

	_picts [n]._ntex = wall_texture (tmp);
	_picts [n]._x1 = 0; _picts [n]._y1 = 8;
	_picts [n]._x2 = 0; _picts [n]._y2 = 10;
	++n;
	_npicts = n;

	// 3 caisses.
	_boxes = new Box [3];

	n = 0;
	// la premi�re.
	_boxes [n]._x = 12; _boxes [n]._y = 70; _boxes [n]._ntex = 0;
	++n;
	// la deuxieme.
	_boxes [n]._x = 5; _boxes [n]._y = 10; _boxes [n]._ntex = 0;
	++n;
	// la derni�re.
	_boxes [n]._x = 22; _boxes [n]._y = 65; _boxes [n]._ntex = 0;
	++n;
	_nboxes = n;

	// cr�ation du tableau d'occupation du sol.
	_data = new char* [lab_width];
	for (int i = 0; i < lab_width; ++i)
		_data [i] = new char [lab_height];
	// initialisation du tableau d'occupation du sol.
	for (int i = 0; i < lab_width; ++i)
		for (int j = 0; j < lab_height; ++j) {
			// murs sur les bords.
			if (i == 0 || i == lab_width-1 || j == 0 || j == lab_height-1)
				_data [i][j] = 1;
			else
				// rien dedans.
				_data [i][j] = EMPTY;
		}

	// indiquer qu'on ne marche pas sur une caisse.
	_data [_boxes [0]._x][_boxes [0]._y] = 1;
	_data [_boxes [1]._x][_boxes [1]._y] = 1;
	_data [_boxes [2]._x][_boxes [2]._y] = 1;

	// coordonn�es du tr�sor.
	_treasor._x = 10;
	_treasor._y = 10;
	// indiquer qu'on ne marche pas dessus.
	_data [_treasor._x][_treasor._y] = 1;

	// le chasseur et les 4 gardiens.
	_nguards = 1+4;
	_guards = new Mover* [_nguards];
	_guards [0] = new Chasseur (this);
	_guards [1] = new Gardien (this, "drfreak");
	_guards [2] = new Gardien (this, "Marvin"); _guards [2] -> _x = 90.; _guards [2] -> _y = 70.;
	_guards [3] = new Gardien (this, "Potator"); _guards [3] -> _x = 60.; _guards [3] -> _y = 10.;
	_guards [4] = new Gardien (this, "garde"); _guards [4] -> _x = 130.; _guards [4] -> _y = 100.;

	// indiquer qu'on ne marche pas sur les gardiens.
	_data [(int)(_guards [1] -> _x / scale)][(int)(_guards [1] -> _y / scale)] = 1;
	_data [(int)(_guards [2] -> _x / scale)][(int)(_guards [2] -> _y / scale)] = 1;
	_data [(int)(_guards [3] -> _x / scale)][(int)(_guards [3] -> _y / scale)] = 1;
	_data [(int)(_guards [4] -> _x / scale)][(int)(_guards [4] -> _y / scale)] = 1;
}

std::vector<std::string> Labyrinthe::getLines(char* file)
{
	std::vector<std::string> lines;
	std::ifstream laby(file);
	if (laby.is_open())
	{
		std::string line;
		while (std::getline(laby,line) )
		{
			lines.push_back(std::string(line));
		}
		laby.close();
	}
	return lines;
}

void Labyrinthe::eraseComments(std::vector<std::string> *lines)
{
    if(lines)
    {
        for(auto&& line : *lines)
        {
            std::size_t commentLoc = line.find('#');
            if(commentLoc != line.npos)
            {
                line.erase(commentLoc, line.npos);
            }   
        }
        
    }
}

std::map<char, std::string> Labyrinthe::getVars(std::vector<std::string> *lines)
{
	std::map<char, std::string> vars; 
	if(lines)
	{
		for(auto&& line : *lines)
		{
			if(line.empty())
			{
				continue;
			}
			char firstChar = line.at(0); 
			if(firstChar == '+')
			{
				return vars;
			}
			else if((firstChar >= 'a' && firstChar <= 'z') || (firstChar >= 'A' && firstChar <= 'Z'))
			{
				std::string val;
				bool readStart = false;
				for(auto&& c : line.substr(1, line.npos))
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


std::vector<std::vector<char>> Labyrinthe::getLabData(std::vector<std::string> *lines)
{
	bool toData = false;
	std::vector<std::vector<char>> labData;
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
			if(toData)
			{
				std::vector<char> l;
				for(auto&& c : line)
				{
					l.push_back(c);					
				}
				
				labData.push_back(l);
			}
		}
	}
	return labData;
}

void Labyrinthe::createWalls(const std::vector<std::vector<char>> *labData)
{
	int nbWalls = 0;
	std::vector<Corner> cornerList = getCornerList(labData, &nbWalls);
	
	
}

std::vector<Corner> Labyrinthe::getCornerList(const std::vector<std::vector<char>> *labData, int *nbWalls)
{
	std::vector<Corner> cornerList;
	if(labData && nbWalls)
	{
		for(std::size_t i = 0; i < labData->size(); i++)
		{
			for(std::size_t j = 0; j < labData->at(i).size(); j++)
			{
				if(labData->at(i).at(j) == '+')
				{
					Corner w;
					w.x = i;
					w.y = j;
					if(i != labData->size() - 1)
					{
						char c = labData->at(i + 1).at(j);
						if(isWall(c))
						{
							(*nbWalls)++;
						}
					}
					if(j != labData->at(i).size() - 1)
					{
						char c = labData->at(i).at(j + 1);
						if(isWall(c))
						{
							(*nbWalls)++;
						}
					}
					cornerList.push_back(w);
				}
			}
			
		}
	}
	return cornerList;
}

bool Labyrinthe::isWall(char c)
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

