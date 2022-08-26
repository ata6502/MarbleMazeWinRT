// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// Converted to WinRT/C++ by ata6502

//
// The SDK Mesh format (.sdkmesh) is not a recommended file format for production games.
// It was designed to meet the specific needs of the SDK samples.  Any real-world
// applications should avoid this file format in favor of a destination format that
// meets the specific needs of the application.
//

#pragma once

//--------------------------------------------------------------------------------------
// Hard Defines for the various structures
//--------------------------------------------------------------------------------------
#define SDKMESH_FILE_VERSION 101
#define MAX_VERTEX_ELEMENTS 32
#define MAX_VERTEX_STREAMS 16
#define MAX_FRAME_NAME 100
#define MAX_MESH_NAME 100
#define MAX_SUBSET_NAME 100
#define MAX_MATERIAL_NAME 100
#define MAX_TEXTURE_NAME MAX_PATH
#define MAX_MATERIAL_PATH MAX_PATH
#define INVALID_FRAME ((uint32_t)-1)
#define INVALID_MESH ((uint32_t)-1)
#define INVALID_MATERIAL ((uint32_t)-1)
#define INVALID_SUBSET ((uint32_t)-1)
#define INVALID_ANIMATION_DATA ((uint32_t)-1)
#define INVALID_SAMPLER_SLOT ((uint32_t)-1)
#define ERROR_RESOURCE_VALUE 1

template <typename TYPE> BOOL IsErrorResource(TYPE data)
{
    if ((TYPE)ERROR_RESOURCE_VALUE == data) {
        return TRUE;
    }

    return FALSE;
}

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p); (p) = nullptr; } }
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p); (p) = nullptr; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p) = nullptr; } }
#endif

//--------------------------------------------------------------------------------------
// Enumerated Types.
//--------------------------------------------------------------------------------------
enum class SDKMeshPrimitiveType
{
    TriangleList = 0,
    TriangleStrip,
    LineList,
    LineStrip,
    PointList,
    TriangleListAdjacent,
    TriangleStripAdjacent,
    LineListAdjacent,
    LineStripAdjacent,
    QuadPatchList,
    TrianglePatchList,
};

enum class SDKMeshIndexType
{
    Bits16 = 0,
    Bits32,
};

enum class FrameTransformType
{
    Relative = 0,
    Absolute,        // This is not currently used but is here to support absolute transformations in the future
};

//--------------------------------------------------------------------------------------
// Structures.  Unions with pointers are forced to 64bit.
//--------------------------------------------------------------------------------------
struct SDKMESH_HEADER
{
    // Basic Info and sizes
    UINT Version;
    BYTE IsBigEndian;
    uint64_t HeaderSize;
    uint64_t NonBufferDataSize;
    uint64_t BufferDataSize;

    // Stats
    UINT NumVertexBuffers;
    UINT NumIndexBuffers;
    UINT NumMeshes;
    UINT NumTotalSubsets;
    UINT NumFrames;
    UINT NumMaterials;

    // Offsets to Data
    uint64_t VertexStreamHeadersOffset;
    uint64_t IndexStreamHeadersOffset;
    uint64_t MeshDataOffset;
    uint64_t SubsetDataOffset;
    uint64_t FrameDataOffset;
    uint64_t MaterialDataOffset;
};

typedef struct _SDKMESHVERTEXELEMENT
{
    WORD    Stream;     // Stream index
    WORD    Offset;     // Offset in the stream in bytes
    BYTE    Type;       // Data type
    BYTE    Method;     // Processing method
    BYTE    Usage;      // Semantics
    BYTE    UsageIndex; // Semantic index
} SDKMESHVERTEXELEMENT, *LPSDKMESHVERTEXELEMENT;

struct SDKMESH_VERTEX_BUFFER_HEADER
{
    uint64_t NumVertices;
    uint64_t SizeBytes;
    uint64_t StrideBytes;
    SDKMESHVERTEXELEMENT Decl[MAX_VERTEX_ELEMENTS];
    union
    {
        uint64_t DataOffset;                // (This also forces the union to 64bits)
        ID3D11Buffer* VertexBuffer;
    };
};

