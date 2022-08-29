// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// Converted to WinRT/C++ by ata6502

#pragma once

#include "Primitives.h"

// The material types for each of the collision meshes
enum class MeshID
{
    Wall,
    Ground,
    Floor,
};

// A contact or penetration of the mesh with the ball
// Using a struct so that this can be stack allocated
struct Contact
{
    DirectX::XMFLOAT4 plane;
    DirectX::XMFLOAT3 contactPosition;
    float closingVelocity;
    float penetrationDistance;
    BOOL contactIsEdge;
    Triangle triangle;
    MeshID meshID;
    size_t triIndex;

    Contact(const Triangle& tri, size_t index, MeshID mesh) :
        triangle(tri),
        triIndex(index),
        meshID(mesh),
        closingVelocity(0.f),
        contactIsEdge(false),
        penetrationDistance(0.f)
    {
        plane = tri.plane;
        ZeroMemory(&contactPosition, sizeof(contactPosition));
    }

    FORCEINLINE DirectX::XMVECTOR GetSurfaceNormal() const
    {
        return XMLoadFloat3((DirectX::XMFLOAT3*)&plane);
    }

    BOOL IsColliding() const
    {
        const float MIN_COLLISION_DISTANCE = -1.0E-5f;
        return penetrationDistance <= MIN_COLLISION_DISTANCE;
    }

    void Invalidate()
    {
        penetrationDistance = FLT_MAX;
    }

    DirectX::XMVECTOR Resolve(DirectX::FXMVECTOR position, DirectX::FXMVECTOR radius);

    BOOL CalculateContact(DirectX::FXMVECTOR position, DirectX::FXMVECTOR radiusIn, DirectX::FXMVECTOR path);
};

// The collision engine
class Collision
{
private:
    BOOL m_intersectsGround;

public:
    std::vector<Triangle> m_groundTriList;
    std::vector<Triangle> m_wallTriList;
    std::vector<Triangle> m_floorTriList;
    std::vector<Contact> m_collisions;

    Collision() :
        m_intersectsGround(FALSE)
    {
    }

    inline BOOL IntersectsWithGround() const
    {
        return m_intersectsGround;
    }

    BOOL BuildCollisionListForSphere(const Sphere& meshLocalSpace, DirectX::FXMVECTOR path);
    Contact* FindWorstInterpenetration();
    void UpdateInterpenetrations(DirectX::FXMVECTOR newPosition, DirectX::FXMVECTOR radius, DirectX::FXMVECTOR path);

private:
    void MergeSharedEdgeCoplanarContacts(DirectX::FXMVECTOR sphere, DirectX::FXMVECTOR radius, DirectX::FXMVECTOR path);
    BOOL AccumulateSphereTriangleIntersections(
        DirectX::FXMVECTOR sphere,
        DirectX::FXMVECTOR radius,
        DirectX::FXMVECTOR path,
        MeshID mesh,
        const std::vector<Triangle>& triList
    );
};
