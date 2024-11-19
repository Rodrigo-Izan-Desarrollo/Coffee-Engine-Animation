#include "Scene.h"

#include "CoffeeEngine/Core/Base.h"
#include "CoffeeEngine/Core/DataStructures/Octree.h"
#include "CoffeeEngine/Renderer/DebugRenderer.h"
#include "CoffeeEngine/Renderer/EditorCamera.h"
#include "CoffeeEngine/Renderer/Material.h"
#include "CoffeeEngine/Renderer/Renderer.h"
#include "CoffeeEngine/Renderer/Shader.h"
#include "CoffeeEngine/Scene/Components.h"
#include "CoffeeEngine/Scene/Entity.h"
#include "CoffeeEngine/Scene/PrimitiveMesh.h"
#include "CoffeeEngine/Scene/SceneCamera.h"
#include "CoffeeEngine/Scene/SceneTree.h"
#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"
#include "entt/entity/snapshot.hpp"
#include <cstdint>
#include <string>
#include <tracy/Tracy.hpp>

#include <cereal/archives/json.hpp>
#include <fstream>

namespace Coffee {

    //TEMPORAL
    static Ref<Material> missingMaterial;

    static Octree octree({glm::vec3(-10.0f), glm::vec3(10.0f)});

    static Entity e1;
    static Entity e2;

    Scene::Scene()
    {
        m_SceneTree = CreateScope<SceneTree>(this);
    }

    Entity Scene::CreateEntity(const std::string& name)
    {
        ZoneScoped;

        Entity entity = { m_Registry.create(), this };
        entity.AddComponent<TransformComponent>();
        auto& nameTag = entity.AddComponent<TagComponent>();
        nameTag.Tag = name.empty() ? "Entity" : name;
        entity.AddComponent<HierarchyComponent>();
        return entity;
    }

    void Scene::DestroyEntity(Entity entity)
    {
        auto& hierarchyComponent = m_Registry.get<HierarchyComponent>(entity);
        auto curr = hierarchyComponent.m_First;

        while(curr != entt::null)
        {
            Entity e{curr, this};
            DestroyEntity(e);
            curr = m_Registry.get<HierarchyComponent>(curr).m_Next;
        }

        m_Registry.destroy((entt::entity)entity);
    }

    void Scene::OnInit()
    {
        ZoneScoped;

        Entity light = CreateEntity("Directional Light");
        light.AddComponent<LightComponent>().Color = {1.0f, 0.9f, 0.85f};
        light.GetComponent<TransformComponent>().Position = {0.0f, 0.8f, -2.1f};
        
        Entity camera = CreateEntity("Camera");
        camera.AddComponent<CameraComponent>();

        Ref<Shader> missingShader = Shader::Create("assets/shaders/MissingShader.vert", "assets/shaders/MissingShader.frag");
        missingMaterial = CreateRef<Material>(missingShader);

        // TEST -------------------------

        octree.Insert(glm::vec3(0.0f), glm::vec3(0.0f));
        octree.Insert(glm::vec3(1.0f), glm::vec3(1.0f));
        octree.Insert(glm::vec3(2.0f), glm::vec3(2.0f));
        octree.Insert(glm::vec3(3.0f), glm::vec3(3.0f));
        octree.Insert(glm::vec3(4.0f), glm::vec3(4.0f));
        octree.Insert(glm::vec3(5.0f), glm::vec3(5.0f));
        octree.Insert(glm::vec3(6.0f), glm::vec3(6.0f));
        octree.Insert(glm::vec3(7.0f), glm::vec3(7.0f));
        octree.Insert(glm::vec3(8.0f), glm::vec3(8.0f));
        octree.Insert(glm::vec3(9.0f), glm::vec3(9.0f));
        octree.Insert(glm::vec3(10.0f), glm::vec3(10.0f));

        // static Octree octree({glm::vec3(-10.0f), glm::vec3(10.0f)});
        // Static is a cube that goes from x -10 y -10 z -10 to x 10 y 10 z 10
        // Fill it with 10 random objects

        /*
        for(int i = 0; i < 10; i++)
        {
            octree.Insert(glm::vec3((float)rand() / RAND_MAX * 20 - 10, (float)rand() / RAND_MAX * 20 - 10, (float)rand() / RAND_MAX * 20 - 10), glm::vec3((float)rand() / RAND_MAX * 20 - 10, (float)rand() / RAND_MAX * 20 - 10, (float)rand() / RAND_MAX * 20 - 10));
        }
        */


        // Primitive

        e1 = CreateEntity("e1");
        e1.AddComponent<MeshComponent>(PrimitiveMesh::CreateCube());

        e2 = CreateEntity("e2");
        e2.AddComponent<MeshComponent>(PrimitiveMesh::CreateCube());
        e2.GetComponent<TransformComponent>().Position = {3.0f, 0.0f, 0.0f};

        COFFEE_INFO("asd");
    }

