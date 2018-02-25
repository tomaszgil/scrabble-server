#include "Server.h"

std::vector<Room> Server::rooms;

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
    createRooms();
    acceptConnection();
}

void Server::createRooms() {
    rooms.emplace_back("Alpha", 4);
    rooms.emplace_back("Beta", 4);
    rooms.emplace_back("Charlie", 4);
    rooms.emplace_back("Delta", 4);
    rooms.emplace_back("Echo", 4);
    rooms.emplace_back("Foxtrot", 4);
    std::cout<< "Rooms created"<<    std::endl;
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
    std::cout << "Socket bound" << std::endl;
}

void Server::listenForConnections() {
    if(listen(this->socket_desc, 32) <0 ){
        perror("Listen for connection failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "Listening for new connections" << std::endl;
}

void *Server::handleClient(void *data){

    pthread_data received_data = *((pthread_data *)data);

    receiveUsername(received_data.client_desc, received_data.player);
    sendAvaibleRooms(received_data.client_desc);

    pthread_exit(NULL);
}


void Server::acceptConnection() {
    struct sockaddr_in player_tmp{};
    socklen_t player_tmp_size;
    int client_desc = 0;
    while((client_desc = accept(this->socket_desc, (struct sockaddr*)&player_tmp, &player_tmp_size))> 0){

        players.emplace_back(player_tmp, player_tmp_size);

        pthread_data data ={client_desc, players.back()};

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handleClient, (void *)&data);

    }
}


void Server::receiveUsername(int desc, Player player) {
    char buffor[50];
    if(read(desc, buffor, sizeof(buffor)) < 0){
        perror("Couldn't receive username");
        printf("%d", errno);
    }else{
        player.setUsername(buffor);
        std:: cout << "Connected user: " << player.getUsername() << std::endl;
    }
}

void Server::sendAvaibleRooms(int desc) {
    std::string room_names[6];

    for(int i =0; i<6; i++){
        room_names[i] = rooms[i].getName();
        std:: cout << room_names[i] << std::endl;
    }

    if(write(desc, &room_names, sizeof(room_names)) <0 ){
        perror("Couldn't send avaible rooms");
        printf("%d", errno);
    }
}



