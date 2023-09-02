#include <iostream>
#include <list>
#include <vector>
#include <cmath>
#include <cstring>
#include <string>

using namespace std;

const string wall_tile = "##";//alt178
const string floor_tile = "__";//alt177
const string player_tile = "PL";//alt1
const string ball_tile = "()";//alt7
const string finish_tile = "  ";//alt8

const int random_seed = 1442968193;
int random_step = 0;
int random(int min_value, int max_value){
    int divide = 256 + random_step;
    int number = random_seed % divide;
    number = number % (max_value - min_value + 1);
    random_step++;
    return round(min_value + number);
}

vector<int> map_size = {15, 10};
bool room_border = true;
vector<int> start_player_pos = {0, 0};
vector<int> player_pos = {};
list <vector<int>> start_wall_pos = {{5, 5}, {10, 5}};
list <vector<int>> start_ball_pos = {{5, 6}, {10, 6}};
list <vector<int>> start_finish_pos = {{5, 7}, {10, 7}};
list <vector<int>> wall_pos = {};
list <vector<int>> ball_pos = {};
list <vector<int>> finish_pos = {};

void print(string text){
    cout << text << endl;
}

string tile_type(int x, int y){
    for(vector<int> i : wall_pos){
        if(x == i[0] && y == i[1]){
            return wall_tile;
        }
    }
    for(vector<int> i : ball_pos){
        if(x == i[0] && y == i[1]){
            return ball_tile;
        }
    }
    for(vector<int> i : finish_pos){
        if(x == i[0] && y == i[1]){
            return finish_tile;
        }
    }
    return floor_tile;
}

string show_room(){
    string room = "\n";

    if (room_border){
        for (int i = 0; i <= map_size[0]+2; i++){
            room += wall_tile;
        }
        room += "\n";
    }
    for (int y = 0; y <= map_size[1]; y++){
        if (room_border){
            room += wall_tile;
        }
        for (int x = 0; x <= map_size[0]; x++){
            if (player_pos[0] == x && player_pos[1] == y){
                room += player_tile;
            }
            else{
                room += tile_type(x, y);
            }
        }
        if (room_border){
            room += wall_tile;
        }
        room += "\n";
    }
    if (room_border){
        for (int i = 0; i <= map_size[0]+2; i++){
            room += wall_tile;
        }
        room += "\n";
    }

    return room;
}

void remove_balls(){
    for (vector<int> ball : ball_pos){
        for (vector<int> finish : finish_pos){
            if (ball == finish){
                list<vector<int>> new_ball_pos = {};
                list<vector<int>> new_finish_pos = {};
                for (vector<int> i : ball_pos){
                    if (i != ball){
                        new_ball_pos.push_back(i);
                    }
                }
                for (vector<int> i : finish_pos){
                    if (i != finish){
                        new_finish_pos.push_back(i);
                    }
                }
                ball_pos = new_ball_pos;
                finish_pos = new_finish_pos;
                return;
            }
        }
    }
}

void move_player(int x, int y){
    if(x != 0){x = x / abs(x);}
    if(y != 0){y = y / abs(y);}
    vector<int> move_to = {player_pos[0] + x, player_pos[1] + y};
    for (vector<int> i : wall_pos){
        if (i == move_to){
            return;
        }
    }
    if (move_to[0] < 0 || move_to[0] > map_size[0] || move_to[1] < 0 || move_to[1] > map_size[1]){
        return;
    }
    for (vector<int> a : ball_pos){
        if (a == move_to){
            vector<int> roll_to = {move_to[0] + x, move_to[1] + y};
            for (vector<int> b : wall_pos){
                if (b == roll_to){
                    return;
                }
            }
            for (vector<int> b : ball_pos){
                if (b == roll_to){
                    return;
                }
            }
            if (roll_to[0] < 0 || roll_to[0] > map_size[0] || roll_to[1] < 0 || roll_to[1] > map_size[1]){
                return;
            }
            list<vector<int>> new_ball_pos;
            for (vector<int> b : ball_pos){
                if (a == b){
                    new_ball_pos.push_back(roll_to);
                }
                else{
                    new_ball_pos.push_back(b);
                }
            }
            ball_pos = new_ball_pos;
        }
    }
    player_pos = move_to;
    remove_balls();
}

void reset(){
    player_pos = start_player_pos;
    wall_pos = start_wall_pos;
    ball_pos = start_ball_pos;
    finish_pos = start_finish_pos;
}

