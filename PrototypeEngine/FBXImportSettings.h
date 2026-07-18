#pragma once


enum MeshCompression
{
    Off,
    Low,
    Medium,
    High
};

enum OptimizeMesh
{
    Nothing,
    Everything,
    PolygonOrder,
    VertexOrder
};

struct ModelImportSettings
{
    float           sScaleFactory       = 1.0f;
    bool            sConvertUnits       = true;
    bool            sBakeAxisConversion = false;
    bool            sImportBlendShapes  = true;
    bool            sImportDeformPercent = true;
    bool            sImportVisibility    = true;
    bool            sImportCameras       = true;
    bool            sImportLights        = true;
    bool            sPreserveHierarchy   = false;
    bool            sSortHierarchyByName = true;

    MeshCompression sMeshCompressionType = MeshCompression::Off;
    bool            sRead_Write          = false;
    //ビット操作に変更予定
    OptimizeMesh    sOptimizeMesh        = OptimizeMesh::Everything;
    bool            sGenerateColliders = false;

    //後々追加...
};

struct RigImportSettings
{
};

struct AnimationImportSettings
{
};

struct MaterialsImportSettings
{
};

struct AllImportSettings
{
    ModelImportSettings     sModel;
    RigImportSettings       sRig;
    AnimationImportSettings sAnimation;
    MaterialsImportSettings sMaterials;
};