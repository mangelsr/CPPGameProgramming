#pragma once

#include <string>
#include <tuple>

#include "Components.h"

class EntityManager;

typedef std::tuple<
    CTransform,
    CLifeSpan,
    CInput,
    CBoundingBox,
    CAnimation,
    CGravity,
    CState>
    ComponentTuple;

class Entity
{
    friend class EntityManager;

    size_t m_id = 0;
    std::string m_tag = "default";
    bool m_active = true;
    ComponentTuple m_components;

    // constructor is private so we can never create
    // entities outside the EntityManager which has friend access
    Entity(const size_t &id, const std::string &tag);

public:
    void destroy();
    size_t id() const;
    bool isActive() const;
    const std::string &tag() const;

    template <typename T>
    bool hasComponent() const
    {
        // Attempt to get the component
        try
        {
            getComponent<T>();
            return true; // If getComponent doesn't throw, the component exists
        }
        catch (const std::out_of_range &oor)
        {
            return false; // If getComponent throws, the component doesn't exist
        }
    }

    template <typename T, typename... TArgs>
    T &addComponent(TArgs &&...mArgs)
    {
        auto &component = getComponent<T>();
        component = T(std::forward<TArgs>(mArgs)...);
        component.has = true;
        return component;
    }

    template <typename T>
    T &getComponent()
    {
        return std::get<T>(m_components);
    }

    template <typename T>
    const T &getComponent() const
    {
        return std::get<T>(m_components);
    }

    template <typename T>
    void removeComponent()
    {
        getComponent<T>() = T();
    }
};