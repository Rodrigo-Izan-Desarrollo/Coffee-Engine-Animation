#pragma once

#include "CoffeeEngine/Core/DataStructures/Octree.h"
#include "CoffeeEngine/Events/Event.h"
#include "CoffeeEngine/Renderer/EditorCamera.h"
#include "CoffeeEngine/Scene/SceneTree.h"
#include "entt/entity/fwd.hpp"

#include <entt/entt.hpp>
#include <filesystem>
#include <string>

namespace Coffee {

    /**
     * @defgroup scene Scene
     * @{
     */

    class Entity;
    class Model;

    /**
     * @brief Class representing a scene.
     * @ingroup scene
     */
    class Scene
    {
    public:
        /**
         * @brief Constructor for Scene.
         */
        Scene();

        /**
         * @brief Default destructor.
         */
        ~Scene() = default;

        //Scene(Ref<Scene> other);

        /**
         * @brief Create an entity in the scene.
         * @param name The name of the entity.
         * @return The created entity.
         */
        Entity CreateEntity(const std::string& name = std::string());

        /**
         * @brief Destroy an entity in the scene.
         * @param entity The entity to destroy.
         */
        void DestroyEntity(Entity entity);

        /**
         * @brief Initialize the scene.
         */
        void OnInitEditor();
        void OnInitRuntime();

        /**
         * @brief Update the scene in editor mode.
         * @param camera The editor camera.
         * @param dt The delta time.
         */
        void OnUpdateEditor(EditorCamera& camera, float dt);

        /**
         * @brief Update the scene in runtime mode.
         * @param dt The delta time.
         */
        void OnUpdateRuntime(float dt);

        /**
         * @brief Handle an event in the scene.
         * @param e The event.
         */
        void OnEvent(Event& e);

        /**
         * @brief Exit the scene.
         */
        void OnExitEditor();
        void OnExitRuntime();

        template<typename... Components>
        auto GetAllEntitiesWithComponents()
        {
            return m_Registry.view<Components...>();
        }

        /**
         * @brief Load a scene from a file.
         * @param path The path to the file.
         * @return The loaded scene.
         */
        static Ref<Scene> Load(const std::filesystem::path& path);

        /**
         * @brief Save a scene to a file.
         * @param path The path to the file.
         * @param scene The scene to save.
         */
        static void Save(const std::filesystem::path& path, Ref<Scene> scene);

        const std::filesystem::path& GetFilePath() { return m_FilePath; }
    private:
        entt::registry m_Registry;
        Scope<SceneTree> m_SceneTree;
        Octree<Ref<Mesh>> m_Octree;

        // Temporal: Scenes should be Resources and the Base Resource class already has a path variable.
        std::filesystem::path m_FilePath;

        friend class Entity;
        friend class SceneTree;
        friend class SceneTreePanel;

        //REMOVE PLEASE, THIS IS ONLY TO TEST THE OCTREE!!!!
        friend class EditorLayer;
    };

    /**
     * @brief Add a model to the scene tree.
     * @param scene The scene.
     * @param model The model to add.
     */
    void AddModelToTheSceneTree(Scene* scene, Ref<Model> model);

    /** @} */ // end of scene group
}