struct SDKMESH_INDEX_BUFFER_HEADER
{
    uint64_t NumIndices;
    uint64_t SizeBytes;
    uint32_t IndexType;
    union
    {
        uint64_t DataOffset;                // (This also forces the union to 64bits)
        ID3D11Buffer* IndexBuffer;
    };
};

struct SDKMESH_MESH
{
    char Name[MAX_MESH_NAME];
    BYTE NumVertexBuffers;
    UINT VertexBuffers[MAX_VERTEX_STREAMS];
    UINT IndexBuffer;
    UINT NumSubsets;
    UINT NumFrameInfluences;             // aka bones

    DirectX::XMFLOAT3 BoundingBoxCenter;
    DirectX::XMFLOAT3 BoundingBoxExtents;

    union
    {
        uint64_t SubsetOffset;            // Offset to list of subsets (This also forces the union to 64bits)
        UINT* Subsets;                  // Pointer to list of subsets
    };
    union
    {
        uint64_t FrameInfluenceOffset;    // Offset to list of frame influences (This also forces the union to 64bits)
        UINT* FrameInfluences;          // Pointer to list of frame influences
    };
};

struct SDKMESH_SUBSET
{
    char Name[MAX_SUBSET_NAME];
    UINT MaterialID;
    UINT PrimitiveType;
    uint64_t IndexStart;
    uint64_t IndexCount;
    uint64_t VertexStart;
    uint64_t VertexCount;
};

struct SDKMESH_FRAME
{
    char Name[MAX_FRAME_NAME];
    UINT Mesh;
    UINT ParentFrame;
    UINT ChildFrame;
    UINT SiblingFrame;
    DirectX::XMFLOAT4X4 Matrix;
    UINT AnimationDataIndex;        // Used to index which set of keyframes transforms this frame
};

struct SDKMESH_MATERIAL
{
    char Name[MAX_MATERIAL_NAME];

    // Use MaterialInstancePath
    char MaterialInstancePath[MAX_MATERIAL_PATH];

    // Or fall back to d3d8-type materials
    char DiffuseTextureName[MAX_TEXTURE_NAME];
    char NormalTextureName[MAX_TEXTURE_NAME];
    char SpecularTextureName[MAX_TEXTURE_NAME];

    DirectX::XMFLOAT4 Diffuse;
    DirectX::XMFLOAT4 Ambient;
    DirectX::XMFLOAT4 Specular;
    DirectX::XMFLOAT4 Emissive;
    FLOAT Power;

    union
    {
        uint64_t Force64_1;            // Force the union to 64bits
        ID3D11Texture2D* DiffuseTexture;
    };
    union
    {
        uint64_t Force64_2;            // Force the union to 64bits
        ID3D11Texture2D* NormalTexture;
    };
    union
    {
        uint64_t Force64_3;            // Force the union to 64bits
        ID3D11Texture2D* SpecularTexture;
    };

    union
    {
        uint64_t Force64_4;            // Force the union to 64bits
        ID3D11ShaderResourceView* DiffuseRV;
    };
    union
    {
        uint64_t Force64_5;            // Force the union to 64bits
        ID3D11ShaderResourceView* NormalRV;
    };
    union
    {
        uint64_t Force64_6;            // Force the union to 64bits
        ID3D11ShaderResourceView* SpecularRV;
    };
};

struct SDKANIMATION_FILE_HEADER
{
    UINT Version;
    BYTE IsBigEndian;
    UINT FrameTransformType;
    UINT NumFrames;
    UINT NumAnimationKeys;
    UINT AnimationFPS;
    uint64_t AnimationDataSize;
    uint64_t AnimationDataOffset;
};

