#include "pch.h"

#include "BasicLoader.h" // used in LoadMaterials to load textures
#include "SimpleSdkMesh.h"

using namespace DirectX;

SimpleSdkMesh::SimpleSdkMesh() :
    m_d3dDevice(nullptr),
    m_adjacencyIndexBufferArray(nullptr),
    m_frameArray(nullptr),
    m_vertexBufferArray(nullptr),
    m_indexBufferArray(nullptr),
    m_vertices(nullptr),
    m_indices(nullptr),
    m_materialArray(nullptr),
    m_meshArray(nullptr),
    m_meshHeader(nullptr),
    m_subsetArray(nullptr),
    m_meshName(L"")
{
}

HRESULT SimpleSdkMesh::Create(ID3D11Device3* device, WCHAR* filename)
{
    Destroy();

    m_d3dDevice = device;
    return CreateFromFile(filename);
}

void SimpleSdkMesh::Render(ID3D11DeviceContext* d3dContext, uint32_t diffuseSlot, uint32_t normalSlot, uint32_t specularSlot)
{
    RenderFrame(0, false, d3dContext, diffuseSlot, normalSlot, specularSlot);
}

void SimpleSdkMesh::Destroy()
{
    if (m_meshData.size() > 0)
    {
        if (m_materialArray != nullptr)
        {
            for (uint64_t m = 0; m < m_meshHeader->NumMaterials; m++)
            {
                winrt::com_ptr<ID3D11Resource> resource = nullptr;
                if (m_materialArray[m].DiffuseRV && !IsErrorResource(m_materialArray[m].DiffuseRV))
                {
                    resource = nullptr;
                    m_materialArray[m].DiffuseRV->GetResource(resource.put());
                    SAFE_RELEASE(m_materialArray[m].DiffuseRV);
                }
                if (m_materialArray[m].NormalRV && !IsErrorResource(m_materialArray[m].NormalRV))
                {
                    resource = nullptr;
                    m_materialArray[m].NormalRV->GetResource(resource.put());
                    SAFE_RELEASE(m_materialArray[m].NormalRV);
                }
                if (m_materialArray[m].SpecularRV && !IsErrorResource(m_materialArray[m].SpecularRV))
                {
                    resource = nullptr;
                    m_materialArray[m].SpecularRV->GetResource(resource.put());
                    SAFE_RELEASE(m_materialArray[m].SpecularRV);
                }
            }
        }
    }

    if (m_adjacencyIndexBufferArray != nullptr)
    {
        for (uint64_t i = 0; i < m_meshHeader->NumIndexBuffers; i++)
        {
            SAFE_RELEASE(m_adjacencyIndexBufferArray[i].IndexBuffer);
        }
    }

    if (m_indexBufferArray != nullptr)
    {
        for (uint64_t i = 0; i < m_meshHeader->NumIndexBuffers; i++)
        {
            SAFE_RELEASE(m_indexBufferArray[i].IndexBuffer);
        }
    }

    if (m_vertexBufferArray != nullptr)
    {
        for (uint64_t i = 0; i < m_meshHeader->NumVertexBuffers; i++)
        {
            SAFE_RELEASE(m_vertexBufferArray[i].VertexBuffer);
        }
    }

    SAFE_DELETE_ARRAY(m_adjacencyIndexBufferArray);
    SAFE_DELETE_ARRAY(m_vertices);
    SAFE_DELETE_ARRAY(m_indices);

    m_meshData.clear();

    m_meshHeader = nullptr;
    m_vertexBufferArray = nullptr;
    m_indexBufferArray = nullptr;
    m_meshArray = nullptr;
    m_subsetArray = nullptr;
    m_frameArray = nullptr;
    m_materialArray = nullptr;
    m_d3dDevice = nullptr;
}

uint32_t SimpleSdkMesh::GetNumMeshes()
{
    if (!m_meshHeader)
    {
        return 0;
    }

    return m_meshHeader->NumMeshes;
}

