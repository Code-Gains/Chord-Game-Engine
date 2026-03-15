#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <optional>

//#include <cereal/cereal.hpp>
// #include "EcsSerialization.hpp"
// #include "CerealRegistry.hpp"

// #include "Archetype.hpp"
// #include "System.hpp"
// #include "Entity.hpp"
// #include "ComponentVector.hpp"
// #include "TransformComponent.hpp"
// #include "MaterialComponent.hpp"
// #include "MeshComponent.hpp"
#include "Slotmap.h"
#include "Entity.h"
#include "Component.h"


// ReSharper disable once CppInconsistentNaming
struct GLFWwindow;

class Core; // forward declaration (used after all systems initialized)

class Ecs
{
	Core* _core; // no owning
	GLFWwindow* _glfwWindow; // non owning

	//std::vector<std::unique_ptr<System>> _systems;
	//std::unordered_map<ComponentSignature, std::unique_ptr<Archetype>> _signatureToArchetype;
	// This could be removed TODO (watch stuff)
	std::unordered_map<Entity, ComponentSignature> _entityToSignature;
	// --- IDs ---
	//std::uint32_t _nextEntityId = 0;

public:
	// --- Constructors ---
	Ecs() {};
	~Ecs() {};

	// --- Core Loops ---
	//void Render();
	//void Update(float deltaTime);
	//void PeriodicUpdate(float deltaTime);

	// System Resources
	//void SetRenderingApplication(RenderingApplication3D* renderingApplication);
	//void SetGlfwWindow(GLFWwindow* window);
	//GLFWwindow* GetGlfwWindow();
	//RenderingApplication3D* GetRenderingApplication3D();

	// --- Archetype Management ---
	//size_t GetArchetypeCount() const;

	// Archetype* GetEntityArchetype(Entity entity) const;
	// Archetype* GetSignatureArchetype(const ComponentSignature& signature) const;
	// Archetype* GetOrCreateArchetype(const ComponentSignature& signature);

	// ComponentSignature GenerateNewSignature(std::optional<ComponentSignature> current, ComponentType componentType) const;
	// ComponentSignature GenerateNewSignature(ComponentSignature current, ComponentType componentType) const;

	// const std::unordered_map<ComponentSignature, std::unique_ptr<Archetype>>& GetSignatureToArchetype() const;

	// --- Entity Management ---
	size_t GetEntityCount() const;

	// std::unordered_map<Entity, ComponentSignature> GetAllEntities() const;

	// Entity GetNextEntityId() const;
	// const std::optional<ComponentSignature> GetEntitySignature(Entity entity) const;

	// void TransferEntityComponents(Entity entity, Archetype& from, Archetype& to)
	// {
	// 	for (ComponentType ct = 0; ct < MAX_COMPONENTS; ++ct)
	// 	{
	// 		if (from.GetSignature().test(ct))
	// 		{
	// 			auto fromVector = from.GetComponentVector(ct);
	// 			auto toVector = to.GetOrCreateComponentVector(ct);

	// 			if (fromVector && toVector)
	// 			{
	// 				fromVector->TransferComponent(entity, *toVector);
	// 			}
	// 			else
	// 				throw std::runtime_error("Could not get or create component vectors!");

	// 		}
	// 	}
	// }

	// Entity CreateEntity();
	// void DestroyEntity(Entity entity);

	// --- Component Management ---
	// template<typename TComponent>
	// void AddComponent(Entity entity, const TComponent& component)
	// {
	// 	// perform inheritance checks and get or register type
	// 	auto componentType = ComponentRegistry::GetOrRegisterComponentType<TComponent>();
	// 	auto existingSignature = GetEntitySignature(entity);

	// 	auto signatureWithComponent = GenerateNewSignature(existingSignature, componentType);

	// 	// archetype with component that is being added
	// 	auto archetype = GetOrCreateArchetype(signatureWithComponent);

	// 	// if the entity had data elsewhere and we are adding a new component, transfer them
	// 	if (existingSignature && archetype != GetSignatureArchetype(existingSignature.value()))
	// 	{
	// 		TransferEntityComponents(entity, *GetSignatureArchetype(existingSignature.value()), *archetype);
	// 	}

