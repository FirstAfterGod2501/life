#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <random>
#include <algorithm>


struct Animal{
    Animal(int x, int y, std::string&& typeOfAnimal) noexcept : x(x), y(y), typeOfAnimal(std::move(typeOfAnimal)) {}

    int x{};
    int y{};
    std::string typeOfAnimal;

    int food = 15;

    int age{};
    int minReproductiveAge{};
    int maxReproductiveAge{};

    int foodPerTick{};
};

struct Enviroment{
    Enviroment(int gridSize,int grassRecoveryPercentage, int cataclysmPercentage):
    gridSize(gridSize),
    grassRecoveryPercentage(grassRecoveryPercentage),
    cataclysmPercentage(cataclysmPercentage){}

    int gridSize;
    std::vector<std::vector<char>> grid;
    int grassRecoveryPercentage;
    int cataclysmPercentage;
    std::vector<Animal> animals;
};


bool food(Enviroment &Env, Animal &animal) {
    if (animal.typeOfAnimal == "predator") {
        auto iter = std::find_if(Env.animals.begin(), Env.animals.end(), [&](Animal &predator) {
            int x = abs(predator.y - animal.y);
            int y = abs(predator.x - animal.x);
            return (predator.x > 0 && predator.y < Env.gridSize && predator.y > 0 && predator.x < Env.gridSize)&&(x!=y && x<=1 && y<=1);
        });
        if(iter != Env.animals.end()) {
            Env.animals.erase(iter);
            Env.grid[iter->x][iter->y] = 'p';
            animal.x = iter->x;
            animal.y = iter->y;
            return true;
        }
    }
    else {
        int x{},y{};
        for (int i = animal.x-1; i < animal.x+1; ++i) {
            for (int j = animal.y-1; j < animal.y+1; ++j) {
                if(Env.grid[i][j] == '1' && (i != j)){
                    x = i;
                    y = j;
                }
            }
        }
        if(x != 0 && y != 0) {
            Env.grid[x][y] = 'h';
            Env.grid[animal.x][animal.y] = '0';
            animal.x = x;
            animal.y = y;
            return true;
        }
    }
    return false;
}


void createGrid(Enviroment &Env){
    std::random_device device{};
    std::mt19937 engine {device()};
    std::uniform_int_distribution<int> distribution {0,1};
       for(int i = 0; i< Env.gridSize; ++i){
           std::vector<char> tmpVec{};
           for (int j = 0; j < Env.gridSize; ++j) {
                tmpVec.push_back(char(distribution(engine)) + '0');
           }
           Env.grid.push_back(tmpVec);
       }
}


void createNewAnimal(Enviroment &Env,std::string& typeOfAnimal) {
    std::random_device device{};
    std::mt19937 engine {device()};
    std::uniform_int_distribution<int> distribution {1,Env.gridSize-1};
    int coord_x = distribution(engine);
    int coord_y = distribution(engine);
    if(Env.grid[coord_x][coord_y] == '0' || Env.grid[coord_x][coord_y] == '1') {
        Env.animals.emplace_back(coord_x,coord_y,std::move(typeOfAnimal));
        Env.grid[coord_x][coord_y] = (Env.animals.end()-1)->typeOfAnimal[0];
    }
    else{
        for (int i = 0; i < Env.gridSize; ++i) {
            for (int j = 0; j < Env.gridSize; ++j) {
                if(Env.grid[i][j] == '0' || Env.grid[i][j] == '1'){
                    Env.animals.emplace_back(i,j,std::move(typeOfAnimal));
                    Env.grid[coord_x][coord_y] = (Env.animals.end()-1)->typeOfAnimal[0];
                    return;
                }
            }
        }
    }
}


void createStartAnimals(Enviroment &Env, int countOfStartPredators,int countOfStartHerbivores){
    for (int i = 0; i < countOfStartPredators; ++i) {
        createNewAnimal(Env, (std::string &) "predator");
    }
    for (int i = 0; i < countOfStartHerbivores; ++i) {
        createNewAnimal(Env, (std::string &) "herbivore");
    }
}

void recoverGrid(Enviroment &Env){

}

void drawGrid(Enviroment &Env){
    for (int i = 0; i < Env.gridSize; ++i) {
        for (int j = 0; j < Env.gridSize; ++j) {
            std::cout<<Env.grid[i][j];
        }
        std::cout<<std::endl;
    }
}

void tick(Enviroment &Env){
    for (auto &animal : Env.animals) {
        animal.age++;
        if(food(Env,animal)){
            animal.food++;
        }
        else{
            animal.food--;
        }
        if(animal.food > 20){
            createNewAnimal(Env,animal.typeOfAnimal);
            animal.food-=20;
        }
    }
    drawGrid(Env);
}

int main(){
    int size;
    std::cin>>size;
    Enviroment Env(size,0,0);
    createGrid(Env);
    createStartAnimals(Env,10,10);
    while (true){
        tick(Env);
    }
}