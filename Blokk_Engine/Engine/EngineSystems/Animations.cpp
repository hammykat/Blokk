#include "EngineClassData.hpp"

void ObjectManager::CreateNewEmptyAnimation(string Name)
{
    AnimNames[Name] = Frames.size();
    Frames.emplace_back();
    FrameWidths.emplace_back();
    FrameHeights.emplace_back();
}

void ObjectManager::AddFramesToAnimation(string Name, vector<Texture2D>& TFrames)
{
    // Helpers
    uint32_t AnimIdx = AnimNames[Name];
    vector<Texture2D>& TargetAnim = Frames[AnimIdx];

    // Insert frames
    TargetAnim.insert(
        TargetAnim.end(),
        TFrames.begin(),
        TFrames.end()
    );

    // Helpers
    vector<uint32_t>& Widths = FrameWidths[AnimIdx];
    vector<uint32_t>& Heights = FrameHeights[AnimIdx];

    // Loop through frames
    for(auto& Frame : TFrames)
    {
        // Add width + height
        Widths.push_back(Frame.width);
        Heights.push_back(Frame.height);
    }
}

void ObjectManager::CreateAnimation(string Name, vector<Texture2D>& TFrames)
{
    uint32_t Idx = Frames.size();
    AnimNames[Name] = Idx;
    Frames.push_back(TFrames);

    FrameWidths.emplace_back();
    FrameHeights.emplace_back();

    // Helpers
    vector<uint32_t>& Widths = FrameWidths[Idx];
    vector<uint32_t>& Heights = FrameHeights[Idx];

    // Loop through frames
    for(auto& Frame : TFrames)
    {
        // Add width + height
        Widths.push_back(Frame.width);
        Heights.push_back(Frame.height);
    }
}