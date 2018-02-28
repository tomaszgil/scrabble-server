#include <sstream>
#include "Server.h"

std::vector<Room> Server::rooms;
std::vector<Game> Server::games;
std::vector<Player> Server::players;

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

bool Server::sendStringToClient(int desc, std::string &message) {
    const char *cstr = message.c_str();

    if(write(desc, cstr, strlen(cstr))<0){
        printf("%d", errno);
        return false;
    }
    return true;
}

void *Server::handleClient(void *data){
    pthread_data received_data = *((pthread_data *)data);

    receiveUsername(received_data.client_desc, received_data.player);
    sendAvaibleRooms(received_data.client_desc);
    receiveSelectedRoom(received_data.client_desc, received_data.player);
    sendBoard(received_data.client_desc, received_data.player);
    sendAvaibleLetters(received_data.client_desc, received_data.player);
    sendPlayersFromCurrentRoom(received_data.client_desc, received_data.player);
    receiveUserMove(received_data.client_desc, received_data.player);
    sendMoveToOtherPlayers(received_data.client_desc, received_data.player);

    pthread_exit(NULL);
}


void Server::acceptConnection() {
    struct sockaddr_in player_tmp{};
    socklen_t player_tmp_size;
    int client_desc = 0;
    while((client_desc = accept(this->socket_desc, (struct sockaddr*)&player_tmp, &player_tmp_size))> 0){

        players.emplace_back(player_tmp, player_tmp_size, client_desc);

        pthread_data data ={client_desc, players.back()};

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handleClient, (void *)&data);
    }
}


void Server::receiveUsername(int desc, Player &player) {
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

    if(!sendStringToClient(desc, room_names)){
        std::cout<< "Couldn't send avaible rooms" << std::endl;
    }
    std:: cout << "Send rooms" << std::endl;
}

void Server::receiveSelectedRoom(int desc, Player &player) {
    char buffor[50];
    if(read(desc, buffor, sizeof(buffor)) <0){
        perror("Couldn't receive selected room");
        printf("%d", errno);
    }else{
        for(int i=0; i< rooms.size(); i++){
            if(rooms[i].getName()==buffor){
                player.setRoom(rooms[i].getName());
                rooms[i].addPlayer(player);
                rooms[i].setFreeSlots(rooms[i].getFreeSlots()-1);
                std::cout<< "Player: " << player.getUsername() << " entered room " << player.getRoom() << std::endl;

                // Send info to other users in room
                for(int j=0; j < players.size(); j++){
                    if(players[j].getRoom() == player.getRoom() && players[j].getUsername() != player.getUsername()){
                        sendPlayersFromCurrentRoom(players[j].getSocket_desc(), players[j], 1);
                        std::cout<<"Wyslalem info do: " << players[j].getUsername() << std::endl;
                    }
                }
            }
        }
    }

}

void Server::sendBoard(int desc, Player &player, int code) {
    std::string temp;
    games[0].board.board[2][3]='A';

    if(code == 2){
        temp ="2_";
        temp.append(player.getUsername()).append("_").append(player.getScore()).append("_");
    }

    int z =0;
    for(; z<games.size(); z++){
        if(games[z].room.getName() == player.getRoom()){
            break;
        }
    }

    for(int i=0; i<15; i++) {
        for (int j = 0; j < 15; j++) {
            temp = (temp + games[z].board.board[i][j]).append("_");
        }
    }

    if(!sendStringToClient(desc,temp)){
        std::cout << "Couldn't send board" << std::endl;
    }
    std::cout << "Board send" << std::endl;
}

void Server::sendAvaibleLetters(int desc, Player &player){
    std::string temp;

    for(int i=0; i<7; i++){
        char letter = 'A' + rand()%26;
        temp = (temp + letter).append("_");
    }

    if(!sendStringToClient(desc,temp)){
        std::cout << "Could'nt send letters" << std::endl;
    }
    std::cout << "Letters send" << std::endl;
}

void Server::sendPlayersFromCurrentRoom(int desc, Player &player, int x) {
    std::string message;
    std::string temp;


    int foundUsers = 0;
    for(int i=0; i<players.size(); i++){
       if(players[i].getRoom() == player.getRoom() && players[i].getUsername() != player.getUsername()){
            foundUsers++;
               message.append(players[i].getUsername()).append("_").append(players[i].getScore()).append("_");

       }
    }
    if(message.empty()){
        if(x == 1){
            message = "1";
        }else{
            message = "0";
        }
        temp.append(message);
    }else{
        if(x == 1){
            temp = std::to_string(x).append("_").append(std::to_string(foundUsers)).append("_").append(message);
        }else{
            temp = std::to_string(foundUsers).append("_").append(message);
        }
    }

    const char *cstr = temp.c_str();
    int size =0;

    if(x == 1)
        size = 102;
    else
        size = 100;

    if(write(desc, cstr, size)<0){
        printf("%d", errno);
        std::cout<<"Couldn't send players"<<std::endl;
    }
    std::cout<<"Players send"<<std::endl;
}

void Server::receiveUserMove(int desc, Player &player){
    char buffer[480];
    if(read(desc, buffer, sizeof(buffer)) < 0){
        perror("Couldn't receive user move");
        printf("%d", errno);
    }else{
        std::stringstream message(buffer);
        std::string segment;
        std::vector<std::string> seglist;

        while(std::getline(message, segment, '_')){
            seglist.push_back(segment);
        }

        std::string player_name = seglist[1];
        player.setScore(seglist[2]);

        int z=0;
        for(; z<games.size(); z++){
            if(games[z].room.getName() == player.getRoom()){
                break;
            }
        }
        int p=3;
        for(int i=0; i<15; i++){
            for(int j=0; j<15; j++){
                games[z].board.board[i][j]=seglist[p][0];
                p++;
            }
        }
    }
}

void Server::sendMoveToOtherPlayers(int desc, Player &player) {
    int z =0;
    for(; z< rooms.size(); z++){
        if(rooms[z].getName() == player.getRoom()){
            break;
        }
    }
    //std::cout<<"TAKI LADNY POKOJ" << player.getRoom()<<std::endl;
    std::cout<<rooms[z].getName()<<std::endl;
   // std::cout<<rooms[z].players.size()<<std::endl;

    for(int i =0; i<rooms[z].players.size(); i++){
        if(rooms[z].players[i].getUsername() != player.getUsername()){
            sendBoard(rooms[z].players[i].getSocket_desc(), player, 2);
            std::cout<<"Wyslalem nowa plansze do: " << rooms[z].players[i].getUsername() << std::endl;

        }else{
            //TODO Gracz gra samemu wiec kolejny ruch tez jest jego
        }
    }
}







