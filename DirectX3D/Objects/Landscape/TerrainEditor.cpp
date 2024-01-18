#include "Framework.h"

TerrainEditor::TerrainEditor()
    : GameObject(L"Light/Light.hlsl"),
    width(MAX_SIZE), height(MAX_SIZE)
{
    material->SetDiffuseMap(L"Textures/Landscape/Dirt.png");
    secondMap = Texture::Add(L"Textures/Landscape/Dirt2.png");
    thirdMap = Texture::Add(L"Textures/Landscape/Dirt3.png");

    // 메시 제작 호출
    mesh = new Mesh<VertexType>();
    MakeMesh();
    MakeNormal();
    MakeTangent();
    MakeComputeData();
    mesh->CreateMesh();

    //컴퓨트 셰이더 사용
    computeShader = Shader::AddCS(L"Compute/ComputePicking.hlsl");

    //버퍼 생성
    brushBuffer = new BrushBuffer();
    structuredBuffer = new StructuredBuffer(
        inputs.data(),
        sizeof(InputDesc),
        triangleSize,
        sizeof(OutputDesc),
        triangleSize);
    rayBuffer = new RayBuffer();

    //경로명 받기
    char path[128];
    GetCurrentDirectoryA(128, path);
    projectPath = path;

}

TerrainEditor::~TerrainEditor()
{
    delete mesh;
    delete brushBuffer;
    delete rayBuffer;
    delete structuredBuffer;


}

void TerrainEditor::Update()
{
    if (!editEnabled) return;

    if (ComputePicking(pickingPos))
    {
        brushBuffer->Get().pickingPos = pickingPos;

    }
    else
    {
        //return;
    }

    //---------------------------------------------


    if (KEY_PRESS(VK_LBUTTON) && !ImGui::GetIO().WantCaptureMouse)
    {
        switch (editType)
        {
        case HEIGHT:
            AdjustHeight();
            break;
        case ALPHA:
            //관련 기능이 생기면 추가
            break;
        }
    }

    if (KEY_UP(VK_LBUTTON))
    {
        UpdateHeight();
    }
}

void TerrainEditor::Render()
{
    brushBuffer->SetPS(13);
    secondMap->PSSet(11);
    thirdMap->PSSet(12);

    SetRender();
    mesh->Draw();
}

void TerrainEditor::RenderUI()
{
    ImGui::Text("Terrain Pick Position");

    ImGui::Text("X : %.1f, Y : %.1f, Z : %.1f", pickingPos.x, pickingPos.y, pickingPos.z);

    ImGui::Text("Adjust : %f", adjustValue);

    ImGui::Text("-------------------------");
    ImGui::Text("Terrain Editor Options");

    if (ImGui::DragInt("Width", (int*)&width, 1.0f, 2, MAX_SIZE))
    {
        Resize();
    }

    if (ImGui::DragInt("Height", (int*)&height, 1.0f, 2, MAX_SIZE))
    {
        Resize();
    }

    const char* editList[] = { "HEIGHT", "ALPHA", "MAP" };
    ImGui::Combo("EditType", (int*)&editType, editList, 3);

    const char* brushList[] = { "CIRCLE", "SOFT CIRCLE", "RECT" };
    if (ImGui::Combo("BrushType", (int*)&brushType, brushList, 3))
    {
        brushBuffer->Get().type = brushType;
    }

    ImGui::DragFloat("Range", &brushBuffer->Get().range, 1.0f, 0.0f, 20.0f);
    ImGui::DragFloat("AdjustLevel", &adjustValue, 1.0f, -50.0f, +50.0f);
    ImGui::ColorEdit3("Color", (float*)&brushBuffer->Get().color);

    SaveHeightMap();
    LoadHeightMap();
}

