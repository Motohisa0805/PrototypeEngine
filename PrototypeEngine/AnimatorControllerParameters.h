#pragma once
#include "Math.h"
#include "Typedefs.h"

namespace fs = filesystem;

struct AnimInfo
{
    filesystem::path sBinayPath;
    bool             sIsLoop     = false;
    bool             sRootMotion = false;
};

// パラメータの種類
enum class AnimParamType
{
    sFloat,
    sInt,
    sBool,
    sTrigger
};
// Animatorに渡すパラメーター
struct AnimParameter
{
    string        sName = "";
    AnimParamType sType;
    float         sDefaultFloat = 0.0f;
    bool          sDefaultBool  = false;
    int           sDefaultInt   = 0;
    bool          sDefaultTrigger = false;
};
enum class AnimNodeType
{
    sNormal,
    sEntry,
    sAnyState,
    sExit
};
// 状態ノード
struct AnimState
{
    string          sStateName;
    AnimNodeType    sNodeType = AnimNodeType::sNormal;
    AnimInfo        sAnimInfo;
    float           sPlaybackSpeed = 1.0f;
    //描画座標
    Vector2         sPos = Vector2::Zero;
};
// 遷移の条件
struct AnimCondition
{
    string sParameterName;
    string sMode;
    float  sThreshold;
};
// トランジション
struct AnimTransition
{
    string                sFromState;
    string                sToState;
    float                 sBlendDuration;
    vector<AnimCondition> sConditions;
};

struct AnimatorControllerParameters
{
    string                 sDefaultState;
    vector<AnimParameter>  sParameters;
    vector<AnimState>      sStates;
    vector<AnimTransition> sTransitions;
};

// enumの文字列相互変換
NLOHMANN_JSON_SERIALIZE_ENUM(AnimParamType,
                             {
                                 {AnimParamType::sFloat, "Float"},
                                 {AnimParamType::sInt, "Int"},
                                 {AnimParamType::sBool, "Bool"},
                                 {AnimParamType::sTrigger, "Trigger"},
                             })

inline void to_json(nlohmann::json& j, const AnimParameter& p)
{
    j = nlohmann::json{{"name", p.sName},
                       {"type", p.sType},
                       {"defaultFloat", p.sDefaultFloat},
                       {"defaultBool", p.sDefaultBool}};
}

inline void to_json(nlohmann::json& j, const AnimState& s)
{
    j = nlohmann::json{{"name", s.sStateName},
                       {"nodeType", (int)s.sNodeType},
                       {"animPath", s.sAnimInfo.sBinayPath.string()},
                       {"isLoop", s.sAnimInfo.sIsLoop},
                       {"rootMotion", s.sAnimInfo.sRootMotion},
                       {"playbackSpeed", s.sPlaybackSpeed},
                       {"posX", s.sPos.x},
                       {"posY", s.sPos.y}};
}

inline void to_json(nlohmann::json& j, const AnimCondition& c)
{
    j = nlohmann::json{{"parameter", c.sParameterName},
                       {"mode", c.sMode},
                       {"threshold", c.sThreshold}};
}

inline void to_json(nlohmann::json& j, const AnimTransition& t)
{
    j = nlohmann::json{
        {"from", t.sFromState},
        {"to", t.sToState},
        {"blendDuration", t.sBlendDuration},
        {"conditions", t.sConditions},
    };
}

inline void to_json(nlohmann::json& j, const AnimatorControllerParameters& p)
{
    j = nlohmann::json{
        {"defaultState", p.sDefaultState},
        {"parameters", p.sParameters},
        {"states", p.sStates},
        {"transitions", p.sTransitions},
    };
}

inline void from_json(const nlohmann::json& j, AnimParameter& p);
inline void from_json(const nlohmann::json& j, AnimState& s);
inline void from_json(const nlohmann::json& j, AnimCondition& c);
inline void from_json(const nlohmann::json& j, AnimTransition& t);
inline void from_json(const nlohmann::json& j, AnimatorControllerParameters& p);

inline void from_json(const nlohmann::json& j, AnimParameter& p)
{
    j.at("name").get_to(p.sName);
    j.at("type").get_to(p.sType);
    j.at("defaultFloat").get_to(p.sDefaultFloat);
    j.at("defaultBool").get_to(p.sDefaultBool);
}

inline void from_json(const nlohmann::json& j, AnimState& s)
{
    j.at("name").get_to(s.sStateName);
    s.sNodeType             = j.at("nodeType").get<AnimNodeType>();
    s.sAnimInfo.sBinayPath       = j.at("animPath").get<string>();
    s.sAnimInfo.sIsLoop     = j.at("isLoop").get<bool>();
    s.sAnimInfo.sRootMotion = j.at("rootMotion").get<bool>();
    s.sPlaybackSpeed        = j.at("playbackSpeed").get<float>();

    if (j.contains("posX") && j.contains("posY"))
    {
        s.sPos.x = j.at("posX").get<float>();
        s.sPos.y = j.at("posY").get<float>();
    }
}

inline void from_json(const nlohmann::json& j, AnimCondition& c)
{
    j.at("parameter").get_to(c.sParameterName);
    j.at("mode").get_to(c.sMode);
    j.at("threshold").get_to(c.sThreshold);
}

inline void from_json(const nlohmann::json& j, AnimTransition& t)
{
    j.at("from").get_to(t.sFromState);
    j.at("to").get_to(t.sToState);
    j.at("blendDuration").get_to(t.sBlendDuration);
    if (j.contains("conditions"))
    {
        j.at("conditions").get_to(t.sConditions);
    }
}

inline void from_json(const nlohmann::json& j, AnimatorControllerParameters& p)
{
    j.at("defaultState").get_to(p.sDefaultState);
    if (j.contains("parameters"))
        j.at("parameters").get_to(p.sParameters);
    if (j.contains("states"))
        j.at("states").get_to(p.sStates);
    if (j.contains("transitions"))
        j.at("transitions").get_to(p.sTransitions);
}