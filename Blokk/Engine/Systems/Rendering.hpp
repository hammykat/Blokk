#include "EngineClassData.hpp"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>


namespace Blokk
{

#ifdef Blokk_Rendering_Enabled

class Renderer
{
public:
    friend class ObjectManager;

    Renderer(ObjectManager& Manager_, SDL_Window* Window_) :
        Manager(Manager_),
        Window(Window_)
    {
        SDLRenderer = SDL_CreateRenderer(Window, nullptr);

        if (!SDLRenderer)
        {
            throw std::runtime_error(
                std::string("Failed to create renderer: ") + SDL_GetError()
            );
        }
    }

    ~Renderer()
    {
        SDL_DestroyRenderer(SDLRenderer);
    }

    void RenderObjects()
    {
        auto& List = Manager.RenderObjectIdxs;

        for (auto& Idx : List)
        {
            // Rendering will go here
        }
    }

    
    [[nodiscard]] 
    double TimeRenderObjects()
    {
        // Start time
        auto StartTime = std::chrono::steady_clock::now();

        RenderObjects();

        // End time
        auto EndTime = std::chrono::steady_clock::now();

        // Calculate total time
        auto TotalTime =
            std::chrono::duration<double, std::milli>(
                EndTime - StartTime
            ).count();

        #ifdef Blokk_Diagnostics
            Manager.RenderTime = TotalTime;
        #endif

        return TotalTime;
    }


private:

    ObjectManager& Manager;
    SDL_Window* Window;
    SDL_Renderer* SDLRenderer;

    SDL_Texture* LoadTexture(const std::string& FilePath)
    {
        SDL_Texture* Texture =
            IMG_LoadTexture(SDLRenderer, FilePath.c_str());

        if (!Texture)
        {
            throw std::runtime_error(
                std::string("Failed to load texture: ") + FilePath +
                " - " + SDL_GetError()
            );
        }

        return Texture;
    }

public:

    void CreateNewEmptyAnimation(const std::string& Name)
    {
        Manager.AnimNames[Name] = Manager.Frames.size();

        Manager.Frames.emplace_back();
        Manager.FrameWidths.emplace_back();
        Manager.FrameHeights.emplace_back();
        Manager.AnimFrameCounts.push_back(0);
    }

    void AddFrameToAnimation(
        const std::string& Name,
        const std::string& FilePath
    )
    {
        uint32_t AnimIdx = Manager.AnimNames[Name];

        SDL_Texture* Texture = LoadTexture(FilePath);

        Manager.Frames[AnimIdx].push_back(Texture);

        float Width;
        float Height;

        SDL_GetTextureSize(
            Texture,
            &Width,
            &Height
        );

        Manager.FrameWidths[AnimIdx].push_back(
            static_cast<uint32_t>(Width)
        );

        Manager.FrameHeights[AnimIdx].push_back(
            static_cast<uint32_t>(Height)
        );

        Manager.AnimFrameCounts[AnimIdx]++;
    }

    void CreateAnimation(
        const std::string& Name,
        const std::vector<std::string>& FilePaths
    )
    {
        CreateNewEmptyAnimation(Name);

        for (const std::string& FilePath : FilePaths)
        {
            AddFrameToAnimation(Name, FilePath);
        }
    }
};

#endif

}