struct SDKANIMATION_DATA
{
    DirectX::XMFLOAT3 Translation;
    DirectX::XMFLOAT4 Orientation;
    DirectX::XMFLOAT3 Scaling;
};

struct SDKANIMATION_FRAME_DATA
{
    char FrameName[MAX_FRAME_NAME];
    union
    {
        uint64_t DataOffset;
        SDKANIMATION_DATA* AnimationData;
    };
};

#ifndef _CONVERTER_APP_

//--------------------------------------------------------------------------------------
// SDKMesh class.  This class reads the sdkmesh file format for use by the samples
//--------------------------------------------------------------------------------------
class SDKMesh
{
private:
    UINT m_numOutstandingResources;
    bool m_loading;
    HANDLE m_hFile;
    HANDLE m_hFileMappingObject;
    ID3D11Device* m_d3dDevice;
    ID3D11DeviceContext* m_d3dContext;

protected:
    // These are the pointers to the two chunks of data loaded in from the mesh file
    BYTE* m_staticMeshData;
    BYTE* m_heapData;
    BYTE* m_animationData;
    BYTE** m_vertices;
    BYTE** m_indices;

    // Keep track of the path
    WCHAR m_path[MAX_PATH];
    WCHAR m_meshName[MAX_PATH];

    // General mesh info
    SDKMESH_HEADER* m_meshHeader;
    SDKMESH_VERTEX_BUFFER_HEADER* m_vertexBufferArray;
    SDKMESH_INDEX_BUFFER_HEADER* m_indexBufferArray;
    SDKMESH_MESH* m_meshArray;
    SDKMESH_SUBSET* m_subsetArray;
    SDKMESH_FRAME* m_frameArray;
    SDKMESH_MATERIAL* m_materialArray;

    // Adjacency information (not part of the m_pStaticMeshData, so it must be created and destroyed separately)
    SDKMESH_INDEX_BUFFER_HEADER* m_adjacencyIndexBufferArray;

    // Animation
    SDKANIMATION_FILE_HEADER* m_animationHeader;
    SDKANIMATION_FRAME_DATA* m_animationFrameData;
    DirectX::XMMATRIX* m_bindPoseFrameMatrices;
    DirectX::XMMATRIX* m_transformedFrameMatrices;
    DirectX::XMMATRIX* m_worldPoseFrameMatrices;

protected:
    void LoadMaterials(ID3D11Device3* d3dDevice, _In_reads_(numMaterials) SDKMESH_MATERIAL* materials, uint32_t numMaterials);
    HRESULT CreateVertexBuffer(ID3D11Device* d3dDevice, SDKMESH_VERTEX_BUFFER_HEADER* header, void* vertices);
    HRESULT CreateIndexBuffer(ID3D11Device* d3dDevice, SDKMESH_INDEX_BUFFER_HEADER* header, void* indices);
    virtual HRESULT CreateFromFile(ID3D11Device3* d3dDevice, WCHAR* filename, bool createAdjacencyIndices);
    virtual HRESULT CreateFromMemory(ID3D11Device3* d3dDevice, byte* data, uint32_t byteCount, bool createAdjacencyIndices, bool copyStatic);

    // frame manipulation
    void TransformBindPoseFrame(uint32_t frame, DirectX::CXMMATRIX parentWorld);
    void TransformFrame(uint32_t frame, DirectX::CXMMATRIX parentWorld, double time);
    void TransformFrameAbsolute(uint32_t frame, double time);

    // Direct3D rendering helpers
    void RenderMesh(uint32_t meshIndex, bool adjacent, ID3D11DeviceContext* d3dContext, uint32_t diffuseSlot, uint32_t normalSlot, uint32_t specularSlot);
    void RenderFrame(uint32_t frame, bool adjacent, ID3D11DeviceContext* d3dContext, uint32_t diffuseSlot, uint32_t normalSlot, uint32_t specularSlot);

public:
    SDKMesh();
    virtual ~SDKMesh();

