#include <iostream>
#include <vector>

#include "Force2D.hpp"


using namespace std;


int main()
{
    cout << "TEST" << endl;

    /*
    Force2D::Scene* scene = new Force2D::Scene();
    //Force2D::GameObject* obj = scene->CreateGameObject({.x=10, .y=10}, {.x=100, .y=100});
    Force2D::GameObject* obj = new Force2D::GameObject({.x=10, .y=10}, {.x=100, .y=100});
    scene->AddGameObject(obj);
    Force2D::BoxCollider* collider = new Force2D::BoxCollider( (Force2D::Vector2){.x=0, .y=0}, (Force2D::Vector2){.x=50, .y=50} );
    int ind = collider->AddOnMouseOverHandler([]() {
        cout << "TEST mouseover " << endl;
    });
    cout << "IND " << ind << endl;
    obj->AddComponent( collider );

    Force2D::GameObject* obj2 = new Force2D::GameObject({.x=30, .y=30}, {.x=100, .y=100});
    scene->AddGameObject(obj2);
    Force2D::BoxCollider* collider2 = new Force2D::BoxCollider( (Force2D::Vector2){.x=0, .y=0}, (Force2D::Vector2){.x=50, .y=50} );
    int ind2 = collider2->AddOnMouseOverHandler([]() {
        cout << "TEST mouseover 2 " << endl;
    });
    cout << "IND2 " << ind2 << endl;
    obj2->AddComponent( collider2 );

    Force2D::GameObject* closeScene = new Force2D::GameObject({.x=200, .y=200}, {.x=100, .y=100}, 5);
    scene->AddGameObject(closeScene);
    Force2D::BoxCollider* closeScene_collider = new Force2D::BoxCollider( (Force2D::Vector2){.x=0, .y=0}, (Force2D::Vector2){.x=100, .y=100} );
    closeScene_collider->AddOnMouseOverHandler([scene]() {
        if (scene->IsMouseDown()) {
            scene->StopRender();
        }
    });
    closeScene->AddComponent( closeScene_collider );

    Force2D::GameObject* test = new Force2D::GameObject({.x=200-10, .y=200-10}, {.x=80, .y=80}, 4);
    scene->AddGameObject(test);
    test->color = {.x=15, .y=15, .z=15, .w=255};
    auto test_bx = new Force2D::BoxCollider({.x = 0, .y = 0}, {.x = 80, .y = 80});
    test_bx->AddOnMouseOverHandler([scene, test]() {
        if (scene->IsMouseDown()) {
            test->layer = (test->layer == 4) ? 6 : 4;
        }
    });
    test->AddComponent(test_bx);


    Force2D::debug = true;
    scene->StartRender(renderer);

    cout << "!!!" <<    endl;


    auto second_scene = new Force2D::Scene();
    auto ss_obj_1 = new Force2D::GameObject({100, 50}, {200, 400});

    second_scene->AddGameObject(ss_obj_1);

    auto ss_obj_1__obj1 = new Force2D::GameObject({10, 10}, {30, 30});
    ss_obj_1->AddChild(ss_obj_1__obj1);


    second_scene->StartRender(renderer);
    */

    using namespace Force2D;

    initSDL("GAME");

    auto scene = new Scene();

    auto obj1 = new GameObject({10, 10}, {100, 100});
    scene->AddGameObject(obj1);

    auto obj2 = new GameObject({60, 60}, {100, 100});
    obj2->SetTexture("./test_tex.png");
    scene->AddGameObject(obj2);
  
    //obj2->velocity = {.x = 5, .y = 3};

    scene->Update = [obj2](Force2D::Time time) {
        if (Force2D::KEY_PRESSED[SDLK_SPACE]) {
            obj2->velocity = {.x = 5, .y = 3};
        }
        else
        {
            obj2->velocity = {.x = 0, .y = 0};
        }
    };

    scene->StartRender();
   
    quitSDL();

    return 0;
}


