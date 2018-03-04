#include <sstream>
#include <mutex>
#include "Server.h"

std::mutex letters_mutex;
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
    fcntl(this->socket_desc, F_SETFL, O_NONBLOCK);
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

bool Server::sendStringToClient(int desc, std::string &message, int size) {
    const char *cstr = message.c_str();

    int length=0;
    if(size !=0){
        length=size;
    }else{
        length=strlen(cstr);
    }

    if(write(desc, cstr, length)<0){
        printf("%d", errno);
        return false;
    }
    return true;
}

void *Server::handleClient(void *data){
   // pthread_data received_data = *((pthread_data *)data);
    Player received_data = *((Player * )data);

    receiveUsername(received_data.getSocket_desc(), received_data);
    sendAvaibleRooms(received_data.getSocket_desc());
    receiveSelectedRoom(received_data.getSocket_desc(), received_data);
    sendBoard(received_data.getSocket_desc(), received_data);
    sendAvaibleLetters(received_data.getSocket_desc(), received_data);
    sendPlayersFromCurrentRoom(received_data.getSocket_desc(), received_data);
   // receiveUserMove(received_data.getSocket_desc(), received_data);
   // sendMoveToOtherPlayers(received_data.getSocket_desc(), received_data);

    char buffer[2] = {'0', '0'};
    int x =0;
    while(true){
        sendPlayersFromCurrentRoom(received_data.getSocket_desc(), received_data, 1 );
        if((x = read(received_data.getSocket_desc(), buffer, sizeof(buffer))) < 0){
            if(errno == EWOULDBLOCK || errno == EAGAIN){
                continue;
            }else{
                perror("Couldn't receive data");
                printf("%d", errno);
            }
        }else if(x!=0 && strlen(buffer) > 0 && buffer[0] != '0' && buffer[1] != '0'){
            if(buffer[0] == '1' && buffer[1] == '_'){
                receiveUserMove(received_data.getSocket_desc(), received_data);
                sendAvaibleLetters(received_data.getSocket_desc(), received_data,1);
                sendMoveToOtherPlayers(received_data.getSocket_desc(), received_data);
            } else if (buffer[0] == '2' && buffer[1] == '_') {
                quitRoom(received_data.getSocket_desc(), received_data);
                sendAvaibleRooms(received_data.getSocket_desc());
                receiveSelectedRoom(received_data.getSocket_desc(), received_data);
                sendBoard(received_data.getSocket_desc(), received_data);
                sendAvaibleLetters(received_data.getSocket_desc(), received_data);
                sendPlayersFromCurrentRoom(received_data.getSocket_desc(), received_data);
            } else if (buffer[0] == 'x') {
                pthread_exit(NULL);
            } else if (buffer[0] == '3' && buffer[1] == '_') {
                swapLetters(received_data.getSocket_desc(), received_data);
            }
        }else{
            std::cout<<"Last step"<<std::endl;
        }
    }
}


void Server::acceptConnection() {
    struct sockaddr_in player_tmp{};
    socklen_t player_tmp_size;
    int client_desc = 0;

    while(true){
        while(true){
            client_desc = accept(this->socket_desc, (struct sockaddr*)&player_tmp, &player_tmp_size);
            if(client_desc>0)
                break;
            else if(errno == EWOULDBLOCK)
                continue;
        }

        players.emplace_back(player_tmp, player_tmp_size, client_desc);

        pthread_data data ={client_desc, players.back()};

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handleClient, (void *)&players.back());
    }

//    while((client_desc = accept(this->socket_desc, (struct sockaddr*)&player_tmp, &player_tmp_size))> 0 || errno != EWOULDBLOCK){
//
//        players.emplace_back(player_tmp, player_tmp_size, client_desc);
//
//        pthread_data data ={client_desc, players.back()};
//
//        pthread_t thread_id;
//        pthread_create(&thread_id, NULL, handleClient, (void *)&players.back());
//    }
}

void Server::receiveUsername(int desc, Player &player) {
    char buffor[50];
    if(read(desc, buffor, sizeof(buffor)) < 0){
        if(errno == EWOULDBLOCK){
            receiveUsername(desc, player);
        }else {
            perror("Couldn't receive username");
            printf("%d", errno);
        }
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
    }else {
        std:: cout << "Send rooms" << std::endl;
    }
}

