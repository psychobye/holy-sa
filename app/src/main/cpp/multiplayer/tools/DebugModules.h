//
// Created on 20.11.2023.
//
#pragma once

#include "DebugModule.h"

class DebugModules {
public:
    //! Pre-Render updates
    static void PreRenderUpdate();

    //! Process stuff (Including rendering)
    static void Render2D();

    //! Process 3D rendering
    static void Render3D();

    //! Creates all modules
    static void CreateModules(ImGuiContext* ctx);

    //! Render menu bar stuff (FPS, etc)
    void RenderMenuBarInfo();

    //! Add a new module
    template<typename T>
    static void Add() {
        if constexpr(std::is_base_of<DebugModule, T>::value) {
            auto& module = m_Modules.emplace_back(std::make_unique<T>());
            module->OnImGuiInitialised(m_ImCtx);
        } else {
        }
    }


private:
    static inline std::vector<std::unique_ptr<DebugModule>> m_Modules{};
    static inline ImGuiContext*                             m_ImCtx{};
};
