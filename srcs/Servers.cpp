#include "Servers.hpp"

Servers::Servers(std::string &conf_file){
    std::ifstream confFile(conf_file.c_str());
    if (!confFile.is_open()){
        std::cout << "Error: cannot open config file." << std::endl;
        return ;
    }
    
    
}

Servers::Servers(Servers const &cpy){

}

Servers::~Servers(){}

Servers &Servers::operator=(Servers const &rhs){
    
}


