#include "Servers.hpp"

Servers::Servers(const std::string &conf_file){
    std::ifstream confFile(conf_file.c_str());
    if (!confFile.is_open()){
        std::cout << "Error: cannot open config file." << std::endl;
        return ;
    }
    else
        loadConfig(confFile);
    
}

Servers::Servers(Servers const &cpy){
    *this = cpy;
}

Servers::~Servers(){}

Servers &Servers::operator=(Servers const &rhs){
    this->_servConf = rhs._servConf;
    return (*this);
}

void    Servers::loadConfig(std::ifstream confFile){
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
    }
    if (line.find("server {")){
        locationFlag = false;
        currentServer = ServerConf();
    }
}