    virtual HRESULT Create(ID3D11Device3* d3dDevice, WCHAR* filename, bool createAdjacencyIndices = false);
    virtual HRESULT Create(ID3D11Device3* d3dDevice, byte* data, uint32_t byteCount, bool createAdjacencyIndices = false, bool copyStatic = false);
    virtual HRESULT LoadAnimation(WCHAR* filename);
    virtual void Destroy();

    // Frame manipulation
    void TransformBindPose(DirectX::CXMMATRIX world);
    void TransformMesh(DirectX::CXMMATRIX world, double time);

    // Rendering
    virtual void Render(
        ID3D11DeviceContext* d3dContext,
        UINT diffuseSlot = INVALID_SAMPLER_SLOT,
        UINT normalSlot = INVALID_SAMPLER_SLOT,
        UINT specularSlot = INVALID_SAMPLER_SLOT
        );

    virtual void RenderAdjacent(
        ID3D11DeviceContext* d3dContext,
        uint32_t diffuseSlot = INVALID_SAMPLER_SLOT,
        uint32_t normalSlot = INVALID_SAMPLER_SLOT,
        uint32_t specularSlot = INVALID_SAMPLER_SLOT
        );

    // Helpers
    static D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveType(SDKMeshPrimitiveType primitiveType);
    DXGI_FORMAT GetIndexBufferFormat(uint32_t mesh);
    ID3D11Buffer* GetVertexBuffer(uint32_t mesh, uint32_t vertexBuffer);
    ID3D11Buffer* GetIndexBuffer(uint32_t mesh);
    SDKMeshIndexType GetIndexType(uint32_t mesh);
    ID3D11Buffer* GetAdjacencyIndexBuffer(uint32_t mesh);
    WCHAR* GetMeshPath();
    uint32_t GetNumMeshes();
    uint32_t GetNumMaterials();
    uint32_t GetNumVertexBuffers();
    uint32_t GetNumIndexBuffers();

    ID3D11Buffer* GetVertexBufferAt(uint32_t vertexBuffer);
    ID3D11Buffer* GetIndexBufferAt(uint32_t indexBuffer);

    byte* GetRawVerticesAt(uint32_t vertexBuffer);
    byte* GetRawIndicesAt(uint32_t indexBuffer);
    SDKMESH_MATERIAL* GetMaterial(uint32_t material);
    SDKMESH_MESH* GetMesh(uint32_t mesh);
    uint32_t GetNumSubsets(uint32_t mesh);
    SDKMESH_SUBSET* GetSubset(uint32_t mesh, uint32_t subset);
    uint32_t GetVertexStride(uint32_t mesh, uint32_t vertexBuffer);
    uint32_t GetNumFrames();
    SDKMESH_FRAME* GetFrame(uint32_t frame);
    SDKMESH_FRAME* FindFrame(char* name);
    uint64_t GetNumVertices(uint32_t mesh, uint32_t vertexBuffer);
    uint64_t GetNumIndices(uint32_t mesh);
    DirectX::XMFLOAT3 GetMeshBoundingBoxCenter(uint32_t mesh);
    DirectX::XMFLOAT3 GetMeshBoundingBoxExtents(uint32_t mesh);
    uint32_t GetOutstandingResources();
    uint32_t GetOutstandingBufferResources();
    bool CheckLoadDone();
    bool IsLoaded();
    bool IsLoading();
    void SetLoading(bool loading);
    bool HadLoadingError();

    // Animation
    uint32_t GetNumInfluences(uint32_t mesh);
    uint32_t GetAnimationKeyFromTime(double time);
    bool GetAnimationProperties(uint32_t* numKeys, float* frameTime);
    const DirectX::CXMMATRIX GetMeshInfluenceMatrix(uint32_t mesh, uint32_t influence);
    const DirectX::CXMMATRIX GetWorldMatrix(uint32_t frameIndex);
    const DirectX::CXMMATRIX GetInfluenceMatrix(uint32_t frameIndex);
};

#endif

