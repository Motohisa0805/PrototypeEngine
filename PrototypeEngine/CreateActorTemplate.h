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
	bool CreateOneSubMeshActor(ActorObject* target,uint64_t& id,const string& localID,filesystem::path path);

	bool CreateFBXFileActor(const nlohmann::json& nodeJson,ActorObject* currentParent,filesystem::path path,vector<uint64_t>& targetIDs);

	bool CreateSkinnedMeshActor(const nlohmann::json& nodeJson,ActorObject* currentParent,filesystem::path path,vector<uint64_t>& targetIDs);

	bool CreateSkeletonActor(const nlohmann::json& nodeJson,ActorObject* currentParent,filesystem::path path,vector<uint64_t>& targetIDs);
}