    void Scene::OnUpdateEditor(EditorCamera& camera, float dt)
    {
        ZoneScoped;

        m_SceneTree->Update();

        Renderer::BeginScene(camera);

        octree.Update();

        // log if e1 and e2 aabb are intersecting
        // COFFEE_INFO("e1 and e2 AABBs intersecting: {0}", Mesh::Intersects(e1.GetComponent<MeshComponent>().mesh->GetAABB(), e1.GetComponent<TransformComponent>().Position, e2.GetComponent<MeshComponent>().mesh->GetAABB(), e2.GetComponent<TransformComponent>().Position));

        // Get all entities with ModelComponent and TransformComponent
        auto view = m_Registry.view<MeshComponent, TransformComponent>();

        // Loop through each entity with the specified components
        for (auto& entity : view)
        {
            // Get the ModelComponent and TransformComponent for the current entity
            auto& meshComponent = view.get<MeshComponent>(entity);
            auto& transformComponent = view.get<TransformComponent>(entity);
            auto materialComponent = m_Registry.try_get<MaterialComponent>(entity);

            Ref<Mesh> mesh = meshComponent.GetMesh();
            Ref<Material> material = (materialComponent and materialComponent->material) ? materialComponent->material : missingMaterial;
            
            Renderer::Submit(material, mesh, transformComponent.GetWorldTransform(), (uint32_t)entity);
        }

        //Get all entities with LightComponent and TransformComponent
        auto lightView = m_Registry.view<LightComponent, TransformComponent>();

        //Loop through each entity with the specified components
        for(auto& entity : lightView)
        {
            auto& lightComponent = lightView.get<LightComponent>(entity);
            auto& transformComponent = lightView.get<TransformComponent>(entity);

            lightComponent.Position = transformComponent.GetWorldTransform()[3];
            lightComponent.Direction = glm::normalize(glm::vec3(-transformComponent.GetWorldTransform()[1]));

            Renderer::Submit(lightComponent);
        }

        Renderer::EndScene();
    }

