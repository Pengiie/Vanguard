#include "Assets.h"

namespace vanguard {
    Assets::Assets() {
        addLoader("txt", [](const File& file) {
            auto data = file.load(false);

            return Asset(std::string(data.begin(), data.end()));
        });
        addLoader("glsl", [](const File& file) {
            return loadSpirVShader(file);
        });
    }

    void Assets::addLoader(const std::string& extension, const std::function<Asset(const File&)>& loader) {
        m_loaders[extension] = loader;
    }

    void Assets::load(const std::string& filePath) {
        File file(filePath);

        m_tasks.emplace(file.path(), std::async(std::launch::async, [this, file]() {
            if(m_assets.find(file.path()) != m_assets.end()) {
                return;
            }

            auto it = m_loaders.find(file.extension());
            if(it == m_loaders.end()) {
                ERROR("No loader for file extension: {}", file.extension());
                return;
            }

            try {
                Asset asset = it->second(file);

                std::lock_guard lock(m_mutex);
                m_assets.emplace(file.path(), std::move(asset));
            } catch (std::exception& e) {
                ERROR("Failed to load asset: {}", file.path());
            }
        }));
    }

    void Assets::finishLoading() {
        for (auto& [_, task] : m_tasks) {
            task.wait();
        }
    }
}