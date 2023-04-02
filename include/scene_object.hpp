#ifndef SCENE_OBJECT_HPP
#define SCENE_OBJECT_HPP

#include "pch.hpp"

#include "transform.hpp"

class SceneObject {
public:
    enum ObjectType {
        MODEL,
        LIGHT,
    };

    SceneObject(ObjectType type, Transform transform = Transform())
        : m_ObjectType(type), m_Transform(transform) {}
    virtual ~SceneObject() = default;

    virtual ObjectType GetObjectType() const { return m_ObjectType; }

    virtual Transform GetTransform() const { return m_Transform; }
    virtual void SetTransform(Transform transform) { m_Transform = transform; }

protected:
    ObjectType m_ObjectType;

    Transform m_Transform;
};

#endif
