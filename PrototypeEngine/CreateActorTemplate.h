#pragma once
#include "MeshRenderer.h"
#include "Actor.h"
#include "Typedefs.h"
#include <filesystem>
#include <nlohmann/json.hpp>

//FBXファイルをヒエラルキーにドロップした時にドロップした形式によって
//オブジェクトの生成をする関数をまとめたネームスペース
namespace CreateActorTemplate
{
	//サブメッシュ1つをオブジェクト化関数
	bool CreateOneSubMeshActor(ActorObject* target,uint64_t& id,const string& localID,filesystem::path path,const string& subMeshName);

	uint64_t CreateFBXFileActor(const nlohmann::json& nodeJson,ActorObject* currentParent,filesystem::path path);

	bool CreateBoneActor(const nlohmann::json& nodeJson,ActorObject* currentParent,filesystem::path path);
	bool CreateSkinnedMeshActor(const nlohmann::json& nodeJson,ActorObject* currentParent,filesystem::path path);

	uint64_t CreateSkeletonActor(const nlohmann::json& metaJson,const nlohmann::json& nodeJson,ActorObject* currentParent,filesystem::path path);
}