Vector3 TerrainEditor::Picking()
{
    // 컴퓨트 셰이더 사용하지 않음 (레거시 코드 : CS와 같이 사용시 오류 가능)

    Ray ray = CAM->ScreenPointToRay(mousePos);

    //그림 픽셀에 계산 시작하기
    for (UINT z = 0; z < height - 1; ++z)
    {
        for (UINT x = 0; x < width - 1; ++x)
        {
            UINT index[4]; //순번 변수 4개 준비
            index[0] = width * (z + 0) + (x + 0); // 0
            index[1] = width * (z + 0) + (x + 1); // 1
            index[2] = width * (z + 1) + (x + 0); // 2
            index[3] = width * (z + 1) + (x + 1); // 3

            //정점받기
            vector<VertexType> vertices = mesh->GetVertices();

            Vector3 p[4]; //픽셀 4개
            for (UINT i = 0; i < 4; ++i)
                p[i] = vertices[index[i]].pos;

            float distance = 0; // 거리 초기화
            if (TriangleTests::Intersects(ray.pos, ray.dir, p[0], p[1], p[2], distance))
            {
                return ray.pos + ray.dir * distance;
            }

            if (TriangleTests::Intersects(ray.pos, ray.dir, p[3], p[1], p[2], distance))
            {
                return ray.pos + ray.dir * distance;
            }
        }
    }

    return Vector3();
}

bool TerrainEditor::ComputePicking(Vector3& pos, Vector3 manualPos, Vector3 manualDir)
{
    // 컴퓨트 셰이더 사용함

    Ray ray;

    if (manualDir == Vector3())
    {
        //마우스 위치에서 광선을 쏘는 걸로
        ray = CAM->ScreenPointToRay(mousePos);
    }
    else
    {
        ray.pos = manualPos;
        ray.dir = manualDir;
    }


    rayBuffer->Get().pos = ray.pos;
    rayBuffer->Get().dir = ray.dir;
    rayBuffer->Get().triangleSize = triangleSize;

    rayBuffer->SetCS(0);

    DC->CSSetShaderResources(0, 1, &structuredBuffer->GetSRV()); 
    DC->CSSetUnorderedAccessViews(0, 1, &structuredBuffer->GetUAV(), nullptr);

    // 컴퓨트 셰이더를 세팅
    computeShader->Set();

    // 병렬 처리 단위 지정
    UINT x = ceil((float)triangleSize / 64.0f);
    DC->Dispatch(x, 1, 1);

    //구조화된 버퍼에 입력에 대한 출력 결과 예약
    structuredBuffer->Copy(outputs.data(), sizeof(OutputDesc) * triangleSize);

    float minDistance = FLT_MAX;
    int minIndex = -1;          

    UINT index = 0;

    for (OutputDesc output : outputs)
    {
        if (output.isPicked)
        {
            if (minDistance > output.distance)
            {
                minDistance = output.distance;
                minIndex = index;             
            }
        }

        index++;
    }

    if (minIndex >= 0)
    {
        pos = ray.pos + ray.dir * minDistance;

        return true;
    }

    return false;
}

void TerrainEditor::MakeMesh()
{
    vector<Float4> pixels(width * height, Float4(0, 0, 0, 0));

    if (heightMap)
    {
        width = (UINT)heightMap->GetSize().x;
        height = (UINT)heightMap->GetSize().y;

        heightMap->ReadPixels(pixels);
    }

    vector<VertexType>& vertices = mesh->GetVertices();
    vertices.clear();
    vertices.reserve(width * height);

    for (UINT z = 0; z < height; ++z)
    {
        for (UINT x = 0; x < width; ++x)
        {
            //현재 픽셀 위치를 정점 및 UV에 대입
            VertexType vertex;
            vertex.pos = { (float)x, 0.0f, (float)(height - z - 1) };
            vertex.uv.x = x / (float)(width - 1);
            vertex.uv.y = z / (float)(height - 1);

            UINT index = width * z + x;
            vertex.pos.y = pixels[index].x * MAX_HEIGHT;

            //정점을 벡터에
            vertices.push_back(vertex);
        }
    }


    // 인덱스
    vector<UINT>& indices = mesh->GetIndices();
    indices.clear();
    indices.reserve((width - 1) * (height - 1) * 6);

    for (UINT z = 0; z < height - 1; ++z)
    {
        for (UINT x = 0; x < width - 1; ++x)
        {
            indices.push_back(width * (z + 0) + (x + 0)); // 0
            indices.push_back(width * (z + 0) + (x + 1)); // 1
            indices.push_back(width * (z + 1) + (x + 0)); // 2

            indices.push_back(width * (z + 1) + (x + 0)); // 2
            indices.push_back(width * (z + 0) + (x + 1)); // 1
            indices.push_back(width * (z + 1) + (x + 1)); // 3
        }
    }
}

