#include "Server.h"

Server::Server() {
    int nFoo = 1;
    memset(&this->server_struct, 0 , sizeof(this->server_struct));
    this->server_struct.sin_family = AF_INET;
    this->server_struct.sin_port = htons(this->service_port);
    inet_aton(this->server_address, &this->server_struct.sin_addr);
    setsockopt(this->socket_desc, SOL_SOCKET, SO_REUSEADDR, (char *)&nFoo, sizeof(nFoo));
}

void Server::run() {
    createSocket();
    bindSocket();
    listenForConnections();
    std::cout << "Server running!" << std::endl;
    acceptConnection();
}

void Server::createSocket() {
    this->socket_desc = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(this->socket_desc < 0){
        perror("Socket creation failed!");
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket created" << std::endl;
}

void Server::bindSocket() {
    if(bind(this->socket_desc, (struct sockaddr*)&this->server_struct, sizeof(this->server_struct)) < 0){
        perror("Socket bind failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket bounded" << std::endl;
}

void Server::listenForConnections() {
    if(listen(this->socket_desc, 32) <0 ){
        perror("Listen for connection failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "Listening for new connections" << std::endl;
}

void Server::acceptConnection() {
    struct sockaddr_in player_tmp;
    socklen_t player_tmp_size;
    while(int client_desc = accept(this->socket_desc, (struct sockaddr*)&player_tmp, &player_tmp_size) > 0){
        std::cout << "Connection with client: " << inet_ntoa(player_tmp.sin_addr) << std::endl;
        players.emplace_back(player_tmp, player_tmp_size);
    }
}