	// 	// add the component to the archetype and update entity signature.
	// 	archetype->AddComponent<TComponent>(entity, component, componentType);
	// 	_entityToSignature[entity] = signatureWithComponent;
	// }

	// template<typename TComponent>
	// TComponent* GetComponent(Entity entity)
	// {
	// 	auto archetype = GetEntityArchetype(entity);
	// 	if (!archetype)
	// 		return nullptr;

	// 	auto componentTypeOpt = ComponentRegistry::GetComponentType<TComponent>();

	// 	return archetype->GetComponent<TComponent>(entity, componentTypeOpt.value());
	// }

	// std::vector<std::pair<ComponentType, Component*>> GetAllComponents(Entity entity) const
	// {
	// 	std::vector<std::pair<ComponentType, Component*>> components;
	// 	auto archetype = GetEntityArchetype(entity);
	// 	if (!archetype)
	// 		return components;

	// 	return archetype->GetAllComponents(entity);
	// }

	// template<typename TComponent>
	// void RemoveComponent(Entity entity) const
	// {

	// }

	// --- Systems ---

	// this is slow, but I do not want to make another registry right now TODO make system registry
	// template<typename TSystem>
	// TSystem* GetSystem() const
	// {
	// 	for (const auto& system : _systems)
	// 	{
	// 		// atempt to cast into specific type
	// 		TSystem* castSystem = dynamic_cast<TSystem*>(system.get());

	// 		if (castSystem) // cast was successful
	// 			return castSystem;
	// 	}

	// 	// system of requested type was not found
	// 	return nullptr;
	// }

	// template<typename TSystem, typename... Args>
	// TSystem& AddSystem(Args&&... args)
	// {
	// 	TSystem* system = new TSystem(std::forward<Args>(args)...);
	// 	_systems.emplace_back(system);
	// 	return *system;
	// }
	 
	// --- Querying ---

	// template is a list of component types <CT1, CT2, ... , CT(N)> (c++11 variadic templates)
	// template<typename... TComponents>
	// std::vector<std::tuple<ComponentVector<TComponents>*...>> QueryComponentVectors()
	// {
	// 	std::vector<std::tuple<ComponentVector<TComponents>*...>> results;

	// 	// pass the component list to component registry to try and generate a signature (c++17 fold expression)
	// 	auto querySignatureOpt = ComponentRegistry::GetComponentArraySignature<TComponents...>();

	// 	// early exit if signature has not been generated
	// 	if (!querySignatureOpt)
	// 		return results;

	// 	auto querySignature = querySignatureOpt.value();

	// 	// iterate through all archetypes and find the ones that include all components in signature
	// 	for (auto& [signature, archetype] : _signatureToArchetype)
	// 	{
	// 		// compare matching bits to the query
	// 		if ((signature & querySignature) == querySignature)
	// 		{
	// 			auto componentVectorsTuple = std::make_tuple(archetype->GetComponentVectorCast<TComponents>()...);
	// 			results.push_back(componentVectorsTuple);
	// 		}
	// 	}
	// 	return results;
	// }

	// template is a list of component types <CT1, CT2, ... , CT(N)> (c++11 variadic templates)
	// raw returns std::vectors instead of Component vectors (no entity-component relations)
	// template<typename... TComponents>
	// std::vector<std::tuple<std::vector<TComponents>*...>> QueryComponentVectorsRaw()
	// {
	// 	std::vector<std::tuple<std::vector<TComponents>*...>> results;

	// 	// pass the component list to component registry to try and generate a signature (c++17 fold expression)
	// 	auto querySignatureOpt = ComponentRegistry::GetComponentArraySignature<TComponents...>();

	// 	// early exit if signature has not been generated
	// 	if (!querySignatureOpt)
	// 		return results;

	// 	auto querySignature = querySignatureOpt.value();

	// 	// iterate through all archetypes and find the ones that include all components in signature
	// 	for (auto& [signature, archetype] : _signatureToArchetype)
	// 	{
	// 		// compare matching bits to the query
	// 		if ((signature & querySignature) == querySignature)
	// 		{
	// 			auto componentVectorsTuple = std::make_tuple(archetype->GetComponentVectorCastRaw<TComponents>()...);
	// 			results.push_back(componentVectorsTuple);
	// 		}
	// 	}
	// 	return results;
	// }
};
