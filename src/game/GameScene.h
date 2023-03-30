#pragma once

#include "../Scene.h"
#include "Camera.h"
#include "world/World.h"
#include "../assets/Assets.h"

#include <mutex>

namespace vanguard {
    class GameScene : public Scene {
    public:
        GameScene() = default;

        void init() override;
        void update(float deltaTime) override;
        void buildRenderGraph(RenderGraphBuilder& builder) override;
    private:
        static void loadAssets(Assets& assets);
    private:
        std::mutex m_registryMutex;

        Camera m_camera{};
        World m_world{};

        uint32_t m_lastFrame = 0;
    };
}