#pragma once

#include "CoffeeEngine/Scripting/ScriptManager.h"
#include <entt/entity/registry.hpp>

#include <functional>
#include <filesystem>

namespace Coffee
{
    class Script
    {
    public:
        Script(const std::filesystem::path& filepath, ScriptingLanguage language)
            : m_Language(language), m_Path(filepath) {}

        std::function<void()> OnCreate;
        std::function<void()> OnUpdate;
        std::function<void()> OnDestroy;

        const std::filesystem::path& GetPath() const { return m_Path; }
        const ScriptingLanguage& GetLanguage() const { return m_Language; }

    private:
        ScriptingLanguage m_Language;
        std::filesystem::path m_Path;
    };

    struct ScriptComponent
    {
        Script script;

        ScriptComponent() = default;
        ScriptComponent(const std::filesystem::path& filepath, ScriptingLanguage language, entt::registry& registry)
            : script(filepath, language)
        {
            registry.on_construct<ScriptComponent>().connect<&ScriptComponent::OnConstruct>();
            registry.on_destroy<ScriptComponent>().connect<&ScriptComponent::OnDestroy>();
        }
        ScriptComponent(const Script& script)
            : script(script) {}

        static void OnConstruct(entt::registry& registry, entt::entity entity)
        {
            auto& scriptComponent = registry.get<ScriptComponent>(entity);
            
            ScriptManager::ExecuteScriptFromFile(scriptComponent.script);
            ScriptManager::BindFunction("OnCreate", scriptComponent.script.OnCreate);
            ScriptManager::BindFunction("OnUpdate", scriptComponent.script.OnUpdate);
            ScriptManager::BindFunction("OnDestroy", scriptComponent.script.OnDestroy);

            scriptComponent.script.OnCreate();
        }

        static void OnDestroy(entt::registry& registry, entt::entity entity)
        {
            auto& scriptComponent = registry.get<ScriptComponent>(entity);
            scriptComponent.script.OnDestroy();
        }
    };
}