SDKMESH_MESH* SimpleSdkMesh::GetMesh(uint32_t mesh)
{
    return &m_meshArray[mesh];
}

uint32_t SimpleSdkMesh::GetNumSubsets(uint32_t mesh)
{
    return m_meshArray[mesh].NumSubsets;
}

byte* SimpleSdkMesh::GetRawIndicesAt(uint32_t indexBuffer)
{
    return m_indices[indexBuffer];
}

byte* SimpleSdkMesh::GetRawVerticesAt(uint32_t vertexBuffer)
{
    return m_vertices[vertexBuffer];
}

DirectX::XMFLOAT3 SimpleSdkMesh::GetMeshBoundingBoxExtents(uint32_t mesh)
{
    return m_meshArray[mesh].BoundingBoxExtents;
}

HRESULT SimpleSdkMesh::CreateFromFile(std::wstring const& path)
{
    HRESULT hr = S_OK;

    winrt::file_handle file{ CreateFile2(path.c_str(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr)};

    winrt::check_bool(bool{ file });
    if (file.get() == INVALID_HANDLE_VALUE)
    {
        winrt::throw_hresult(E_FAIL);
    }

    FILE_STANDARD_INFO fileInfo = { 0 };
    if (!GetFileInformationByHandleEx(file.get(), FileStandardInfo, &fileInfo, sizeof(fileInfo)))
    {
        winrt::throw_hresult(E_FAIL);
    }

    if (fileInfo.EndOfFile.HighPart != 0)
    {
        winrt::throw_hresult(E_OUTOFMEMORY);
    }

    // Store the filename (without the extension) as the mesh name.
    std::wstring filename = path.substr(path.find_last_of(L"/\\") + 1);
    std::wstring::size_type const p(filename.find_last_of('.'));
    m_meshName = filename.substr(0, p);

    uint32_t byteCount = fileInfo.EndOfFile.LowPart;

    // Allocate memory
    m_meshData.resize(byteCount);

    // Read in the file
    DWORD bytesRead = 0;
    if (!ReadFile(file.get(), static_cast<LPVOID>(m_meshData.data()), byteCount, &bytesRead, nullptr))
    {
        winrt::throw_hresult(E_FAIL);
    }

    if (SUCCEEDED(hr))
    {
        hr = CreateFromMemory();
        if (FAILED(hr))
        {
            m_meshData.clear();
        }
    }

    return hr;
}

HRESULT SimpleSdkMesh::CreateFromMemory()
{
    // Get data structures from the mesh file.
    m_meshHeader = reinterpret_cast<SDKMESH_HEADER*>(m_meshData.data());
    m_vertexBufferArray = reinterpret_cast<SDKMESH_VERTEX_BUFFER_HEADER*>(m_meshData.data() + m_meshHeader->VertexStreamHeadersOffset);
    m_indexBufferArray = reinterpret_cast<SDKMESH_INDEX_BUFFER_HEADER*>(m_meshData.data() + m_meshHeader->IndexStreamHeadersOffset);
    m_meshArray = reinterpret_cast<SDKMESH_MESH*>(m_meshData.data() + m_meshHeader->MeshDataOffset);
    m_subsetArray = reinterpret_cast<SDKMESH_SUBSET*>(m_meshData.data() + m_meshHeader->SubsetDataOffset);
    m_frameArray = reinterpret_cast<SDKMESH_FRAME*>(m_meshData.data() + m_meshHeader->FrameDataOffset);
    m_materialArray = reinterpret_cast<SDKMESH_MATERIAL*>(m_meshData.data() + m_meshHeader->MaterialDataOffset);

    // Setup subsets
    for (uint32_t i = 0; i < m_meshHeader->NumMeshes; i++)
    {
        m_meshArray[i].Subsets = (uint32_t*)(m_meshData.data() + m_meshArray[i].SubsetOffset);
        m_meshArray[i].FrameInfluences = (uint32_t*)(m_meshData.data() + m_meshArray[i].FrameInfluenceOffset);
    }

    // error condition
    if (m_meshHeader->Version != SDKMESH_FILE_VERSION)
    {
        return E_NOINTERFACE;
    }

    // Setup buffer data pointer
    byte* bufferData = m_meshData.data() + m_meshHeader->HeaderSize + m_meshHeader->NonBufferDataSize;

    // Get the start of the buffer data
    uint64_t bufferDataStart = m_meshHeader->HeaderSize + m_meshHeader->NonBufferDataSize;

    // Create vertex buffers
    m_vertices = new byte * [m_meshHeader->NumVertexBuffers];
    for (uint32_t i = 0; i < m_meshHeader->NumVertexBuffers; i++)
    {
        byte* vertices = nullptr;
        vertices = (byte*)(bufferData + (m_vertexBufferArray[i].DataOffset - bufferDataStart));
        CreateVertexBuffer(&m_vertexBufferArray[i], vertices);
        m_vertices[i] = vertices;
    }

    // Create index buffers
    m_indices = new byte * [m_meshHeader->NumIndexBuffers];
    for (uint32_t i = 0; i < m_meshHeader->NumIndexBuffers; i++)
    {
        byte* indices = nullptr;
        indices = (byte*)(bufferData + (m_indexBufferArray[i].DataOffset - bufferDataStart));
        CreateIndexBuffer(&m_indexBufferArray[i], indices);
        m_indices[i] = indices;
    }

    // Load Materials
    LoadMaterials(m_materialArray, m_meshHeader->NumMaterials);

    SDKMESH_SUBSET* subset = nullptr;
    D3D11_PRIMITIVE_TOPOLOGY primitiveType;

    // update bounding volume
    SDKMESH_MESH* currentMesh = &m_meshArray[0];
    int tris = 0;
    for (uint32_t mesh = 0; mesh < m_meshHeader->NumMeshes; ++mesh)
    {
        XMFLOAT3 lower;
        XMFLOAT3 upper;

        lower.x = XMVectorGetX(g_XMFltMax); lower.y = XMVectorGetX(g_XMFltMax); lower.z = XMVectorGetX(g_XMFltMax);
        upper.x = -XMVectorGetX(g_XMFltMax); upper.y = -XMVectorGetX(g_XMFltMax); upper.z = -XMVectorGetX(g_XMFltMax);
        currentMesh = GetMesh(mesh);

        int indsize;
        if (m_indexBufferArray[currentMesh->IndexBuffer].IndexType == static_cast<uint32_t>(SDKMeshIndexType::Bits16))
        {
            indsize = 2;
        }
        else
        {
            indsize = 4;
        }

        for (uint32_t subsetIndex = 0; subsetIndex < currentMesh->NumSubsets; subsetIndex++)
        {
            subset = GetSubset(mesh, subsetIndex); // &m_pSubsetArray[currentMesh->pSubsets[subset]];

            primitiveType = GetPrimitiveType(static_cast<SDKMeshPrimitiveType>(subset->PrimitiveType));
            assert(primitiveType == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // only triangle lists are handled.

            uint32_t indexCount = (uint32_t)subset->IndexCount;
            uint32_t indexStart = (uint32_t)subset->IndexStart;

            uint32_t* ind = (uint32_t*)m_indices[currentMesh->IndexBuffer];
            float* verts = (float*)m_vertices[currentMesh->VertexBuffers[0]];
            uint32_t stride = (uint32_t)m_vertexBufferArray[currentMesh->VertexBuffers[0]].StrideBytes;
            assert(stride % 4 == 0);
            stride /= 4;
            for (uint32_t vertind = indexStart; vertind < indexStart + indexCount; ++vertind)
            {
                uint32_t currentIndex = 0;
                if (indsize == 2)
                {
                    uint32_t ind_div2 = vertind / 2;
                    currentIndex = ind[ind_div2];
                    if ((vertind % 2) == 0)
                    {
                        currentIndex = currentIndex << 16;
                        currentIndex = currentIndex >> 16;
                    }
                    else
                    {
                        currentIndex = currentIndex >> 16;
                    }
                }
                else
                {
                    currentIndex = ind[vertind];
                }
                tris++;
                XMFLOAT3* pt = (XMFLOAT3*)&(verts[stride * currentIndex]);
                if (pt->x < lower.x)
                {
                    lower.x = pt->x;
                }
                if (pt->y < lower.y)
                {
                    lower.y = pt->y;
                }
                if (pt->z < lower.z)
                {
                    lower.z = pt->z;
                }
                if (pt->x > upper.x)
                {
                    upper.x = pt->x;
                }
                if (pt->y > upper.y)
                {
                    upper.y = pt->y;
                }
                if (pt->z > upper.z)
                {
                    upper.z = pt->z;
                }
            }
        }
        XMVECTOR u = XMLoadFloat3(&upper);
        XMVECTOR l = XMLoadFloat3(&lower);
        XMVECTOR half = XMVectorSubtract(u, l);
        XMVectorScale(half, 0.5f);
        XMStoreFloat3(&currentMesh->BoundingBoxExtents, half);
        half = XMVectorAdd(l, half);
        XMStoreFloat3(&currentMesh->BoundingBoxCenter, half);
    }

    return S_OK;
}

void SimpleSdkMesh::CreateVertexBuffer(SDKMESH_VERTEX_BUFFER_HEADER* header, void* vertices)
{
    header->DataOffset = 0;

    // Vertex Buffer
    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.ByteWidth = static_cast<uint32_t>(header->SizeBytes);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = vertices;

    winrt::check_hresult(
        m_d3dDevice->CreateBuffer(
            &bufferDesc, 
            &initData, 
            &header->VertexBuffer
        )
    );

    WCHAR objectName[MAX_PATH];
    wcsncpy_s(objectName, MAX_PATH, m_meshName.c_str(), m_meshName.size());
    wcscat_s(objectName, MAX_PATH, L"_VertexBuffer");

    winrt::check_hresult(
        header->VertexBuffer->SetPrivateData(
            WKPDID_D3DDebugObjectName, 
            (uint32_t)wcslen(objectName), 
            objectName
        )
    );
}

HRESULT SimpleSdkMesh::CreateIndexBuffer(SDKMESH_INDEX_BUFFER_HEADER* header, void* indices)
{
    HRESULT hr = S_OK;
    header->DataOffset = 0;

    // Index Buffer
    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.ByteWidth = static_cast<uint32_t>(header->SizeBytes);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = indices;
    hr = m_d3dDevice->CreateBuffer(&bufferDesc, &initData, &header->IndexBuffer);
    if (FAILED(hr))
        return hr;

    WCHAR objectName[MAX_PATH];
    wcsncpy_s(objectName, MAX_PATH, m_meshName.c_str(), m_meshName.size());
    wcscat_s(objectName, MAX_PATH, L"_IndexBuffer");
    hr = header->IndexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, (uint32_t)wcslen(objectName), objectName);

    return hr;
}

void SimpleSdkMesh::LoadMaterials(_In_reads_(numMaterials) SDKMESH_MATERIAL* materials, uint32_t numMaterials)
{
    winrt::com_ptr<ID3D11Device3> pDevice;
    pDevice.copy_from(m_d3dDevice);

    BasicLoader loader(pDevice);

    wchar_t path[MAX_PATH];
    wchar_t texturePath[MAX_PATH];
    size_t convertedChars = 0;

    // This is a simple Mesh format that doesn't reuse texture data
    for (uint32_t m = 0; m < numMaterials; m++)
    {
        materials[m].DiffuseTexture = nullptr;
        materials[m].NormalTexture = nullptr;
        materials[m].SpecularTexture = nullptr;
        materials[m].DiffuseRV = nullptr;
        materials[m].NormalRV = nullptr;
        materials[m].SpecularRV = nullptr;

        // load textures
        if (materials[m].DiffuseTextureName[0] != 0)
        {
            size_t size = strlen(materials[m].DiffuseTextureName) + 1;
            mbstowcs_s(&convertedChars, texturePath, size, materials[m].DiffuseTextureName, _TRUNCATE);
            swprintf_s(path, MAX_PATH, L"Media\\Textures\\%s", texturePath);
            loader.LoadTexture(path, nullptr, &materials[m].DiffuseRV);
        }
    }
}

void SimpleSdkMesh::RenderMesh(uint32_t meshIndex, bool adjacent, ID3D11DeviceContext* d3dContext, uint32_t diffuseSlot, uint32_t normalSlot, uint32_t specularSlot)
{
    if (0 < GetOutstandingBufferResources())
    {
        return;
    }

    SDKMESH_MESH* mesh = &m_meshArray[meshIndex];
    uint32_t strides[MAX_D3D11_VERTEX_STREAMS];
    uint32_t offsets[MAX_D3D11_VERTEX_STREAMS];
    ID3D11Buffer* vertexBuffer[MAX_D3D11_VERTEX_STREAMS];

    ZeroMemory(strides, sizeof(uint32_t) * MAX_D3D11_VERTEX_STREAMS);
    ZeroMemory(offsets, sizeof(uint32_t) * MAX_D3D11_VERTEX_STREAMS);
    ZeroMemory(vertexBuffer, sizeof(ID3D11Buffer*) * MAX_D3D11_VERTEX_STREAMS);

    if (mesh->NumVertexBuffers > MAX_D3D11_VERTEX_STREAMS)
    {
        return;
    }

    for (uint64_t i = 0; i < mesh->NumVertexBuffers; i++)
    {
        vertexBuffer[i] = m_vertexBufferArray[mesh->VertexBuffers[i]].VertexBuffer;
        strides[i] = static_cast<uint32_t>(m_vertexBufferArray[mesh->VertexBuffers[i]].StrideBytes);
        offsets[i] = 0;
    }

    SDKMESH_INDEX_BUFFER_HEADER* indexBufferArray;
    if (adjacent)
    {
        indexBufferArray = m_adjacencyIndexBufferArray;
    }
    else
    {
        indexBufferArray = m_indexBufferArray;
    }

    ID3D11Buffer* indexBuffer = indexBufferArray[mesh->IndexBuffer].IndexBuffer;
    DXGI_FORMAT indexBufferFormat = DXGI_FORMAT_R16_UINT;
    switch (indexBufferArray[mesh->IndexBuffer].IndexType)
    {
    case static_cast<uint32_t>(SDKMeshIndexType::Bits16):
        indexBufferFormat = DXGI_FORMAT_R16_UINT;
        break;
    case static_cast<uint32_t>(SDKMeshIndexType::Bits32):
        indexBufferFormat = DXGI_FORMAT_R32_UINT;
        break;
    };

    d3dContext->IASetVertexBuffers(0, mesh->NumVertexBuffers, vertexBuffer, strides, offsets);
    d3dContext->IASetIndexBuffer(indexBuffer, indexBufferFormat, 0);

    SDKMESH_SUBSET* subset = nullptr;
    SDKMESH_MATERIAL* material = nullptr;
    D3D11_PRIMITIVE_TOPOLOGY primitiveType;

    for (uint32_t i = 0; i < mesh->NumSubsets; i++)
    {
        subset = &m_subsetArray[mesh->Subsets[i]];

        primitiveType = GetPrimitiveType(static_cast<SDKMeshPrimitiveType>(subset->PrimitiveType));
        if (adjacent)
        {
            switch (primitiveType)
            {
            case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
                primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
                primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
                primitiveType = D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
                primitiveType = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
                break;
            }
        }

        d3dContext->IASetPrimitiveTopology(primitiveType);

        material = &m_materialArray[subset->MaterialID];
        if (diffuseSlot != INVALID_SAMPLER_SLOT && !IsErrorResource(material->DiffuseRV))
        {
            d3dContext->PSSetShaderResources(diffuseSlot, 1, &material->DiffuseRV);
        }
        if (normalSlot != INVALID_SAMPLER_SLOT && !IsErrorResource(material->NormalRV))
        {
            d3dContext->PSSetShaderResources(normalSlot, 1, &material->NormalRV);
        }
        if (specularSlot != INVALID_SAMPLER_SLOT && !IsErrorResource(material->SpecularRV))
        {
            d3dContext->PSSetShaderResources(specularSlot, 1, &material->SpecularRV);
        }

        uint32_t indexCount = (uint32_t)subset->IndexCount;
        uint32_t indexStart = (uint32_t)subset->IndexStart;
        uint32_t vertexStart = (uint32_t)subset->VertexStart;

        if (adjacent)
        {
            indexCount *= 2;
            indexStart *= 2;
        }

        d3dContext->DrawIndexed(indexCount, indexStart, vertexStart);
    }
}

void SimpleSdkMesh::RenderFrame(uint32_t frame, bool adjacent, ID3D11DeviceContext* d3dContext, uint32_t diffuseSlot, uint32_t normalSlot, uint32_t specularSlot)
{
    if (m_meshData.size() == 0 || !m_frameArray)
    {
        return;
    }

    if (m_frameArray[frame].Mesh != INVALID_MESH)
    {
        RenderMesh(m_frameArray[frame].Mesh, adjacent, d3dContext, diffuseSlot, normalSlot, specularSlot);
    }

    // Render our children
    if (m_frameArray[frame].ChildFrame != INVALID_FRAME)
    {
        RenderFrame(m_frameArray[frame].ChildFrame, adjacent, d3dContext, diffuseSlot, normalSlot, specularSlot);
    }

    // Render our siblings
    if (m_frameArray[frame].SiblingFrame != INVALID_FRAME)
    {
        RenderFrame(m_frameArray[frame].SiblingFrame, adjacent, d3dContext, diffuseSlot, normalSlot, specularSlot);
    }
}

D3D11_PRIMITIVE_TOPOLOGY SimpleSdkMesh::GetPrimitiveType(SDKMeshPrimitiveType primitiveType)
{
    D3D11_PRIMITIVE_TOPOLOGY returnType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    switch (primitiveType)
    {
    case SDKMeshPrimitiveType::TriangleList:
        returnType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        break;
    case SDKMeshPrimitiveType::TriangleStrip:
        returnType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        break;
    case SDKMeshPrimitiveType::LineList:
        returnType = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
        break;
    case SDKMeshPrimitiveType::LineStrip:
        returnType = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
        break;
    case SDKMeshPrimitiveType::PointList:
        returnType = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
        break;
    case SDKMeshPrimitiveType::TriangleListAdjacent:
        returnType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
        break;
    case SDKMeshPrimitiveType::TriangleStripAdjacent:
        returnType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
        break;
    case SDKMeshPrimitiveType::LineListAdjacent:
        returnType = D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
        break;
    case SDKMeshPrimitiveType::LineStripAdjacent:
        returnType = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
        break;
    };

    return returnType;
}

SDKMESH_SUBSET* SimpleSdkMesh::GetSubset(uint32_t mesh, uint32_t subset)
{
    return &m_subsetArray[m_meshArray[mesh].Subsets[subset]];
}

uint32_t SimpleSdkMesh::GetOutstandingBufferResources()
{
    uint32_t outstandingResources = 0;
    if (m_meshHeader == nullptr)
    {
        return 1;
    }

    for (uint32_t i = 0; i < m_meshHeader->NumVertexBuffers; i++)
    {
        if ((m_vertexBufferArray[i].VertexBuffer == nullptr) && !IsErrorResource(m_vertexBufferArray[i].VertexBuffer))
        {
            outstandingResources++;
        }
    }

    for (uint32_t i = 0; i < m_meshHeader->NumIndexBuffers; i++)
    {
        if ((m_indexBufferArray[i].IndexBuffer == nullptr) && !IsErrorResource(m_indexBufferArray[i].IndexBuffer))
        {
            outstandingResources++;
        }
    }

    return outstandingResources;
}