void TerrainEditor::MakeNormal()
{
    vector<VertexType>& vertices = mesh->GetVertices();
    vector<UINT> indices = mesh->GetIndices();

    for (UINT i = 0; i < indices.size() / 3; ++i)
    {
        UINT index0 = indices[i * 3 + 0];
        UINT index1 = indices[i * 3 + 1];
        UINT index2 = indices[i * 3 + 2];

        Vector3 v0 = vertices[index0].pos;
        Vector3 v1 = vertices[index1].pos;
        Vector3 v2 = vertices[index2].pos;

        Vector3 e0 = v1 - v0;
        Vector3 e1 = v2 - v0;

        Vector3 normal = Cross(e0, e1).GetNormalized();

        vertices[index0].normal = normal + vertices[index0].normal;
        vertices[index1].normal = normal + vertices[index1].normal;
        vertices[index2].normal = normal + vertices[index2].normal;
    }
}

void TerrainEditor::MakeTangent()
{
    vector<VertexType>& vertices = mesh->GetVertices();
    vector<UINT> indices = mesh->GetIndices();

    for (UINT i = 0; i < indices.size() / 3; ++i)
    {
        UINT index0 = indices[i * 3 + 0];
        UINT index1 = indices[i * 3 + 1];
        UINT index2 = indices[i * 3 + 2];

        Vector3 p0 = vertices[index0].pos;
        Vector3 p1 = vertices[index1].pos;
        Vector3 p2 = vertices[index2].pos;

        Vector2 uv0 = vertices[index0].uv;
        Vector2 uv1 = vertices[index1].uv;
        Vector2 uv2 = vertices[index2].uv;

        Vector3 e0 = p1 - p0;
        Vector3 e1 = p2 - p0;

        float u1 = uv1.x - uv0.x;
        float v1 = uv1.y - uv0.y;
        float u2 = uv2.x - uv0.x;
        float v2 = uv2.y - uv0.y;

        float d = 1.0f / (u1 * v2 - u2 * v1);
        Vector3 tangent = d * (e0 * v2 - e1 * v1);

        vertices[index0].tangent = tangent + vertices[index0].tangent;
        vertices[index1].tangent = tangent + vertices[index1].tangent;
        vertices[index2].tangent = tangent + vertices[index2].tangent;
    }
}

void TerrainEditor::MakeComputeData()
{
    // 모양 데이터 받기
    vector<VertexType> vertices = mesh->GetVertices();
    vector<UINT> indices = mesh->GetIndices();

    // 삼각형의 크기 구하기
    triangleSize = indices.size() / 3;

    // 벡터 초기화
    inputs.resize(triangleSize);
    outputs.resize(triangleSize);

    //각 벡터의 내용을 갱신
    for (UINT i = 0; i < triangleSize; ++i)
    {
        UINT index0 = indices[i * 3 + 0];
        UINT index1 = indices[i * 3 + 1];
        UINT index2 = indices[i * 3 + 2];

        inputs[i].v0 = vertices[index0].pos;
        inputs[i].v1 = vertices[index1].pos;
        inputs[i].v2 = vertices[index2].pos;
    }
}

void TerrainEditor::Resize()
{
    MakeMesh();
    MakeNormal();
    MakeTangent();
    MakeComputeData();

    mesh->UpdateVertex();
    mesh->UpdateIndex();

    structuredBuffer->UpdateInput(inputs.data());
}

void TerrainEditor::UpdateHeight()
{
    vector<VertexType>& vertices = mesh->GetVertices();

    for (VertexType& vertex : vertices)
    {
        vertex.normal = {}; 
        vertex.tangent = {};
    }

    MakeNormal();
    MakeTangent();
    MakeComputeData();

    mesh->UpdateVertex();
    structuredBuffer->UpdateInput(inputs.data());
}

