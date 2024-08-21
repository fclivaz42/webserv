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

std::pair<std::string, std::string>	Servers::pairConf(std::string line){
	std::size_t	pos = line.find(' ');
	std::string	key = line.substr(0, pos);
	std::string	value = line.substr(pos);
	value.erase(0, value.find_first_not_of(' '));

	return (std::pair<std::string, std::string>(key, value));
}

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

void	Servers::printConfigs() const {
        std::vector<ServerConf>::const_iterator it = _servConf.begin();
        for (; it != _servConf.end(); ++it) {
            it->print();
        }
    }
