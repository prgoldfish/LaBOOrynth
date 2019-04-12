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

Labyrinthe::Labyrinthe (char* filename)
{ 
	std::vector<std::string> lines = getLines(filename);
	eraseComments(&lines);
	std::map<char, std::string> vars = getVars(&lines);
	std::vector<std::vector<char>> labData = getLabData(&lines);
	createSolVide();
	createWalls(&labData);
	createWallsHitbox();
	createAffiche(&labData, &vars);
	createBoites(&labData);
	createTresor(&labData);
	createMovers(&labData);
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
			if(toData)
			{
				maxLineSize = maxLineSize < line.size() ? line.size() : maxLineSize; 
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

void Labyrinthe::createWalls(const std::vector<std::vector<char>> *labData)
{
	std::vector<Corner> cornerList = getCornerList(labData);
	_walls = new Wall[_nwall];
	int wallNb = 0;
	for(auto&& corner : cornerList)
	{
		if(corner.hasDown)
		{
			for(std::size_t i = corner.x + 1; i < labData->size(); i++)
			{
				if(labData->at(i).at(corner.y) == '+')
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
		if(corner.hasRight)
		{
			for(std::size_t i = corner.y + 1; i < labData->at(corner.x).size(); i++)
			{
				if(labData->at(corner.x).at(i) == '+')
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

std::vector<Corner> Labyrinthe::getCornerList(const std::vector<std::vector<char>> *labData)
{
	std::vector<Corner> cornerList;
	_nwall = 0;
	if(labData)
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
					w.hasDown = false;
					w.hasRight = false;
					if(i != labData->size() - 1)
					{
						char c = labData->at(i + 1).at(j);
						if(isWall(c))
						{
							w.hasDown = true;
							_nwall++;
						}
					}
					if(j != labData->at(i).size() - 1)
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

bool isWall(char c)
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

void Labyrinthe::createSolVide()
{
	_data = new char*[lab_width];
	for (int i = 0; i < lab_width; ++i)
		_data[i] = new char[lab_height];
	// initialisation du tableau d'occupation du sol.
	for (int i = 0; i < lab_width; ++i)
		for (int j = 0; j < lab_height; ++j) {
			_data[i][j] = EMPTY;
		}
}

void Labyrinthe::createWallsHitbox()
{
	for(int i = 0; i < _nwall; i++)
	{
		Wall w = _walls[i];
		if (w._x1 == w._x2) 
		{
			for(int j = w._y1; j <= w._y2; j++)
			{
				_data[w._x1][j] = 1;
			}
			
		}
		else 
		{
			for(int j = w._x1; j <= w._x2; j++)
			{
				_data[j][w._y1] = 1;
			}
		}
		
	}
	
}

void Labyrinthe::createAffiche(const std::vector<std::vector<char>> *labData, std::map<char, std::string> *vars)
{
	if(labData)
	{
		std::string tex_dir(texture_dir);
		std::vector<Affiche> affiches;
		for(std::size_t i = 0; i < labData->size(); i++)
		{
			for(std::size_t j = 0; j < labData->at(i).size(); j++)
			{
				char c = labData->at(i).at(j);
				if(isAffiche(c, vars))
				{
					Affiche a;
					a.x = i;
					a.y = j;		
					a.imagePath = vars->at(c);
					if(j != labData->at(i).size() - 1)
					{
						char caseRight = labData->at(i).at(j + 1);
						a.orientation = !(caseRight == '+' || caseRight == '-');						
					}
					affiches.push_back(a);
				}
			}
		}
		_picts = new Wall[affiches.size()];
		for(std::size_t i = 0; i < affiches.size(); i++)
		{
			Affiche a = affiches[i];
			_picts[i]._x1 = a.x;
			_picts[i]._y1 = a.y;
			_picts[i]._x2 = a.orientation ? a.x - 2 : a.x;
			_picts[i]._y2 = a.orientation ? a.y : a.y - 2;

			char tmp [128];
			sprintf(tmp, "%s/%s", texture_dir, a.imagePath.data());
			_picts[i]._ntex = wall_texture(tmp);
			
		}
	
	_npicts = affiches.size();
		
	}
	
}

bool isAffiche(char c, std::map<char, std::string> *vars)
{
	if(vars && vars->count(c) > 0)
	{
		return true;
	}
	return false;
}

void Labyrinthe::createBoites(const std::vector<std::vector<char>> *labData)
{
	if(labData)
	{
		std::vector<std::pair<int, int>> boites;
		for(std::size_t i = 0; i < labData->size(); i++)
		{
			for(std::size_t j = 0; j < labData->at(i).size(); j++)
			{
				if(labData->at(i).at(j) == 'X')
				{
					boites.push_back(std::pair<int, int>(i, j));
				}
			}
		}
		_nboxes = boites.size();
		_boxes = new Box[_nboxes];
		for(std::size_t i = 0; i < boites.size(); i++)
		{
			_boxes[i]._x = boites[i].first;
			_boxes[i]._y = boites[i].second;
			_boxes[i]._ntex = 0;
			_data [_boxes [i]._x][_boxes [i]._y] = 1;	
		}
	}

}

void Labyrinthe::createTresor(const std::vector<std::vector<char>> *labData)
{
	if(labData)
	{
		for(std::size_t i = 0; i < labData->size(); i++)
		{
			for(std::size_t j = 0; j < labData->at(i).size(); j++)
			{
				if(labData->at(i).at(j) == 'T')
				{
					_treasor._x = i;
					_treasor._y = j;
					_data[i][j] = 1;
				}
			}
		}
	}
}

void Labyrinthe::createMovers(const std::vector<std::vector<char>> *labData)
{
	if(labData)
	{
		std::pair<int, int> chasseurCoords(0, 0);
		std::vector<std::pair<int, int>> gardiens;
		for(std::size_t i = 0; i < labData->size(); i++)
		{
			for(std::size_t j = 0; j < labData->at(i).size(); j++)
			{
				if(labData->at(i).at(j) == 'G')
				{
					gardiens.push_back(std::pair<int, int>(i, j));
				}
				else if (labData->at(i).at(j) == 'C') 
				{
					chasseurCoords.first = i;
					chasseurCoords.second = j;
				}
				
			}
		}
		_nguards = gardiens.size() + 1;
		_guards = new Mover*[_nguards];
		_guards[0] = new Chasseur(this);
		_guards[0]->_x = chasseurCoords.first * scale;
		_guards[0]->_y = chasseurCoords.second * scale;
		initEnnemisList();
		for(std::size_t i = 0; i < gardiens.size(); i++)
		{
			srand(time(0) + i);
			_guards[i + 1] = new Gardien(this, randomGuard());
			_guards[i + 1]->_x = gardiens[i].first * scale;
			_guards[i + 1]->_y = gardiens[i].second * scale;
		}
		
	}
}

void initEnnemisList()
{
	DIR *dir = opendir("./modeles");
	struct dirent *ent;
	if (dir) 
	{
		while ((ent = readdir (dir))) {
			std::string filename(ent->d_name);
			unsigned int iDot = filename.rfind('.');

			if(iDot != std::string::npos)
			{
				std::string ext = filename.substr(iDot+1);
				if(ext == "md2")
				{
					ennemis.push_back(filename.substr(0, iDot));
				}
			}
		}
		closedir (dir);
	}
}

const char* randomGuard()
{
	int num = (int) (ennemis.size() * rand() / (double) RAND_MAX);
	return ennemis[num].data();
}