void Server::receiveSelectedRoom(int desc, Player &player) {
    char buffor[15];
    memset(buffor,'0',15);
    if(read(desc, buffor, sizeof(buffor)) <0){
        if(errno == EWOULDBLOCK){
            receiveSelectedRoom(desc, player);
        }else {
            perror("Couldn't receive selected room");
            printf("%d", errno);
        }
    }else{
        int z =0;
        for(; z<games.size(); z++){
            if(games[z].room.getName() == player.getRoom()){
                break;
            }
        }
        if(buffor[0]=='0'){
            receiveSelectedRoom(desc, player);
        }else{
            std::cout<<buffor[0]<<std::endl;

            for(int i=0; i< rooms.size(); i++){
                if(rooms[i].getName()==buffor){
                    if(rooms[i].players.size()==4){
                        memset(buffor,'0',15);
                        std::string x ="x";
                        sendStringToClient(desc, x, 1);
                        sendAvaibleRooms(desc);
                        receiveSelectedRoom(desc, player);
                        break;
                    }
                    else{
                        if(rooms[i].players.empty() ) {
                            player.setTurn(true);
                        }

                        player.setRoom(rooms[i].getName());
                        rooms[i].addPlayer(player);
                        rooms[i].setFreeSlots(rooms[i].getFreeSlots()-1);
                        std::cout<< "Player: " << player.getUsername() << " entered room " << player.getRoom() << std::endl;

                        for(int j=0; j<rooms[i].players.size(); j++){
                            if(rooms[i].players[j].getUsername() == player.getUsername()){
                                rooms[i].players[j].setRoom(rooms[i].getName());
                                std::string code = "o";
                                sendStringToClient(desc, code, 1);
                            }
                        }

                        // Send info to other users in room
                        for(int j=0; j<rooms[i].players.size(); j++){
                            if(rooms[i].players[j].getUsername() != player.getUsername()){
                                sendPlayersFromCurrentRoom(rooms[i].players[j].getSocket_desc(), rooms[i].players[j], 1);
                                std::cout<<"Sending info about new player to: " << rooms[i].players[j].getUsername() <<std::endl;
                            }
                        }
                    }
                }

            }
        }
    }

}

void Server::sendBoard(int desc, Player &player, int code) {
    std::string temp;
    int length=480;

    if(code == 2){
        length=480;
        temp ="2_";
        temp.append(player.getUsername()).append("_").append(player.getScore()).append("_");
    }

    int z =0;
    for(; z<games.size(); z++){
        if(games[z].room.getName() == player.getRoom()){
            break;
        }
    }

    int y=0;
    for(; y<rooms.size(); y++){
        if(rooms[y].getName() == player.getRoom()){
            break;
        }
    }

    for(int i =0; i<rooms[y].players.size(); i++){
        if(rooms[y].players[i].getSocket_desc() == desc){
            if(rooms[y].players[i].isTurn()){
                temp.append("t_");
            }else{
                temp.append("f_");
            }
        }
    }

    for(int i=0; i<15; i++) {
        for (int j = 0; j < 15; j++) {
            temp = (temp + games[z].board.board[i][j]).append("_");
        }
    }

    while(temp.length() < 480){
        temp.append("0");
    }

    if(!sendStringToClient(desc,temp,length)){
        std::cout << "Couldn't send board" << std::endl;
    }else{
        std::cout << "Board send  " << desc <<std::endl;
    }
}

void Server::sendAvaibleLetters(int desc, Player &player, int x){
    std::string temp;
    Game * game;

    for(int i = 0; i < games.size(); i++){
        if(games[i].room.getName() == player.getRoom()){
            game = &(games[i]);
        }
    }

    if(x == 1){
        temp = "3_";
        temp = (temp + player.getAvaible_letters()).append("_");
    }else{
        for(int i=0; i<7; i++){
            char letter = game -> takeLetter();
            temp = (temp + letter).append("_");
        }
    }

    if(!sendStringToClient(desc,temp)){
        std::cout << "Couldn't send letters" << std::endl;
    }else{
        std::cout << "Letters send" << std::endl;
    }
}

void Server::sendPlayersFromCurrentRoom(int desc, Player &player, int x) {
    std::string message ="";
    std::string temp= "";

    int foundUsers = 0;
    int z=0;
    for(; z<rooms.size(); z++){
        if(rooms[z].getName() == player.getRoom()){
            break;
        }
    }

    for(int i=0; i<rooms[z].players.size(); i++){
        if(rooms[z].players[i].getRoom() == player.getRoom() && rooms[z].players[i].getUsername() != player.getUsername()){
            foundUsers++;
            message.append(rooms[z].players[i].getUsername()).append("_").append(rooms[z].players[i].getScore()).append("_");
        }
    }

    if(message.empty()){
        if(x == 1){
            message = "1";
        }else{
            message = "0";
        }
        temp.append(message).append("_").append(std::to_string(foundUsers)).append("_");
    }else{
        if(x == 1){
            temp = temp.append("1_").append(std::to_string(foundUsers)).append("_").append(message).append("_");
        }else{
            temp = std::to_string(foundUsers).append("_").append(message).append("_");
        }
    }

    while(temp.length() < 104){
        temp.append("0");
    }

    const char *cstr = temp.c_str();
    std::cout<<cstr<<std::endl;

    if(write(desc, cstr, strlen(cstr))<0){
        printf("%d", errno);
        std::cout<<"Couldn't send players"<<std::endl;
        pthread_exit(NULL);
    }else{
        std::cout<<"Players send"<<std::endl;
    }
}

void Server::getNewLetters(Player &player) {
    Game * game;
    int index;

    for(int i = 0; i < games.size(); i++){
        if(games[i].room.getName() == player.getRoom()){
            game = &(games[i]);
        }
    }

    while((index = player.getAvaible_letters().find("0")) != -1) {
        std::string letters = player.getAvaible_letters();
        letters[index] = game -> takeLetter();
        player.setAvaible_letters(letters);
    }
}

