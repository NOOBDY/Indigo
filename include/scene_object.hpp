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

    SceneObject(ObjectType type, const std::string &label,
                Transform transform = Transform())
        : m_ID(idCount++), m_Label(label), m_ObjectType(type),
          m_Transform(transform) {}
    virtual ~SceneObject() = default;

    virtual unsigned int GetID() const { return m_ID; }

    virtual std::string GetLabel() const { return m_Label; }

    virtual ObjectType GetObjectType() const { return m_ObjectType; }

    virtual Transform GetTransform() const { return m_Transform; }
    virtual void SetTransform(Transform transform) { m_Transform = transform; }

    static int GetIDCount() { return idCount; }

protected:
    static unsigned int idCount;
    int m_ID;

    std::string m_Label;

    ObjectType m_ObjectType;

    Transform m_Transform;
};

#endif
