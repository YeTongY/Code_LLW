/*
 * LLW Project - Main Entry Point
 * This is a test program to verify that Raylib and the build environment work correctly
 */

#include "raylib.h"

int main(void)
{
    // Initialize window
    const int screenWidth = 800;
    const int screenHeight = 600;
    
    InitWindow(screenWidth, screenHeight, "LLW Project - Test Window");
    
    // Set target FPS to 60
    SetTargetFPS(60);
    
    // Main game loop
    while (!WindowShouldClose())  // Detect ESC key or window close button
    {
        // Update logic
        // (no logic code yet)
        
        // Draw
        BeginDrawing();
        
            ClearBackground(RAYWHITE);  // Clear screen to white
            
            // Draw title text
            DrawText("Welcome to LLW Project!", 190, 200, 30, LIGHTGRAY);
            DrawText("A JRPG Game Demo", 250, 250, 25, GRAY);
            
            // Draw information
            DrawText("If you see this window, compilation was successful!", 120, 350, 20, DARKGRAY);
            DrawText("Press ESC to exit", 280, 400, 20, DARKGRAY);
            
            // Draw test graphics (blue circle)
            DrawCircle(400, 500, 30, BLUE);
            DrawText("This is a circle rendered by Raylib", 230, 540, 16, DARKBLUE);
        
        EndDrawing();
    }
    
    // Close window and cleanup resources
    CloseWindow();
    
    return 0;
}
