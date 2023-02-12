#ifndef FORCE_2D_HPP
#define FORCE_2D_HPP

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <functional> //std::function
#include <string> //std::to_string
// #include <iostream> 


namespace Force2D
{

bool debug = false;

struct Vector2
{
    int x, y;

    Vector2 operator+(Vector2 other)
    {
        return {.x = x + other.x, .y = y + other.y};
    }    
};

// struct GameObject
// {
//     int x, y;
//     int w, h;
//     Vector2 velocity; 
//     bool selected;
// };

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
    // Vector2 parentPosition;
    Vector2 position;
    Vector2 size;
    int parentLayer;
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

    void SetParentData(Vector2 _parentPosition, int _parentLayer)
    {
        parentPosition = _parentPosition;
        parentLayer    = _parentLayer;
        position       = parentPosition + relativePosition;
    }

    virtual void Update(Time time)
    {
        LogError("Update call in Component");
    }

private:
    Vector2 parentPosition;

};

class BoxCollider : public Component
{

// private:
public:
    std::vector<std::pair<std::function<void()>, bool>> onMouseOverHandlers;
    static double onMouseOverHandlersPoolTime;
    static std::vector<std::pair<std::function<void()>, int>> onMouseOverHandlersPool;

// public:

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

        if((x >= position.x && x <= position.x + size.x) && (y >= position.y && y <= position.y + size.y))
            for (auto p : onMouseOverHandlers)
            {
                if (p.first == NULL) continue;
                
                if (p.second)
                {
                    p.first();
                }
                else
                {
                    if (!BoxCollider::onMouseOverHandlersPool.empty() && BoxCollider::onMouseOverHandlersPool[0].second > parentLayer) continue;
                    if (!BoxCollider::onMouseOverHandlersPool.empty() && BoxCollider::onMouseOverHandlersPool[0].second < parentLayer)
                        BoxCollider::onMouseOverHandlersPool.clear();

                    BoxCollider::onMouseOverHandlersPool.push_back({p.first, parentLayer});
                }
            }
    }

};
double Force2D::BoxCollider::onMouseOverHandlersPoolTime;
std::vector<std::pair<std::function<void()>, int>> Force2D::BoxCollider::onMouseOverHandlersPool;


class GameObject
{
public:
    Vector2 position;
    Vector2 size;
    Vector2 velocity;
    int layer;
    bool selected;


    GameObject(Vector2 _position, Vector2 _size)
    {
        position = _position;
        size     = _size;
    }


    void AddComponent(Component* component)
    {
        // component->SetParentPosition(position);
        component->SetParentData(position, layer);
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

};


class Scene
{
public:
    Time time;

    GameObject* CreateGameObject(Vector2 position, Vector2 size)
    {
        gameObjects.push_back(new GameObject(position, size));
        return gameObjects[gameObjects.size() - 1];
    }

    void StartRender(SDL_Renderer* _renderer)
    {
        rendering = true;
        renderer = _renderer;

        SDL_Event event;
        while (rendering)
        {
            time.next(SDL_GetPerformanceCounter(), SDL_GetPerformanceFrequency());

            SDL_PollEvent(&event);
            if (event.type == SDL_QUIT)
            {
                rendering = false;
                return;
            }

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderClear(renderer);

            for(GameObject* obj : gameObjects)
            {
                // LogInfo("OBJ");
                obj->Update(time);
                SDL_Rect r = {.x = obj->position.x, .y = obj->position.y, .w = obj->size.x, .h = obj->size.y};
                SDL_SetRenderDrawColor(renderer, 122, 122, 122, 255);
                SDL_RenderFillRect(renderer, &r);

                if (debug)
                    for(Component* c : obj->GetComponents<BoxCollider>())
                    {
                        SDL_Rect r = {.x = c->position.x, .y = c->position.y, .w = c->size.x, .h = c->size.y};
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

};

}

#endif