void new_map(){
    vector<int> new_player_pos = {};
    list <vector<int>> new_wall_pos = {};
    list <vector<int>> new_ball_pos = {};
    list <vector<int>> new_finish_pos = {};
    int ball_amount = round(pow(map_size[0]*map_size[1], .25)) - 1;
    int wall_amount = (round(pow(map_size[0]*map_size[1], .25)) - 1) * 5;
    int line_amount = round(pow(map_size[0]*map_size[1], .2)) - 1;
    new_player_pos = {random(0, map_size[0]-1), random(0, map_size[1]-1)};

    for (int a = 0; a < random(0, line_amount); a++){
        int rotation = random(0, 1);
        if (rotation == 0){
            vector<int> line = {random(0, map_size[1]), random(0, map_size[0]), random(0, map_size[0])};
            for (int b = 0; b <= abs(line[1] - line[2]); b++){
                bool can_place = true;
                for (int x = -1; x <= 1; x++){
                    for (int y = -1; y <= 1; y++){
                        vector<int> c = {b+min(line[1], line[2]) + x, line[0] + y};
                        if(c == player_pos){
                            can_place = false;
                        }
                    }
                }
                for (vector<int> wall : new_wall_pos){
                    vector<int> c = {b+min(line[1], line[2]), line[0]};
                    if (c == wall){
                        can_place = false;
                        break;
                    }
                }
                if (can_place){
                    new_wall_pos.push_back({b+min(line[1], line[2]), line[0]});
                }
            }
        }
        else{
            vector<int> line = {random(0, map_size[0]), random(0, map_size[1]), random(0, map_size[1])};
            for (int b = 0; b <= abs(line[1] - line[2]); b++){
                bool can_place = true;
                for (int x = -1; x <= 1; x++){
                    for (int y = -1; y <= 1; y++){
                        vector<int> c = {line[0] + y, b+min(line[1], line[2]) + x};
                        if(c == player_pos){
                            can_place = false;
                        }
                    }
                }
                for (vector<int> wall : new_wall_pos){
                    vector<int> c = {line[0], b+min(line[1], line[2])};
                    if (c == wall){
                        can_place = false;
                        break;
                    }
                }
                if (can_place){
                    new_wall_pos.push_back({line[0], b+min(line[1], line[2])});
                }
            }
        }
    }

    for (int a = 0; a < random(0, wall_amount); a++){
        bool free_spot = false;
        vector<int> pos = {};
        while (!free_spot){
            pos = {random(0, map_size[0]-1), random(0, map_size[1]-1)};
            free_spot = true;
            for (vector<int> wall : new_wall_pos){
                if (pos == wall){
                    free_spot = false;
                }
            }
        }
        new_wall_pos.push_back(pos);
    }

    for (int a = 0; a < random(1, ball_amount); a++){
        bool free_ball_spot = false;
        vector<int> ball_pos = {};
        while (!free_ball_spot){
            ball_pos = {random(1, map_size[0]-2), random(1, map_size[1]-2)};
            free_ball_spot = true;
            for (int x = -1; x <= 1; x++){
                for (int y = -1; y <= 1; y++){
                    for (vector<int> wall : new_wall_pos){
                        if (ball_pos == wall){
                            free_ball_spot = false;
                        }
                    }
                }
            }
        }
        new_ball_pos.push_back(ball_pos);

        bool free_finish_spot = false;
        vector<int> finish_pos = {};
        while (!free_finish_spot){
            finish_pos = {random(1, map_size[0]-2), random(1, map_size[1]-2)};
            free_finish_spot = true;
            for (int x = -1; x <= 1; x++){
                for (int y = -1; y <=1; y++){
                    for (vector<int> wall : new_wall_pos){
                        if (finish_pos == wall){
                            free_finish_spot = false;
                        }
                    }
                }
            }
        }
        new_finish_pos.push_back(finish_pos);
    }

    start_player_pos = new_player_pos;
    start_wall_pos = new_wall_pos;
    start_ball_pos = new_ball_pos;
    start_finish_pos = new_finish_pos;
    reset();
}

void execute_player_input(string input){
    char char_input[input.length() + 1];
    strcpy(char_input, input.c_str());

    if (input == "restart"){
        reset();
        print("restarted");
        return;
    }
    if (input == "resize"){
        int x;
        int y;
        cout << "x: ";
        cin >> x;
        cout << "y: ";
        cin >> y;
        map_size = {min(max(x, 12), 55), min(max(y, 8), 20)};
        new_map();
        reset();
        cout << endl << "resized to: " << map_size[0] << ", " << map_size[1] << endl;
        return;
    }
    if (input == "giveup"){
        new_map();
        print("was it really that impossible?");
        return;
    }
    if (input == "help"){
        print("w/a/s/d to move (you can use multiple in one input)");
        print("\"quit\" to leave");
        print("\"restart\" to restar");
        print("\"giveup\" to to generate a new map");
        print("\"resize\" to to generate a new map with different size");
        print("\"help\" to show this");
    }


    for (char a : char_input){
        if (tolower(a) == 'w'){
            move_player(0, -1);
            continue;
        }
        if (tolower(a) == 'a'){
            move_player(-1, 0);
            continue;
        }
        if (tolower(a) == 's'){
            move_player(0, 1);
            continue;
        }
        if (tolower(a) == 'd'){
            move_player(1, 0);
            continue;
        }
        return;
    }
}

int main()
{
    string player_input = "";
    bool game_over = false;
    while (!game_over){
        bool is_finished = false;
        reset();
        while (!is_finished){
            print(show_room());
            cin >> player_input;
            if (player_input == "quit"){
                game_over = true;
                is_finished = true;
            }
            execute_player_input(player_input);
            if (ball_pos.size() == 0 || finish_pos.size() == 0){
                is_finished = true;
            }
        }
        if (!game_over){
            print(show_room());
            print("this room is finished");
            new_map();
        }
    }
    return 0;
}
