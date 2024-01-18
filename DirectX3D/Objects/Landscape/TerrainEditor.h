#pragma once
class TerrainEditor : public GameObject
{
private:

    enum BrushType
    {
        CIRCLE,      
        SOFT_CIRCLE, 
        RECT,        
    };

    enum EditType
    {
        HEIGHT,
        ALPHA, 
        MAP,   
    };

    typedef VertexUVNormalTangentAlpha VertexType;

    float MIN_HEIGHT = -10.0f;
    float MAX_HEIGHT = +20.0f;

    UINT MAX_SIZE = 256;      

    class BrushBuffer : public ConstBuffer
    {
    private:
        struct Data
        {
            int type = 0;     
            Float3 pickingPos;
            float range = 10; 
            Float3 color = { 1, 1, 0 };
        };
        Data data;

    public:
        BrushBuffer() : ConstBuffer(&data, sizeof(Data)) {}
        Data& Get() { return data; }
    };

    class RayBuffer : public ConstBuffer
    {
    private:
        struct Data
        {
            Float3 pos;       
            UINT triangleSize;

            Float3 dir;       
            float padding;    
        };
        Data data;

    public:
        RayBuffer() : ConstBuffer(&data, sizeof(Data)) {}
        Data& Get() { return data; }
    };

    struct InputDesc
    {
        Float3 v0;
        Float3 v1;
        Float3 v2;
    };

    struct OutputDesc
    {
        int isPicked;
        float distance; 
    };

public:
    TerrainEditor();
    ~TerrainEditor();

    void Update();
    void Render();
    void RenderUI();


    Vector3 Picking();
    bool ComputePicking(Vector3& pos, Vector3 manualPos = Vector3(), Vector3 manualDir = Vector3());

    Vector2 GetTerrainSize() { return { (float)width, (float)height }; }

private:
    void MakeMesh();
    void MakeNormal();
    void MakeTangent(); 
    void MakeComputeData();

    void Resize();
    void UpdateHeight(); 

    void AdjustHeight(); 
    void AdjustAlpha();  
    void AdjustMap();    

    void SaveHeightMap();
    void LoadHeightMap();

    void SaveAlphaMap();
    void LoadAlphaMap();

private:
    string projectPath; 

    UINT width;        
    UINT height;       
    UINT triangleSize; 


    float adjustValue = 10; 
    BrushType brushType = CIRCLE;
    EditType editType = HEIGHT;  

    UINT selectMap = 0;

    Vector3 pickingPos;

    Mesh<VertexType>* mesh;
    BrushBuffer* brushBuffer;
    RayBuffer* rayBuffer;
    StructuredBuffer* structuredBuffer; 

    vector<InputDesc> inputs;
    vector<OutputDesc> outputs;

    Texture* heightMap; 
    Texture* secondMap;
    Texture* thirdMap;

    ComputeShader* computeShader;

    bool editEnabled = false;
};

