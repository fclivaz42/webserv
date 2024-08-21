#include "Servers.hpp"

Servers::Servers(const std::string &conf_file){
		loadConfig(conf_file);
}

Servers::Servers(Servers const &cpy){
	*this = cpy;
}

Servers::~Servers(){}

Servers &Servers::operator=(Servers const &rhs){
	this->_servConf = rhs._servConf;
	return (*this);
}

std::vector<ServerConf>	Servers::getServConf(void) const{
	return (this->_servConf);

}

/* Fonction qui va ouvrir le fichier de config et le parser afin de stocker les elements dans les bons attributs.
	1. Ouverture du file et gestion de l'erreur en cas de probleme a l'ouverture
	2. Creation d'une variable de type string whitespace pour pouvoir ignorer tout les espaces du file. 
	3. Creation d'une variable line qui permettera de lire le fichier ligne par ligne et d'une variable LocationFlag qui permettera de savoir si on est a l'interieur d'un scope location.
	4. Creation des instance de class ServerConf et Location pour y stocker les elements. 
	5. Boucle while pour lire le fichier ligne par ligne jusqu'a la fin.
	6. Fonction permettant la suppression des withespace sur la ligne en cours + ignorer ligne vide ou commentaire.
	7. Condition pour rentrer dans le scope server, on met le flag en off et cree une instance de ServerConf
	8. Condition de fin de scope permettant de savoir si on doit set les locations ou ajouter linstance de ServerConf dans le vector de la class Server.
	9. Condtion qui permet de gerer le scope location et passer le flag en on.
	10.Condtion qui permet de traiter toute les donnees a entrer dans les attributs des class Location et ServerConf en faisant des pair "value/key" afin de faciliter l'insertion.
	11.Fermeture du fichier.
*/
void    Servers::loadConfig(std::string const &conf_file){
	std::ifstream confFile(conf_file.c_str());
	if (!confFile.is_open()){
		std::cout << "Error: cannot open config file." << std::endl;
		return ;
	}
	std::string line;
	std::string whitespaces(" \t\f\v\n\r");
	bool        locationFlag = false;
	ServerConf  currentServer;
	Location    currentLocation;

	while (std::getline(confFile, line)){
		line.erase(0, line.find_first_not_of(whitespaces));
		line.erase(line.find_last_not_of(whitespaces) + 1);
		if (line.empty() || line[0] == '#')
			continue;
		if (line.find("server {") != std::string::npos){
			locationFlag = false;
			currentServer = ServerConf();
		}
		else if (line.find("}") != std::string::npos){
			if (locationFlag){
				currentServer.setLocation(currentLocation.getPath(), currentLocation);
				locationFlag = false;
			}
			else
				this->_servConf.push_back(currentServer);
		}
		else if (line.find("location ")!= std::string::npos){
			if (locationFlag)
				currentServer.setLocation(currentLocation.getPath(), currentLocation);
			locationFlag = true;
			currentLocation = Location();
			currentLocation.setPath(line.substr(9, line.size() - 10));
		}
		else{
			std::pair<std::string, std::string> keyValue = pairConf(line);
			if (!locationFlag)
				setConf(keyValue, currentServer);
			else
				setConfLoc(keyValue, currentLocation);
		}
	}
	confFile.close();

	return ;
}

/* Fonction qui permet de faire les paires pour attribue les valeurs aux attributs des class.
	1. Creation d'une variable pos a laquelle on va donner la position de l'espace qui est le separateur.
	2. Creation de la valeur "key" qui comprendra le mot cle qui est le premier mot.
	3. Creation de la valeur "value" qui comprendra la valeur qui est le second mot.
	4. Retour des pairs. 
*/
std::pair<std::string, std::string>	Servers::pairConf(std::string line){
	std::size_t	pos = line.find(' ');
	std::string	key = line.substr(0, pos);
	std::string	value = line.substr(pos);
	value.erase(0, value.find_first_not_of(' '));

	return (std::pair<std::string, std::string>(key, value));
}

/* Fonction permettant de set les attributs de la class ServerConf. */
void	Servers::setConf(std::pair<std::string, std::string> keyValue, ServerConf &currentServer){
	if (keyValue.first == "listen")
		currentServer.setPort(keyValue.second);
	else if (keyValue.first == "server_name")
		currentServer.setServerName(keyValue.second);
	else if (keyValue.first == "root")
		currentServer.setRoot(keyValue.second);
	else if (keyValue.first == "index")
		currentServer.setIndex(keyValue.second);
	else if (keyValue.first == "error_page")
		currentServer.setErrorPage(keyValue.second);

	return ;
}

/* Fonction permettant de set les attributs de la class Location. */
void	Servers::setConfLoc(std::pair<std::string, std::string> keyValue, Location &currentLocation){
	if (keyValue.first == "method")
		currentLocation.setAllowMethods(keyValue.second);
	else if (keyValue.first == "root")
		currentLocation.setRoot(keyValue.second);
	else if (keyValue.first == "return")
		currentLocation.setReturnURL(keyValue.second);
	else if (keyValue.first == "index")
		currentLocation.setIndex(keyValue.second);
	else if (keyValue.first == "accept_uploads")
		currentLocation.setUpload(keyValue.second);

	return ;
}

/* Fonction permettant de print le contenu de l'attribut vector<ServerConf> de la class Server. */
void	Servers::printConfigs() const {
    std::vector<ServerConf>::const_iterator it = _servConf.begin();
    for (; it != _servConf.end(); ++it) {
        it->print();
    }
}
