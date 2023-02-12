#include <iostream>
#include <vector>

#include "Force2D.hpp"


using namespace std;


int main()
{
    cout << "TEST" << endl;
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0){
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Test game", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
    if (window == nullptr){
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr){
        SDL_DestroyWindow(window);
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    Force2D::Scene* scene = new Force2D::Scene();
    Force2D::GameObject* obj = scene->CreateGameObject({.x=10, .y=10}, {.x=100, .y=100});
    Force2D::BoxCollider* collider = new Force2D::BoxCollider( (Force2D::Vector2){.x=0, .y=0}, (Force2D::Vector2){.x=50, .y=50} );
    int ind = collider->AddOnMouseOverHandler([]() {
        cout << "TEST mouseover " << endl;
    });
    cout << "IND " << ind << endl;
    obj->AddComponent( collider );

    Force2D::GameObject* obj2 = scene->CreateGameObject({.x=30, .y=30}, {.x=100, .y=100});
    Force2D::BoxCollider* collider2 = new Force2D::BoxCollider( (Force2D::Vector2){.x=0, .y=0}, (Force2D::Vector2){.x=50, .y=50} );
    int ind2 = collider2->AddOnMouseOverHandler([]() {
        cout << "TEST mouseover 2 " << endl;
    });
    cout << "IND2 " << ind2 << endl;
    obj2->AddComponent( collider2 );


    Force2D::debug = true;
    scene->StartRender(renderer);


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}