    void Scene::OnUpdateRuntime(float dt)
    {
        ZoneScoped;

        m_SceneTree->Update();

        Camera* camera = nullptr;
        glm::mat4 cameraTransform;
        auto cameraView = m_Registry.view<TransformComponent, CameraComponent>();
        for(auto entity : cameraView)
        {
            auto [transform, cameraComponent] = cameraView.get<TransformComponent, CameraComponent>(entity);
            
            //TODO: Multiple cameras support (for now, the last camera found will be used)
            camera = &cameraComponent.Camera;
            cameraTransform = transform.GetWorldTransform();
        }

        if(!camera)
        {
            COFFEE_ERROR("No camera entity found!");

            SceneCamera sceneCamera;
            camera = &sceneCamera;

            cameraTransform = glm::mat4(1.0f);
        }

        //TODO: Add this to a function bc it is repeated in OnUpdateEditor
        Renderer::BeginScene(*camera, cameraTransform);
        
        // Get all entities with ModelComponent and TransformComponent
        auto view = m_Registry.view<MeshComponent, TransformComponent>();

        // Loop through each entity with the specified components
        for (auto& entity : view)
        {
            // Get the ModelComponent and TransformComponent for the current entity
            auto& meshComponent = view.get<MeshComponent>(entity);
            auto& transformComponent = view.get<TransformComponent>(entity);
            auto materialComponent = m_Registry.try_get<MaterialComponent>(entity);

            Ref<Mesh> mesh = meshComponent.GetMesh();
            Ref<Material> material = (materialComponent and materialComponent->material) ? materialComponent->material : missingMaterial;
            
            Renderer::Submit(material, mesh, transformComponent.GetWorldTransform());
        }

        //Get all entities with LightComponent and TransformComponent
        auto lightView = m_Registry.view<LightComponent, TransformComponent>();

        //Loop through each entity with the specified components
        for(auto& entity : lightView)
        {
            auto& lightComponent = lightView.get<LightComponent>(entity);
            auto& transformComponent = lightView.get<TransformComponent>(entity);

            lightComponent.Position = transformComponent.GetWorldTransform()[3];
            lightComponent.Direction = glm::normalize(glm::vec3(-transformComponent.GetWorldTransform()[1]));

            Renderer::Submit(lightComponent);
        }

        Renderer::EndScene();
    }

    void Scene::OnEvent(Event& e)
    {
        ZoneScoped;
    }

    void Scene::OnExit()
    {
        ZoneScoped;
    }

    Ref<Scene> Scene::Load(const std::filesystem::path& path)
    {
        ZoneScoped;

        Ref<Scene> scene = CreateRef<Scene>();

        std::ifstream sceneFile(path);
        cereal::JSONInputArchive archive(sceneFile);

        entt::snapshot_loader{scene->m_Registry}
            .get<entt::entity>(archive)
            .get<TagComponent>(archive)
            .get<TransformComponent>(archive)
            .get<HierarchyComponent>(archive)
            .get<CameraComponent>(archive)
            //.get<MeshComponent>(archive)
            //.get<MaterialComponent>(archive)
            .get<LightComponent>(archive);

        return scene;
    }

    void Scene::Save(const std::filesystem::path& path, Ref<Scene> scene)
    {
        ZoneScoped;

        std::ofstream sceneFile(path);
        cereal::JSONOutputArchive archive(sceneFile);

        //archive(*scene);

        //TEMPORAL
        entt::snapshot{scene->m_Registry}
            .get<entt::entity>(archive)
            .get<TagComponent>(archive)
            .get<TransformComponent>(archive)
            .get<HierarchyComponent>(archive)
            .get<CameraComponent>(archive)
            //.get<MeshComponent>(archive)
            //.get<MaterialComponent>(archive)
            .get<LightComponent>(archive);
    }

    // Is possible that this function will be moved to the SceneTreePanel but for now it will stay here
    void AddModelToTheSceneTree(Scene* scene, Ref<Model> model)
    {
        static Entity parent;

        Entity modelEntity = scene->CreateEntity(model->GetName());

        if((entt::entity)parent != entt::null)modelEntity.SetParent(parent);
        modelEntity.GetComponent<TransformComponent>().SetLocalTransform(model->GetTransform());

        auto& meshes = model->GetMeshes();
        bool hasMultipleMeshes = meshes.size() > 1;

        for(auto& mesh : meshes)
        {
            Entity entity = hasMultipleMeshes ? scene->CreateEntity(mesh->GetName()) : modelEntity;

            entity.AddComponent<MeshComponent>(mesh);

            if(mesh->GetMaterial())
            {
                entity.AddComponent<MaterialComponent>(mesh->GetMaterial());
            }

            if(hasMultipleMeshes)
            {
                entity.SetParent(modelEntity);
            }
        }

        for(auto& c : model->GetChildren())
        {
            parent = modelEntity;
            AddModelToTheSceneTree(scene, c);
        }
    }

}
