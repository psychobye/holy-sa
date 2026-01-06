//
// Created on 20.11.2023.
//

#include "DebugModules.h"
#include "CollisionDebugModule.h"
#include "ModelsDebugModule.h"


void DebugModules::PreRenderUpdate() {
    for (auto& module : m_Modules) {
        module->Update();
    }
}

void DebugModules::Render2D() {
    static bool isOpen = true;
    const notsa::ui::ScopedWindow window{ "Debug Menu", {600.f, 400.f}, isOpen, true };

    for (auto& module : m_Modules) {
        module->RenderMenuEntry();
    }

    for (auto& module : m_Modules) {
        module->RenderWindow();
    }
}

void DebugModules::Render3D() {
    for (auto& module : m_Modules) {
        module->Render3D();
    }
}

void DebugModules::CreateModules(ImGuiContext* ctx) {
    m_ImCtx = ctx;
    // "Tools" menu
    Add<CollisionDebugModule>();
    Add<CModelsDebugModule>();
}