void TerrainEditor::AdjustHeight()
{
    // 정점 받기
    vector<VertexType>& vertices = mesh->GetVertices();

    switch (brushType)
    {
    case CIRCLE:
        for (VertexType& vertex : vertices)
        {
            Vector3 pos = Vector3(vertex.pos.x, 0, vertex.pos.z);
            pickingPos.y = 0;

            float distance = Distance(pos, pickingPos);

            if (distance <= brushBuffer->Get().range)
            {
                vertex.pos.y += adjustValue * DELTA;
                vertex.pos.y = Clamp(MIN_HEIGHT, MAX_HEIGHT, vertex.pos.y);
            }
        }
        break;

    case SOFT_CIRCLE:

        for (VertexType& vertex : vertices)
        {
            Vector3 pos = Vector3(vertex.pos.x, 0, vertex.pos.z);
            pickingPos.y = 0;

            float distance = Distance(pos, pickingPos);

            float tmp = adjustValue * max(0, cos(distance / brushBuffer->Get().range));

            if (distance <= brushBuffer->Get().range)
            {
                vertex.pos.y += tmp * DELTA;
                vertex.pos.y = Clamp(MIN_HEIGHT, MAX_HEIGHT, vertex.pos.y);
            }
        }

        break;

    case RECT:

        float size = brushBuffer->Get().range * 0.5f;

        float left = max(0, pickingPos.x - size);
        float right = max(0, pickingPos.x + size);
        float top = max(0, pickingPos.z + size);
        float bottom = max(0, pickingPos.z - size);

        for (UINT z = (UINT)bottom; z <= (UINT)top; ++z)
        {
            for (UINT x = (UINT)left; x <= (UINT)right; ++x)
            {
                UINT index = width * ((height - 1) - z) + x;

                if (index >= vertices.size())
                    continue;

                vertices[index].pos.y += adjustValue * DELTA;
                vertices[index].pos.y = Clamp(MIN_HEIGHT, MAX_HEIGHT, vertices[index].pos.y);
            }
        }
        break;
    }

    mesh->UpdateVertex();
}

void TerrainEditor::AdjustAlpha()
{
    vector<VertexType>& vertices = mesh->GetVertices();

    switch (brushType)
    {
    case TerrainEditor::CIRCLE:
        for (VertexType& vertex : vertices)
        {
            Vector3 pos = Vector3(vertex.pos.x, 0, vertex.pos.z);
            pickingPos.y = 0.0f;

            float distance = Distance(pos, pickingPos);

            if (distance <= brushBuffer->Get().range)
            {
                vertex.alpha[selectMap] += adjustValue * DELTA;
                vertex.alpha[selectMap] = Clamp(0.0f, 1.0f, vertex.alpha[selectMap]);
            }
        }
        break;
    case TerrainEditor::SOFT_CIRCLE:
        for (VertexType& vertex : vertices)
        {
            Vector3 pos = Vector3(vertex.pos.x, 0, vertex.pos.z);
            pickingPos.y = 0.0f;

            float distance = Distance(pos, pickingPos);

            float temp = adjustValue * max(0, cos(distance / brushBuffer->Get().range));

            if (distance <= brushBuffer->Get().range)
            {
                vertex.alpha[selectMap] += temp * DELTA;
                vertex.alpha[selectMap] = Clamp(0.0f, 1.0f, vertex.alpha[selectMap]);
            }
        }
        break;
    case TerrainEditor::RECT:
    {
        float size = brushBuffer->Get().range * 0.5f;

        float left = max(0, pickingPos.x - size);
        float right = max(0, pickingPos.x + size);
        float top = max(0, pickingPos.z + size);
        float bottom = max(0, pickingPos.z - size);

        for (UINT z = (UINT)bottom; z <= (UINT)top; z++)
        {
            for (UINT x = (UINT)left; x <= (UINT)right; x++)
            {
                UINT index = width * (height - z - 1) + x;

                if (index >= vertices.size()) continue;

                vertices[index].alpha[selectMap] += adjustValue * DELTA;
                vertices[index].alpha[selectMap] = Clamp(0.0f, 1.0f, vertices[index].alpha[selectMap]);
            }
        }
    }
    break;
    }

    mesh->UpdateVertex();
}

void TerrainEditor::AdjustMap()
{
}

