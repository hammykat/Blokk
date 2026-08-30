#include "GameObject.hpp"

namespace Blokk {

void GameObject::SetFrameNum(uint32_t FrameNum)
{
    UpdateEngineData<uint32_t>(
        CommandTypes::Set,
        &EngineObjects->FrameNums,
        EngineIdx,
        FrameNum
    );
}

void GameObject::SetAnimation(string AnimName)
{
    auto It = EngineObjects->AnimNames.find(AnimName);

    if (It == EngineObjects->AnimNames.end())
        return;

    UpdateEngineData<uint32_t>(
        CommandTypes::Set,
        &EngineObjects->AnimNums,
        EngineIdx,
        It->second
    );
}

}