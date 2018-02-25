#include "Server.h"

std::vector<Room> Server::rooms;
std::vector<Game> Server::games;

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
    createGames();
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

void Server::createGames() {
    for(int i=0; i<6; i++){
        games.emplace_back(rooms[i], Board());
    }
    std::cout<< "Games created" << std::endl;
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
    sendBoard(received_data.client_desc);

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
    std::string room_names;
    std::string temp;
    for(int i =0; i<6; i++){
        temp = std::to_string(rooms[i].getFreeSlots());
        room_names.append(rooms[i].getName()).append("_").append(temp).append("_");
    }

    const char *cstr = room_names.c_str();

    if(write(desc, cstr, sizeof(cstr)*strlen(cstr)) <0 ){
        perror("Couldn't send avaible rooms");
        printf("%d", errno);
    }
    std:: cout << "Send rooms" << std::endl;
}

void Server::sendBoard(int desc) {
    std::string temp = "WIADOMOSC";

    for(int i=0; i<15; i++)
        for(int j=0; j<15; j++)
        {
            temp = (temp + games[0].board.board[i][j]).append("_");
        }
    const char *cstr = temp.c_str();

    std::cout<<cstr<<std::endl;

    if(write(desc, cstr, sizeof(cstr)*strlen(cstr)) <0){
        perror("Couldn't send current board");
        printf("%d", errno);
    }
    std::cout << "Board send" << std::endl;
    temp = nullptr;
}