void TerrainEditor::SaveHeightMap()
{
    if (ImGui::Button("SaveHeight"))
    {
        DIALOG->OpenDialog("SaveHeight", "SaveHeight", ".png", ".");
    }

    if (DIALOG->Display("SaveHeight"))
    {
        if (DIALOG->IsOk())
        {
            string file = DIALOG->GetFilePathName();
            file = file.substr(projectPath.size() + 1, file.size());

            UINT size = width * height * 4;
            uint8_t* pixels = new uint8_t[size];

            vector<VertexType>& vertices = mesh->GetVertices();

            for (UINT i = 0; i < size / 4; ++i)
            {
                float y = vertices[i].pos.y;

                uint8_t height = ((y - MIN_HEIGHT) / (MAX_HEIGHT - MIN_HEIGHT)) * 255;


                pixels[i * 4 + 0] = height;
                pixels[i * 4 + 1] = height; 
                pixels[i * 4 + 2] = height; 
                pixels[i * 4 + 3] = 255;   
            }

            Image image;
            image.width = width;
            image.height = height;
            image.format = DXGI_FORMAT_R8G8B8A8_UNORM;
            image.rowPitch = width * 4;
            image.slicePitch = size;
            image.pixels = pixels;

            SaveToWICFile(image, WIC_FLAGS_FORCE_RGB,
                GetWICCodec(WIC_CODEC_PNG), ToWString(file).c_str());

            delete[] pixels;
        }

        DIALOG->Close();
    }
}

void TerrainEditor::LoadHeightMap()
{
    if (ImGui::Button("LoadHeightMap"))
        DIALOG->OpenDialog("LoadHeightMap", "LoadHeightMap", ".png", ".");

    if (DIALOG->Display("LoadHeightMap"))
    {
        if (DIALOG->IsOk())
        {
            string file = DIALOG->GetFilePathName();
            file = file.substr(projectPath.size() + 1, file.size());

            heightMap = Texture::Add(ToWString(file));

            Resize();
        }

        DIALOG->Close();
    }
}


void TerrainEditor::SaveAlphaMap()
{
    if (ImGui::Button("SaveAlpha"))
        DIALOG->OpenDialog("SaveAlpha", "SaveAlpha", ".png", ".");

    if (DIALOG->Display("SaveAlpha"))
    {
        if (DIALOG->IsOk())
        {
            string file = DIALOG->GetFilePathName();

            file = file.substr(projectPath.size() + 1, file.size());

            UINT size = width * height * 4;
            uint8_t* pixels = new uint8_t[size];

            vector<VertexType> vertices = mesh->GetVertices();

            for (UINT i = 0; i < size / 4; i++)
            {
                pixels[i * 4 + 0] = vertices[i].alpha[0] * 255;
                pixels[i * 4 + 1] = vertices[i].alpha[1] * 255;
                pixels[i * 4 + 2] = vertices[i].alpha[2] * 255;
                pixels[i * 4 + 3] = 255;
            }

            Image image;
            image.width = width;
            image.height = height;
            image.format = DXGI_FORMAT_R8G8B8A8_UNORM;
            image.rowPitch = width * 4;
            image.slicePitch = size;
            image.pixels = pixels;

            SaveToWICFile(image, WIC_FLAGS_FORCE_RGB,
                GetWICCodec(WIC_CODEC_PNG), ToWString(file).c_str());

            delete[] pixels;
        }

        DIALOG->Close();
    }
}

void TerrainEditor::LoadAlphaMap()
{
    if (ImGui::Button("LoadAlpha"))
        DIALOG->OpenDialog("LoadAlpha", "LoadAlpha", ".png", ".");

    if (DIALOG->Display("LoadAlpha"))
    {
        if (DIALOG->IsOk())
        {
            string file = DIALOG->GetFilePathName();

            file = file.substr(projectPath.size() + 1, file.size());

            Texture* alphaMap = Texture::Add(ToWString(file));

            vector<Float4> pixels;
            alphaMap->ReadPixels(pixels);

            vector<VertexType>& vertices = mesh->GetVertices();

            for (UINT i = 0; i < vertices.size(); i++)
            {
                vertices[i].alpha[0] = pixels[i].z;
                vertices[i].alpha[1] = pixels[i].y;
                vertices[i].alpha[2] = pixels[i].x;
                vertices[i].alpha[3] = pixels[i].w;
            }

            mesh->UpdateVertex();
        }

        DIALOG->Close();
    }
}
