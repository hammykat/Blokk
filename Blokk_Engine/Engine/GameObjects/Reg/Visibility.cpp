/* #include "GameObject.hpp"

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