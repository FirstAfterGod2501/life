#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <thread>


struct Animal{
    Animal(int x,int y, std::string typeOfAnimal){
        this->x = x;
        this->y = y;
        this->typeOfAnimal = std::move(typeOfAnimal);
        this->food = 15;
    }
    int x{};
    int y{};
    std::string typeOfAnimal;
    int food = 15;
    int age{};
    int minReproductiveAge{};
    int maxReproductiveAge{};
    int foodPerTick{};

    bool operator== (const Animal& animal1) noexcept
    {
        return animal1.x == x && animal1.y == y;
    }

    bool operator!= (const Animal& animal1) noexcept
    {
        return animal1.x != x && animal1.y != y;
    }

};

struct Environment{
    Environment(int gridSize,int grassRecoveryPercentage){
        this->gridSize = gridSize;
        this->grassRecoveryPercentage = grassRecoveryPercentage;
    }

    int gridSize;
    std::vector<std::vector<char>> grid;
    int grassRecoveryPercentage;
    std::vector<Animal> animals;
};


bool food(Environment &Env, Animal &animal) {
    if (animal.typeOfAnimal == "predator") {
        auto iter = std::find_if(Env.animals.begin(), Env.animals.end(), [&](Animal &predator) {
            int x = abs(predator.y - animal.y);
            int y = abs(predator.x - animal.x);
            return (predator.x > 0 && predator.y < Env.gridSize && predator.y > 0 && predator.x < Env.gridSize)&&(x!=y && x<=1 && y<=1);
        });
        if(iter != Env.animals.end()) {
            Env.animals.erase(iter);
            Env.grid[iter->x][iter->y] = 'p';
            Env.grid[animal.x][animal.y] = '0';
            animal.x = iter->x;
            animal.y = iter->y;
            return true;
        }
    }
    else {
        int x{},y{};
        for (int i = animal.x-1; i < animal.x+1; ++i) {
            for (int j = animal.y-1; j < animal.y+1; ++j) {
                if((i+j > 0) && Env.grid[i][j] == '1' && (i != j)){
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


void createGrid(Environment &Env){
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


void createNewAnimal(Environment &Env, const std::string& typeOfAnimal) {
    std::random_device device{};
    std::mt19937 engine {device()};
    std::uniform_int_distribution<int> distribution {1,Env.gridSize-1};
    int coord_x = distribution(engine);
    int coord_y = distribution(engine);
    if(Env.grid[coord_x][coord_y] == '0' || Env.grid[coord_x][coord_y] == '1') {
        Env.animals.emplace_back(coord_x,coord_y,typeOfAnimal);
        Env.grid[coord_x][coord_y] = typeOfAnimal[0];
    }
    else{
        for (int i = 0; i < Env.gridSize; ++i) {
            for (int j = 0; j < Env.gridSize; ++j) {
                if(Env.grid[i][j] == '0' || Env.grid[i][j] == '1'){
                    Env.animals.emplace_back(i,j,typeOfAnimal);
                    Env.grid[coord_x][coord_y] = typeOfAnimal[0];
                    return;
                }
            }
        }
    }
}


void createStartAnimals(Environment &Env, int countOfStartPredators, int countOfStartHerbivores){
    for (int i = 0; i < countOfStartPredators; ++i) {
        createNewAnimal(Env, "predator");
    }
    for (int i = 0; i < countOfStartHerbivores; ++i) {
        createNewAnimal(Env, "herbivore");
    }
}

void drawFrame(Environment &Env,int tiks) {
    system("clear");
    for (int i = 0; i < Env.gridSize; ++i) {
        for (int j = 0; j < Env.gridSize; ++j) {
            std::cout << Env.grid[i][j];
        }
        std::cout << std::endl;
    }
    std::cout << "predators " << std::count_if(Env.animals.begin(), Env.animals.end(),
                                               [](Animal &animal) { return animal.typeOfAnimal == "predator"; })
              << "\n";

    std::cout << "herbivores " << std::count_if(Env.animals.begin(), Env.animals.end(),
                                                [](Animal &animal) { return animal.typeOfAnimal == "herbivore"; })
              << "\n";
    std::cout<<"tiks: "<<tiks<<"\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void removeAnimal(Environment &Env, Animal& animal){
    Env.animals.erase(std::find(Env.animals.begin(),Env.animals.end(),animal));
    Env.grid[animal.x][animal.y] = '0';
}

void moveAnimal(Environment &Env, Animal &animal, int x, int y){
    if(x>0 && y>0 && x<Env.gridSize && y<Env.gridSize && Env.grid[x][y] != 'p' && Env.grid[x][y] != 'h'){
        Env.grid[x][y] = animal.typeOfAnimal[0];
        animal.x = x;
        animal.y = y;
    }
}

void deleteDiedAnimals(Environment &Env){
    for (int i = 0; i < Env.gridSize; ++i) {
        for (int j = 0; j < Env.gridSize; ++j) {
            if (!std::any_of(Env.animals.begin(), Env.animals.end(),
                             [&](Animal &animal) { return animal.x == i && animal.y == j; }) && Env.grid[i][j] != '1') {
                Env.grid[i][j] = '0';
            }
        }
    }
}
void recoverFood(Environment &Env) {
    std::random_device device{};
    std::mt19937 engine{device()};
    std::uniform_int_distribution<int> distribution{0, Env.grassRecoveryPercentage};
    for (int i = 0; i < Env.gridSize; i++) {
        for (int j = 0; j < Env.gridSize; j++) {
            if (Env.grid[i][j] == '0' && distribution(engine) == 0) {
                Env.grid[i][j] = '1';
            }
        }
    }
}

/*
 * 012
 * 7x3
 * 654
 */

void move(Environment &env, Animal & animal){
    std::random_device device{};
    std::mt19937 engine {device()};
    std::uniform_int_distribution<int> distribution {0,7};
    switch (distribution(engine)) {
        case 0:
            moveAnimal(env,animal,animal.x-1,animal.y-1);
            break;
        case 1:
            moveAnimal(env,animal,animal.x,animal.y-1);
            break;
        case 2:
            moveAnimal(env,animal,animal.x+1,animal.y-1);
            break;
        case 3:
            moveAnimal(env,animal,animal.x+1,animal.y);
            break;
        case 4:
            moveAnimal(env,animal,animal.x-1,animal.y);
            break;
        case 5:
            moveAnimal(env,animal,animal.x+1,animal.y+1);
            break;
        case 6:
            moveAnimal(env,animal,animal.x+1,animal.y);
            break;
        case 7:
            moveAnimal(env,animal,animal.x-1,animal.y+1);
            break;

    }
}

void tick(Environment &Env,int tick) {
    for (auto &animal: Env.animals) {
        animal.age++;
        if (animal.food == 0 && animal.age > animal.maxReproductiveAge) {
            removeAnimal(Env, animal);
        }
        if (food(Env, animal)) {
            animal.food++;
        } else {
            animal.food--;
            move(Env, animal);
        }
        if (animal.food > 20) {
            createNewAnimal(Env, animal.typeOfAnimal);
            animal.food -= 20;
        }
        if(tick%25 ==0) {
            recoverFood(Env);
        }
    }
}

int main(){
    int size,predators{},herbivores{},grassRecoveryPercentage{};
    std::cin>>size;
    std::cin>>predators;
    std::cin>>herbivores;
    std::cin>>grassRecoveryPercentage;//1/grassRecoveryPercentage = final grassRecoveryPercentage
    Environment Env(size, grassRecoveryPercentage);
    createGrid(Env);
    createStartAnimals(Env,predators,herbivores);
    int tiks =0;
    while (true){
        tiks++;
        tick(Env,tiks);
        std::cout<<tiks << "\n";
        drawFrame(Env,tiks);
        deleteDiedAnimals(Env);
    }
}