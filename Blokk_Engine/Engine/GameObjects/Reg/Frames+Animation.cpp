#include "GameObject.hpp"

void SetFrameNum(uint32_t FrameNum)
{
    UpdateEngineData(
        CommandTypes::Set,
        (IsVisible)? &EngineObjects->VisibleFrameNums : 
            &EngineObjects->InvisibleFrameNums,
        EngineVisIdx,
        FrameNum
    )
}

void SetAnimation(string AnimName)
{
    auto It = EngineObjects->AnimNames.find(AnimName);

    if (It == EngineObjects->AnimNames.end())
        return;

    UpdateEngineData(
        CommandTypes::Set,
        IsVisible
            ? &EngineObjects->VisibleAnimNums
            : &EngineObjects->InvisibleAnimNums,
        EngineVisIdx,
        It->second
    );
}