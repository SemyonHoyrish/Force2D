#ifndef FORCE_2D_HPP
#define FORCE_2D_HPP

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <functional> //std::function
#include <string> //std::to_string
// #include <iostream> 


namespace Force2D
{

bool debug = false;

void LogInfo(std::string message)
{
    std::cout << "[INFO ] " + message << std::endl;
}
void LogError(std::string message)
{
    std::cout << "[ERROR] " + message << std::endl;
}
void LogError(std::string message, std::string subinfo)
{
    std::cout << "[ERROR] [" + subinfo + "]" + message << std::endl;
}
void LogFatal(std::string message)
{
    std::cout << "[FATAL] " + message << std::endl;
    exit(1);
}
void LogFatal(std::string message, std::string subinfo)
{
    std::cout << "[FATAL] [" + subinfo + "]" + message << std::endl;
    exit(1);
}

SDL_Window *mainWindow = NULL;
SDL_Renderer *mainWindowRenderer = NULL;

void initSDL(std::string mainWindowTitle, int mainWindowX = 100, int mainWindowY = 100, int mainWindowWidth = 640, int mainWindowHeight = 480)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0){
        LogFatal((std::string)"SDL_Init Error: " + SDL_GetError());
    }

    mainWindow = SDL_CreateWindow(mainWindowTitle.c_str(), mainWindowX, mainWindowY, mainWindowWidth, mainWindowHeight, SDL_WINDOW_SHOWN);
    if (mainWindow == nullptr){
        LogFatal((std::string)"SDL_CreateWindow Error: " + SDL_GetError());
        SDL_Quit();
    }

    mainWindowRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (mainWindowRenderer == nullptr){
        SDL_DestroyWindow(mainWindow);
        LogFatal((std::string)"SDL_CreateRenderer Error: " + SDL_GetError());
        SDL_Quit();
    }
}
void quitSDL()
{
    SDL_DestroyRenderer(mainWindowRenderer);
    SDL_DestroyWindow(mainWindow);
    SDL_Quit();
}

struct Vector2
{
    int x, y;

    Vector2 operator+(Vector2 other)
    {
        return {.x = x + other.x, .y = y + other.y};
    }    
};

struct Vector4
{
    int x, y, z, w;

    Vector4 operator+(Vector4 other)
    {
        return {.x = x + other.x, .y = y + other.y,
                .z = z + other.z, .w = w + other.w};
    }    
};

// struct GameObject
// {
//     int x, y;
//     int w, h;
//     Vector2 velocity; 
//     bool selected;
// };

struct Time
{
    Uint64 previous  = 0;
    Uint64 now       = 0;
    double deltaTime = 0;

    void next(Uint64 next, Uint64 frequency)
    {
        previous = now;
        now = next;
        deltaTime = (double)((now - previous)*1000 / (double)frequency);
    }
};

class Component
{
public:
    std::string name;
    Vector2 relativePosition;
    Vector2* parentPosition;
    //Vector2 position;
    Vector2 size;
    int* parentLayer;
    // const ComponentType type = ComponentType::BOX_COLLIDER;

    Component(Vector2 _relativePosition, Vector2 _size)
    {
        relativePosition = _relativePosition;
        size     = _size;
    }
    Component(Vector2 _relativePosition, Vector2 _size, std::string _name)
    {
        relativePosition = _relativePosition;
        size     = _size;
        name     = _name;
    }

    Vector2 GetPosition()
    {
        return (*parentPosition) + relativePosition;
    }

    int GetParentLayer()
    {
        return *parentLayer;
    }

    void SetParentData(Vector2* _parentPosition, int* _parentLayer)
    {
        parentPosition = _parentPosition;
        parentLayer    = _parentLayer;
    }

    virtual void Update(Time time)
    {
        LogError("Update call in Component");
    }

//private:
    //Vector2 parentPosition;

};

class BoxCollider : public Component
{

private:
    std::vector<std::pair<std::function<void()>, bool>> onMouseOverHandlers;
    static double onMouseOverHandlersPoolTime;
    static std::vector<std::pair<std::function<void()>, int*>> onMouseOverHandlersPool;


public:

    BoxCollider(Vector2 _position, Vector2 _size) : Component(_position, _size) {}
    BoxCollider(Vector2 _position, Vector2 _size, std::string _name) : Component(_position, _size, _name) {}

    int AddOnMouseOverHandler(std::function< void() > handler, bool ignoreLayer = false)
    {
        onMouseOverHandlers.push_back({handler, ignoreLayer});
        return onMouseOverHandlers.size() - 1;
    }
    