void Server::receiveUserMove(int desc, Player &player){
    char buffer[488];
    int x = 0;
    if((x = read(desc, buffer, sizeof(buffer))) < 0){
        if(errno == EWOULDBLOCK){
            receiveUserMove(desc, player);
        }else {
            perror("Couldn't receive move");
            printf("%d", errno);
        }
    }else if(x!=0){
        std::stringstream message(buffer);
        std::string segment;
        std::vector<std::string> seglist;

        while(std::getline(message, segment, '_')){
            seglist.push_back(segment);
        }

        std::string player_name = seglist[0];
        player.setScore(seglist[1]);
        player.setAvaible_letters(seglist[2]);
        getNewLetters(player);
        player.setTurn(false);

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

        z=0;
        for(; z<rooms.size(); z++){
            if(rooms[z].getName() == player.getRoom()){
                break;
            }
        }
        if(rooms[z].players.size()>1){
            for(int i=0; i<rooms[z].players.size(); i++){
                if(rooms[z].players[i].getUsername() == player.getUsername()){
                    rooms[z].players[i].setTurn(false);
                    rooms[z].players[i].setScore(seglist[1]);
                    rooms[z].players[i].setAvaible_letters(seglist[2]);

                    if(i+1<rooms[z].players.size()){
                        rooms[z].players[i+1].setTurn(true);
                    }else{
                        rooms[z].players[0].setTurn(true);
                    }
                }
            }
        }else{
            for(int i=0; i<rooms[z].players.size(); i++){
                if(rooms[z].players[i].getUsername() == player.getUsername()){
                    rooms[z].players[i].setTurn(true);
                }
            }
            player.setTurn(true);
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

    for(int i = 0; i<rooms[z].players.size(); i++) {
        if (rooms[z].players[i].getUsername() != player.getUsername() && rooms[z].players.size() > 1) {
            sendBoard(rooms[z].players[i].getSocket_desc(), player, 2);
            std::cout << "Send new board to: " << rooms[z].players[i].getUsername() << std::endl;
        }else{
            sendAvaibleLetters(desc, player, 1);
            sendBoard(rooms[z].players[i].getSocket_desc(),rooms[z].players[i],2);
        }
    }
}

void Server::swapLetters(int desc, Player &player) {
    char buffer[9];
    int x = 0;
    Game *game;

    for (int i = 0; i < games.size(); i++) {
        if (games[i].room.getName() == player.getRoom()) {
            game = &(games[i]);
        }
    }

    if ((x = read(desc, buffer, sizeof(buffer))) < 0) {
        perror("Couldn't receive user letters");
        printf("%d", errno);
    } else if (x != 0) {
        std::stringstream message(buffer);
        std::string segment;

        while (std::getline(message, segment)) {}
        segment.pop_back();

        letters_mutex.lock();
        for (int i = 0; i < 7; i++) {
            char tmp = segment[i];
            if (tmp != '0') {
                segment[i] = game->takeLetter();
                game->insertLetter(tmp);
            }
        }
        player.setAvaible_letters(segment);
        letters_mutex.unlock();

        std::string prefix = "3_";
        sendStringToClient(desc, prefix.append(segment));
    }
}

void Server::quitRoom(int desc, Player &player) {
    player.setAvaible_letters("");
    player.setScore("0");
    player.setTurn(false);

    Game * game;
    for (int i = 0; i < games.size(); i++){
        if(games[i].room.getName() == player.getRoom()){
            game = &(games[i]);
        }
    }

    if (game -> room.players.size() == 1) {
        game -> clear();
        game -> room.setFreeSlots(4);
    }


    for (int i = 0; i < game -> room.players.size(); i++) {
        if (game->room.players.at(i).getUsername() == player.getUsername()) {
            auto start = game -> room.players.begin();
            game -> room.players.erase(start + i);
            game -> room.setFreeSlots(game -> room.getFreeSlots() + 1);
        }
    }

    int z = 0;
    for (; z< rooms.size(); z++){
        if(rooms[z].getName() == player.getRoom()){
            break;
        }
    }

    if (rooms[z].players.size() == 1) {
        game -> clear();
        game -> room.setFreeSlots(4);
        rooms[z].players.clear();
        rooms[z].setFreeSlots(4);
    }

    for(int i = 0; i<rooms[z].players.size(); i++) {
        if (rooms[z].players[i].getUsername() == player.getUsername()) {
            rooms[z].players[i].setTurn(false);

            auto start = rooms[z].players.begin();
            rooms[z].players.erase(start + i);
            rooms[z].setFreeSlots(rooms[z].getFreeSlots() + 1);
        }
    }

    for(int i=0; i<rooms[z].players.size(); i++){
        std::cout<<rooms[z].players[i].getUsername()<<std::endl;
        if(rooms[z].players[i].getUsername() != player.getUsername()){
            sendPlayersFromCurrentRoom(rooms[z].players[i].getSocket_desc(), rooms[z].players[i], 1);
            std::cout<<"WYKONALEM SIE" << std::endl;
        }
    }

    std::cout << "Quitting room " << player.getRoom() << std::endl;
}








