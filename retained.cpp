#include <raylib.h>
#include <raymath.h>

#include <iostream>
#include <string>
#include <vector>


int width = 800, height = 600;

// Generic UI component
struct UIComponent
{
    // Rectangle reprsenting the bounds of the UI component
    Rectangle bounds;

    // Draws this particular UI component
    // Set as abstract so that child widgets will implement this for us
    virtual void Draw() = 0;

    // Handles a mouse click event
    // Set as abstract so that child widgets will implement this for us
    // Returns a boolean indicating whether this UI component successfully handled the event
    virtual bool HandleClick(Vector2 click_position) = 0;
};

// Generic UI component that can contain other UI components as children
struct UIContainer : public UIComponent
{
    std::vector<UIComponent*> children;

    // Adds a child to the container
    void AddChild(UIComponent* child)
    {
        children.push_back(child);
    }

    // Draw
    void Draw() override
    {
        // Since we are just a container for other widgets, we simply
        // call the draw function of other widgets.
        // This results in a pre-order traversal when we also draw child widgets that are also containers
        for (size_t i = 0; i < children.size(); ++i)
        {
            children[i]->Draw();
        }
    }

    // Handles a mouse click event
    // Returns a boolean indicating whether this UI component successfully handled the event
    bool HandleClick(Vector2 click_position) override
    {
        // Since we are just a container for other widgets, we call the HandleClick function of our child widgets
        // Since later children are drawn last, we do the opposite of draw where we start from the last child.
        // This results in a pre-order traversal but in the reverse order.
        for (size_t i = children.size(); i > 0; --i)
        {
            // If a child already handles the click event, we instantly return so no more child widgets handle the click
            if (children[i - 1]->HandleClick(click_position))
            {
                return true;
            }
        }

        return false;
    }
};

// Button widget
struct Button : public UIComponent
{
    // Text displayed by the button
    std::string text;
    void (*func)();
    bool isActive = true;
    // Draw
    void Draw() override
    {
        DrawRectangleRec(bounds, GRAY);
        DrawText(text.c_str(), bounds.x, bounds.y, 14, BLACK);
    }

    // Handle mouse click
    // Returns a boolean indicating whether this UI component successfully handled the event
    bool HandleClick(Vector2 click_position) override
    {
        // Check if the mouse click position is within our bounds
        if (CheckCollisionPointRec(click_position, bounds) && isActive)
        {
            func();
            return true;
        }

        return false;
    }
};

struct CheckBox : public UIComponent
{
    bool isChecked = false;
    std::string text;
    // Draw
    void Draw() override
    {
        DrawRectangleRec(bounds, GRAY);
        if(isChecked == true){
            DrawRectangleRec(bounds, BLACK);
        }
        DrawText(text.c_str(), bounds.x + (bounds.width) + 10, bounds.y, 14, BLACK);
    }

    void Check(){
        isChecked = !isChecked;
    }

    // Handle mouse click
    // Returns a boolean indicating whether this UI component successfully handled the event
    bool HandleClick(Vector2 click_position) override
    {
        // Check if the mouse click position is within our bounds
        if (CheckCollisionPointRec(click_position, bounds))
        {
            Check();
            return true;
        }

        return false;
    }
};

// Text display widget
struct Label : public UIComponent
{
    // Text to be displayed
    std::string text;

    // Draw
    void Draw() override
    {
        DrawText(text.c_str(), bounds.x, bounds.y, 14, BLACK);
    }

    // Handle mouse click
    // Returns a boolean indicating whether this UI component successfully handled the event
    bool HandleClick(Vector2 click_position) override
    {
        // Always return false since we're not going to handle click events for this particular widget
        // (unless you have to)
        return false;
    }
};

// Struct to encapsulate our UI library
struct UILibrary
{
    // Root container
    UIContainer root_container;

    // Updates the current UI state
    void Update()
    {
        // If the left mouse button was released, we handle the click from the root container
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            root_container.HandleClick(GetMousePosition());
        }
    }

    // Draw
    void Draw()
    {
        root_container.Draw();
    }
};

void SayHi(){
    std::cout << "Hello!" << std::endl;
}

void SayDie(){
    std::cout << "DIE!" << std::endl;
}

void changeRes800x600(){
    SetWindowSize(800, 600);
}

void changeRes1280x720(){
    SetWindowSize(1280, 720);
}

void changeRes1366x768(){
    SetWindowSize(1366, 768);
}

int main()
{
    int WINDOW_WIDTH = width, WINDOW_HEIGHT = height;
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Retained Mode");
    SetTargetFPS(60);

    UILibrary ui_library;
    ui_library.root_container.bounds = { 10, 10, 600, 500 };

    

    Button button;
    button.text = "800x600";
    button.bounds = { 120, 10, 80, 40 };
    button.func = changeRes800x600;
    ui_library.root_container.AddChild(&button);

    Button button2;
    button2.text = "1280x720";
    button2.bounds = { 210, 10, 80, 40 };
    button2.func = changeRes1280x720;
    ui_library.root_container.AddChild(&button2);

    Button button3;
    button3.text = "1366x768";
    button3.bounds = { 300, 10, 80, 40 };
    button3.func = changeRes1366x768;
    ui_library.root_container.AddChild(&button3);

    Label label;
    label.text = "This is a label";
    label.bounds = { 10, 20, 100, 40 };
    ui_library.root_container.AddChild(&label);

    CheckBox testBox;
    testBox.text = "Lock Screen Size";
    testBox.bounds = {120, 100, 20, 20};
    ui_library.root_container.AddChild(&testBox);

    while (!WindowShouldClose())
    {
        if(testBox.isChecked == true){
            button.isActive = false;
            button2.isActive = false;
            button3.isActive = false;
        }

        if(testBox.isChecked == false){
            button.isActive = true;
            button2.isActive = true;
            button3.isActive = true;
        }
        ui_library.Update();

        ClearBackground(WHITE);
        BeginDrawing();
        ui_library.Draw();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
