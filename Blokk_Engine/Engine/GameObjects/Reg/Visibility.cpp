/* #include "GameObject.hpp"

// Set to visible
    void SetToVisible() // Assumes that the object was invisible before
    {
        // Update local var
        IsVisible = true;

        // Get into visible
        UpdateEngineData<bool>(
            CommandTypes::Set,
            &EngineObjects->IsVisible
        );
    }

    // Set to invisible
    void SetToInvisible() // Assumes that the object was visible before
    {
        // Update local var
        IsVisible = false;

        // Remove from visible
        
    }

void ObjectManager::Hide() 
{
    // If visible
    if(IsVisible)
    {
        // Set to invisible
        SetToInvisible();
    }
    
    // If not visible, do nothing
}

void ObjectManager::Show() 
{
    // If is already visible
    if(IsVisible)
    {
        // Do nothing
        return;
    }
    else // If hidden
    {
        // Set to visible
        SetToVisible();
    }
}

void ObjectManager::SetVisible(bool Vis) 
{
    // If not visible and setting to visible
    if(!IsVisible && Vis) 
    {
        SetToVisible();
    }
    // If visible and being set to invisible
    elif(IsVisible && !Vis)
    {
        // Set to invisible
        SetToInvisible();
    }

    // Update local var
    IsVisible = Vis;
}

bool ObjectManager::IsVisible() {
    return IsVisible;
}

void ObjectManager::ToggleVisibility() 
{
    // If visible
    if(IsVisible)
    {
        // Set to invisible
        SetToInvisible();
    }
    else // If not visible
    {
        // Set to visible
        SetToVsisible();
    }
}  */