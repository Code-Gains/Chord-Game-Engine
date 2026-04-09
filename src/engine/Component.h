#pragma once
#include <iostream>
#include <typeindex>
#include <unordered_map>
#include <bitset>
#include <functional>
#include <optional>
#include <any>

#include "Entity.h"

constexpr size_t MAX_COMPONENTS = 64;
using ComponentSignature = std::bitset<MAX_COMPONENTS>;
using ComponentType = std::uint16_t;

class Component
{
public:
    virtual ~Component() = default;
};

class ComponentUI
{
public:
    ComponentUI() = default;
    virtual ~ComponentUI() = default;
    virtual void RenderUI(Component& component) = 0;
};