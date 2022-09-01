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
// Hard defines for the various structures
//--------------------------------------------------------------------------------------
constexpr uint32_t SDKMESH_FILE_VERSION = 101;
constexpr uint32_t MAX_FRAME_NAME = 100;
constexpr uint32_t MAX_MATERIAL_NAME = 100; 
constexpr uint32_t MAX_MESH_NAME = 100;
constexpr uint32_t MAX_SUBSET_NAME = 100;
constexpr uint32_t MAX_MATERIAL_PATH = MAX_PATH;
constexpr uint32_t MAX_TEXTURE_NAME = MAX_PATH;
constexpr uint32_t MAX_D3D11_VERTEX_STREAMS = D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT;
constexpr uint32_t MAX_VERTEX_ELEMENTS = 32;
constexpr uint32_t MAX_VERTEX_STREAMS = 16;
constexpr uint32_t INVALID_FRAME = uint32_t(-1);
constexpr uint32_t INVALID_MESH = uint32_t(-1);
constexpr uint32_t INVALID_SAMPLER_SLOT = uint32_t(-1);

//--------------------------------------------------------------------------------------
// Error detection
//--------------------------------------------------------------------------------------
template <typename T> bool IsErrorResource(T data)
{
    constexpr uint32_t ERROR_RESOURCE_VALUE = 1;

    return (data == (T)ERROR_RESOURCE_VALUE);
}

//--------------------------------------------------------------------------------------
// Structures.  Unions with pointers are forced to 64bit.
//--------------------------------------------------------------------------------------

struct SDKMESH_HEADER
{
    // Basic info and sizes
    uint32_t Version;
    uint8_t  IsBigEndian;
    uint64_t HeaderSize;
    uint64_t NonBufferDataSize;
    uint64_t BufferDataSize;

    // Stats
    uint32_t NumVertexBuffers;
    uint32_t NumIndexBuffers;
    uint32_t NumMeshes;
    uint32_t NumTotalSubsets;
    uint32_t NumFrames;
    uint32_t NumMaterials;

    // Offsets to data
    uint64_t VertexStreamHeadersOffset;
    uint64_t IndexStreamHeadersOffset;
    uint64_t MeshDataOffset;
    uint64_t SubsetDataOffset;
    uint64_t FrameDataOffset;
    uint64_t MaterialDataOffset;
};

#pragma pack(push,4)

struct D3DVERTEXELEMENT9
{
    uint16_t Stream;     // Stream index
    uint16_t Offset;     // Offset in the stream in bytes
    uint8_t  Type;       // Data type
    uint8_t  Method;     // Processing method
    uint8_t  Usage;      // Semantics
    uint8_t  UsageIndex; // Semantic index
};

#pragma pack(pop)

struct SDKMESH_VERTEX_BUFFER_HEADER
{
    uint64_t NumVertices;
    uint64_t SizeBytes;
    uint64_t StrideBytes;
    D3DVERTEXELEMENT9 Decl[MAX_VERTEX_ELEMENTS];
    union
    {
        uint64_t DataOffset; // forces the union to 64bits
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
        uint64_t DataOffset; // forces the union to 64bits
        ID3D11Buffer* IndexBuffer;
    };
};

struct SDKMESH_MESH
{
    char Name[MAX_MESH_NAME];
    uint8_t NumVertexBuffers;
    uint32_t VertexBuffers[MAX_VERTEX_STREAMS];
    uint32_t IndexBuffer;
    uint32_t NumSubsets;
    uint32_t NumFrameInfluences; // aka bones

    DirectX::XMFLOAT3 BoundingBoxCenter;
    DirectX::XMFLOAT3 BoundingBoxExtents;

    union
    {
        uint64_t SubsetOffset; // the offset to the list of subsets (forces the union to 64bits)
        uint32_t* Subsets;     // pointer to list of subsets
    };
    union
    {
        uint64_t FrameInfluenceOffset; // the offset to the list of frame influences (forces the union to 64bits)
        uint32_t* FrameInfluences;     // pointer to list of frame influences
    };
};

struct SDKMESH_SUBSET
{
    char Name[MAX_SUBSET_NAME];
    uint32_t MaterialID;
    uint32_t PrimitiveType;
    uint64_t IndexStart;
    uint64_t IndexCount;
    uint64_t VertexStart;
    uint64_t VertexCount;
};

struct SDKMESH_FRAME
{
    char Name[MAX_FRAME_NAME];
    uint32_t Mesh;
    uint32_t ParentFrame;
    uint32_t ChildFrame;
    uint32_t SiblingFrame;
    DirectX::XMFLOAT4X4 Matrix;
    uint32_t AnimationDataIndex; // used to index which set of keyframes transforms this frame
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
    float Power;

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

//
// Platform helpers
//
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p); (p) = nullptr; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p) = nullptr; } }
#endif

class SimpleSdkMesh
{
public:
    SimpleSdkMesh();

    HRESULT Create(ID3D11Device3* d3dDevice, WCHAR* filename, bool createAdjacencyIndices);
    void Render(ID3D11DeviceContext* d3dContext, uint32_t diffuseSlot, uint32_t normalSlot, uint32_t specularSlot);
    void Destroy();

    uint32_t GetNumMeshes();
    SDKMESH_MESH* GetMesh(uint32_t mesh);
    SDKMESH_SUBSET* GetSubset(uint32_t mesh, uint32_t subset);
    uint32_t GetNumSubsets(uint32_t mesh);
    byte* GetRawIndicesAt(uint32_t indexBuffer);
    byte* GetRawVerticesAt(uint32_t vertexBuffer);
    DirectX::XMFLOAT3 GetMeshBoundingBoxExtents(uint32_t mesh);

private:
    std::wstring m_meshName;

    uint32_t m_numOutstandingResources;
    ID3D11Device* m_d3dDevice;

    // Pointers to the data loaded in from the mesh file.
    std::vector<uint8_t> m_meshData;
    uint8_t** m_vertices;
    uint8_t** m_indices;

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

    HRESULT CreateFromFile(ID3D11Device3* d3dDevice, std::wstring const& path, bool createAdjacencyIndices);
    HRESULT CreateFromMemory(ID3D11Device3* d3dDevice, uint32_t byteCount, bool createAdjacencyIndices);
    HRESULT CreateVertexBuffer(ID3D11Device* d3dDevice, SDKMESH_VERTEX_BUFFER_HEADER* header, void* vertices);
    HRESULT CreateIndexBuffer(ID3D11Device* d3dDevice, SDKMESH_INDEX_BUFFER_HEADER* header, void* indices);
    void LoadMaterials(ID3D11Device3* d3dDevice, _In_reads_(numMaterials) SDKMESH_MATERIAL* materials, uint32_t numMaterials);
    void RenderMesh(uint32_t meshIndex, bool adjacent, ID3D11DeviceContext* d3dContext, uint32_t diffuseSlot, uint32_t normalSlot, uint32_t specularSlot);
    void RenderFrame(uint32_t frame, bool adjacent, ID3D11DeviceContext* d3dContext, uint32_t diffuseSlot, uint32_t normalSlot, uint32_t specularSlot);

    D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveType(SDKMeshPrimitiveType primitiveType);
    uint32_t GetOutstandingBufferResources();
};
