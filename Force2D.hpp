#ifndef FORCE_2D_HPP
#define FORCE_2D_HPP

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <functional> //std::function
#include <string> //std::to_string


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


class Component
{
public:
    std::string name;
    Vector2 relativePosition;
    Vector2 parentPosition;
    Vector2 position;
    Vector2 size;
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

    void SetParentPosition(Vector2 _parentPosition)
    {
        parentPosition = _parentPosition;
        position = parentPosition + relativePosition;
    }

    virtual void Update()
    {
        LogError("Update call in Component");
    }

};

class BoxCollider : public Component
{

private:
    std::vector<std::function<void()>> onMouseOverHandlers;

public:

    BoxCollider(Vector2 _position, Vector2 _size) : Component(_position, _size) {}
    BoxCollider(Vector2 _position, Vector2 _size, std::string _name) : Component(_position, _size, _name) {}

    int AddOnMouseOverHandler(std::function< void() > handler)
    {
        onMouseOverHandlers.push_back(handler);
        return onMouseOverHandlers.size() - 1;
    }
    
    void RemoveOnMouseOverHandler(int handler_index)
    {
        if (handler_index < 0 || handler_index >= onMouseOverHandlers.size())
            LogFatal("handler index out of bounds", "RemoveOnMouseOverHandler");

        onMouseOverHandlers[handler_index] = NULL;
    }

    

    void Update() override
    {
        // LogInfo("Update call in BoxCollider");
        int x, y;
        SDL_GetMouseState(&x, &y);

        if((x >= position.x && x <= position.x + size.x) && (y >= position.y && y <= position.y + size.y))
            for (auto f : onMouseOverHandlers)
                f();
    }

};

class GameObject
{
public:
    Vector2 position;
    Vector2 size;
    Vector2 velocity;
    bool selected;


    GameObject(Vector2 _position, Vector2 _size)
    {
        position = _position;
        size     = _size;
    }


    void AddComponent(Component* component)
    {
        component->SetParentPosition(position);
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

    void Update()
    {
        for(Component* c : components)
        {
            c->Update();
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
                obj->Update();
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