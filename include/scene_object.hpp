#ifndef SCENE_OBJECT_HPP
#define SCENE_OBJECT_HPP

#include "pch.hpp"

class SceneObject {
public:
    enum ObjectType {
        MODEL,
        LIGHT,
    };

    virtual ~SceneObject() = default;

    virtual ObjectType GetObjectType() const { return m_ObjectType; }

protected:
    ObjectType m_ObjectType;
};

#endif