    void RemoveOnMouseOverHandler(int handler_index)
    {
        if (handler_index < 0 || handler_index >= onMouseOverHandlers.size())
            LogFatal("handler index out of bounds", "RemoveOnMouseOverHandler");

        onMouseOverHandlers[handler_index] = {NULL, false};
    }

    

    void Update(Time time) override
    {
        if (BoxCollider::onMouseOverHandlersPoolTime < time.now)
        {
            for (auto p : BoxCollider::onMouseOverHandlersPool)
                p.first();

            BoxCollider::onMouseOverHandlersPool.clear();
            BoxCollider::onMouseOverHandlersPoolTime = time.now;
        }
        // LogInfo("Update call in BoxCollider");
        int x, y;
        SDL_GetMouseState(&x, &y);

        if((x >= GetPosition().x && x <= GetPosition().x + size.x) && (y >= GetPosition().y && y <= GetPosition().y + size.y))
            for (auto p : onMouseOverHandlers)
            {
                if (p.first == NULL) continue;
                
                if (p.second)
                {
                    p.first();
                }
                else
                {
                    if (!BoxCollider::onMouseOverHandlersPool.empty() && *BoxCollider::onMouseOverHandlersPool[0].second > GetParentLayer()) continue;
                    if (!BoxCollider::onMouseOverHandlersPool.empty() && *BoxCollider::onMouseOverHandlersPool[0].second < GetParentLayer())
                        BoxCollider::onMouseOverHandlersPool.clear();

                    BoxCollider::onMouseOverHandlersPool.push_back({p.first, parentLayer});
                }
            }
    }

};
double Force2D::BoxCollider::onMouseOverHandlersPoolTime;
std::vector<std::pair<std::function<void()>, int*>> Force2D::BoxCollider::onMouseOverHandlersPool;


class GameObject
{
public:
    std::string name;
    Vector2 position;
    Vector2 size;
    Vector2 velocity;
    int layer;
    bool selected;
    Vector4 color;
    std::string texture_filename;
    SDL_Texture* texture = NULL;


    GameObject(Vector2 _position, Vector2 _size, int _layer = -1)
    {
        position = _position;
        size     = _size;
        layer    = _layer;
        color    = {.x = 122, .y = 122, .z = 122, .w = 255};
    }
    GameObject(Vector2 _position, Vector2 _size, std::string _name, int _layer = -1)
    {
        position = _position;
        size     = _size;
        name     = _name;
        layer    = _layer;
        color    = {.x = 122, .y = 122, .z = 122, .w = 255};
    }

    void SetTexture(std::string filename)
    { 
        texture_filename = filename;
    }

    void AddComponent(Component* component)
    {
        // component->SetParentPosition(position);
        component->SetParentData(&position, &layer);
        components.push_back(component);
    }
    
    template<class T>
    T* GetComponent()
    {
        for(Component* c : components)
            if (dynamic_cast<T*>(c) != nullptr) return (T*)c;
    }
    
    template<class T>
    T* GetComponent(const std::string name)
    {
        for(Component* c : components)
            if (dynamic_cast<T*>(c) != nullptr && c->name == name) return (T*)c;
    }
    
    template<class T>
    std::vector<T*> GetComponents()
    {
        std::vector<T*> result;
        for(Component* c : components)
            if (dynamic_cast<T*>(c) != nullptr) result.push_back((T*)c);
        
        return result;
    }


    void AddChild(GameObject* child)
    {
        childObjects.push_back(child);
    }

    GameObject* GetChild(std::string name)
    {
        for (GameObject* child : childObjects)
            if (child->name == name) return child;
    }

    std::vector<GameObject*> GetChildren()
    {
        std::vector<GameObject*> res;
        for (GameObject* child : childObjects)
            res.push_back(child);
        
        return res;
    }

    std::vector<GameObject*> GetChildren(std::string name)
    {
        std::vector<GameObject*> res;
        for (GameObject* child : childObjects)
            if (child->name == name)
                res.push_back(child);
        
        return res;
    }
    

    void Update(Time time)
    {
        for(Component* c : components)
        {
            c->Update(time);
        }
    }


    // bool CollidesWith(const GameObject* const gameObject)
    // {
    //     if((point.x >= obj.x && point.x <= obj.x + obj.w) &&
    //         (point.y >= obj.y && point.y <= obj.y + obj.h))
    //         return true;

    //     return false;
    // }

