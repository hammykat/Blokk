#include <string>
#include <vector>

#include "EngineClassData.hpp"

namespace Blokk 
{

void ObjectManager::CreateNewEmptyAnimation(std::string Name)
{
    AnimNames[Name] = Frames.size();
    Frames.emplace_back();
    FrameWidths.emplace_back();
    FrameHeights.emplace_back();
    AnimFrameCounts.push_back(0);
}

void ObjectManager::AddFramesToAnimation(
    std::string Name,
    std::vector<Texture2D>& TFrames
)
{
    // Helpers
    uint32_t AnimIdx = AnimNames[Name];
    std::vector<Texture2D>& TargetAnim = Frames[AnimIdx];

    // Insert frames
    TargetAnim.insert(
        TargetAnim.end(),
        TFrames.begin(),
        TFrames.end()
    );

    // Helpers
    std::vector<uint32_t>& Widths = FrameWidths[AnimIdx];
    std::vector<uint32_t>& Heights = FrameHeights[AnimIdx];
    uint32_t& AnimSize = AnimFrameCounts[AnimIdx];

    // Loop through frames
    for (auto& Frame : TFrames)
    {
        // Add width + height
        Widths.push_back(Frame.width);
        Heights.push_back(Frame.height);
    }

    AnimSize += TFrames.size();
}

void ObjectManager::CreateAnimation(
    std::string Name,
    std::vector<Texture2D>& TFrames
)
{
    uint32_t Idx = Frames.size();
    AnimNames[Name] = Idx;
    Frames.push_back(TFrames);

    FrameWidths.emplace_back();
    FrameHeights.emplace_back();

    // Helpers
    std::vector<uint32_t>& Widths = FrameWidths[Idx];
    std::vector<uint32_t>& Heights = FrameHeights[Idx];

    // Loop through frames
    for (auto& Frame : TFrames)
    {
        // Add width + height
        Widths.push_back(Frame.width);
        Heights.push_back(Frame.height);
    }

    AnimFrameCounts.push_back(TFrames.size());
}

}