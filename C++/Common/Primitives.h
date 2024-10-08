// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// Converted to WinRT/C++ by ata6502

#pragma once

class Triangle
{
public:
    DirectX::XMFLOAT3 A;
    DirectX::XMFLOAT3 B;
    DirectX::XMFLOAT3 C;
    DirectX::XMFLOAT4 plane;

    Triangle(Triangle const& rhs)
    {
        A = rhs.A;
        B = rhs.B;
        C = rhs.C;
        plane = rhs.plane;
    }

    Triangle(DirectX::XMFLOAT3 const& V0, DirectX::XMFLOAT3 const& V1, DirectX::XMFLOAT3 const& V2) :
        A(V0),
        B(V1),
        C(V2)
    {
        using namespace DirectX;

        // Create a plane from three points (the triangle verticies) on the plane. XMPlaneFromPoints returns a vector 
        // whose components are the coefficients of the plane (A, B, C, D) for the plane equation Ax + By + Cz + D = 0,
        // where D = -n dot v0. When representing a plane in code, it suffices to store only the normal vector n and 
        // the constant d as XMFLOAT4.
        XMStoreFloat4(&plane, XMPlaneFromPoints(XMLoadFloat3(&V0), XMLoadFloat3(&V1), XMLoadFloat3(&V2)));
    }

    // Returns true if the triangle is degenerate.
    FORCEINLINE BOOL IsDegenerate() const
    {
        using namespace DirectX;

        XMVECTOR planeNormal = XMLoadFloat3((XMFLOAT3*)&plane);
        return XMVector3Equal(planeNormal, XMVectorZero());
    }

    // Tests this triangle against another one to see if they share any vertices.
    // This method may have bugs as noted here: https://github.com/microsoft/Windows-appsample-marble-maze/issues/8
    FORCEINLINE bool SharesVerts(
        DirectX::FXMVECTOR V0,
        DirectX::FXMVECTOR V1,
        DirectX::FXMVECTOR V2
    ) const
    {
        using namespace DirectX;

        // XMVectorOrInt computes the logical OR of two vectors, treating each component as an unsigned integer.
        // XMVectorEqual performs a per-component test for equality of two vectors. Returns a vector containing 
        // the results of each component test.

        const XMVECTOR VA = XMLoadFloat3(&A);
        XMVECTOR sharesEdge = XMVectorOrInt(
            XMVectorOrInt(
                XMVectorEqual(V0, VA),
                XMVectorEqual(V1, VA)
            ),
            XMVectorEqual(V2, VA)
        );
        const XMVECTOR VB = XMLoadFloat3(&B);
        sharesEdge = XMVectorOrInt(
            sharesEdge,
            XMVectorOrInt(
                XMVectorOrInt(
                    XMVectorEqual(V0, VB),
                    XMVectorEqual(V1, VB)
                ),
                XMVectorEqual(V2, VB)
            )
        );
        const XMVECTOR VC = XMLoadFloat3(&C);
        sharesEdge = XMVectorOrInt(
            sharesEdge,
            XMVectorOrInt(
                XMVectorOrInt(
                    XMVectorEqual(V0, VC),
                    XMVectorEqual(V1, VC)
                ),
                XMVectorEqual(V2, VC)
            )
        );

        return XMVector3EqualInt(sharesEdge, XMVectorTrueInt());
    }

    // Checks if this triangle is coplanar with a given plane.
    FORCEINLINE bool IsCoplanar(
        DirectX::CXMVECTOR planeIn
    ) const
    {
        using namespace DirectX;

        const XMVECTOR VP = XMLoadFloat3((XMFLOAT3*)&plane);
        return XMVector3Equal(VP, planeIn);
    }

    // Returns the unit normal of the triangle surface..
    FORCEINLINE DirectX::XMVECTOR Normal() const
    {
        using namespace DirectX;

        return XMLoadFloat3((XMFLOAT3*)&plane);
    }
};

// Simple sphere wrapper. We store the sphere as a 16-byte aligned FLOAT4, so that we can perform
// fast unaligned reads. All of the parameters are packed into a single FLOAT4.
struct Sphere
{
    DirectX::XMFLOAT4 center;

    FORCEINLINE float Radius() const
    {
        return center.w;
    }

    Sphere()
    {
        ZeroMemory(&center, sizeof(center));
    }

    explicit Sphere(DirectX::XMFLOAT3 const& centerIn, FLOAT radius)
    {
        center = DirectX::XMFLOAT4(centerIn.x, centerIn.y, centerIn.z, radius);
    }

    explicit Sphere(float x, float y, float z, float radius)
    {
        center = DirectX::XMFLOAT4(x, y, z, radius);
    }
};