    // bool CollidesWith(const Vector2& point)
    // {
    //     if((point.x >= position.x && point.x <= position.x + size.x) &&
    //         (point.y >= position.y && point.y <= position.y + size.y))
    //         return true;

    //     return false;
    // }



    std::string ToString()
    {
        return "position.x: " + std::to_string(position.x) + " position.y: " + std::to_string(position.y) + 
            " size.x: " + std::to_string(size.x) + " size.y: " + std::to_string(size.y);
    }

private:
    std::vector<Component*> components;
    std::vector<GameObject*> childObjects;

};


class Scene
{
public:
    Time time;
    SDL_Event sdl_event;

    /*GameObject* CreateGameObject(Vector2 position, Vector2 size)
    {
        gameObjects.push_back(new GameObject(position, size));
        return gameObjects[gameObjects.size() - 1];
    }*/

    void AddGameObject(GameObject* obj)
    {
        if (obj->layer == -1)
            obj->layer = default_layer++;

        gameObjects.push_back(obj);
        //return gameObjects.size() - 1;
    }

    bool IsMouseDown()
    {
        return sdl_event.type == SDL_MOUSEBUTTONDOWN;
    }

    void StartRender(SDL_Renderer* _renderer = NULL)
    {
        if (app_quit) return;

        rendering = true;
        renderer = (_renderer != NULL) ? _renderer : mainWindowRenderer;

        //SDL_Event event;
        while (rendering)
        {
            time.next(SDL_GetPerformanceCounter(), SDL_GetPerformanceFrequency());

            //SDL_PollEvent(&event);
            //if (event.type == SDL_QUIT)
            SDL_PollEvent(&sdl_event);
            if (sdl_event.type == SDL_QUIT)
            {
                rendering = false;
                app_quit = true;
                return;
            }

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderClear(renderer);

            std::vector<GameObject*> objs_for_render;
            for (GameObject* obj : gameObjects) {
                objs_for_render.push_back(obj);
                for (GameObject* o : ExportGameObjectChildren(obj))
                    objs_for_render.push_back(o);
            } 

            //std::sort(gameObjects.begin(), gameObjects.end(), [](const GameObject* const a, const GameObject* const b) {
            //            return a->layer < b->layer;
            //        });

            std::sort(objs_for_render.begin(), objs_for_render.end(), [](const GameObject* const a, const GameObject* const b) {
                        return a->layer < b->layer;
                    });

            //for(GameObject* obj : gameObjects)
            for(GameObject* obj : objs_for_render)
            {
                // LogInfo("OBJ");
                obj->Update(time);

                SDL_Rect r = {.x = obj->position.x, .y = obj->position.y, .w = obj->size.x, .h = obj->size.y};
                if (obj->texture_filename == "")
                {
                    SDL_SetRenderDrawColor(renderer, obj->color.x, obj->color.y, obj->color.z, obj->color.w);
                    SDL_RenderFillRect(renderer, &r);
                }
                else
                {
                    if (obj->texture == NULL)
                    {
                        obj->texture = IMG_LoadTexture(renderer, obj->texture_filename.c_str());
                    }
                    SDL_RenderCopy(renderer, obj->texture, NULL, &r);
                }

                if (debug)
                    for(Component* c : obj->GetComponents<BoxCollider>())
                    {
                        SDL_Rect r = {.x = c->GetPosition().x, .y = c->GetPosition().y, .w = c->size.x, .h = c->size.y};
                        SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
                        SDL_RenderDrawRect(renderer, &r);
                    }
            }    

            SDL_RenderPresent(renderer);

        //     if (obj.selected)
        //     {
        //         SDL_SetRenderDrawColor(renderer, 245, 91, 20, 255);
        //         SDL_RenderDrawRect(renderer, &r);
        //     }

        }
    }

    void StopRender()
    {
        rendering = false;
        if (renderer != NULL)
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderClear(renderer);
        }
    }

private:
    std::vector<GameObject*> gameObjects;
    bool rendering;
    SDL_Renderer* renderer = NULL;
    int default_layer = 0;

    static bool app_quit;

    std::vector<GameObject*> ExportGameObjectChildren(GameObject* obj, bool recursive = true)
    {
        std::vector<GameObject*> res;
        
        for (GameObject* child : obj->GetChildren())
        {
            res.push_back(child);
            if (recursive)
                for (GameObject* cc : ExportGameObjectChildren(child))
                    res.push_back(cc);
        }

        return res;
    }

};
bool Force2D::Scene::app_quit = false;


}

#endif
