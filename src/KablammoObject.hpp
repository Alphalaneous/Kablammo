#pragma once

#include "Utils.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class KablammoObject;

struct KablammoObjectData {
	using ObjectsModifier = std::function<void(LevelEditorLayer*, KablammoObject*, float distance, GameObject*)>;
	using OnAction = std::function<void(LevelEditorLayer*, KablammoObject*)>;
	float explosionRadius = 0.f;
    bool skipRadiusCheck = false;
	float explosionFuse = 0.f;
	std::string explosionSound = "standard-explosion.ogg"_spr;
	std::string fuseSound = "standard-fuse.ogg"_spr;
    std::string identifier = "unknown";
    std::string name = "Unknown";
    std::string description = "No Description";
	ObjectsModifier objectModifier = nullptr;
    OnAction onExplode = nullptr;
	OnAction onHold = nullptr;
	OnAction onRelease = nullptr;
};

class KablammoObject : public CCSprite {
public:

    static kablammo_utils::OrderedMap<std::string, KablammoObjectData> s_kablammoObjects;

    static KablammoObject* create(const KablammoObjectData& data, GameObject* object = nullptr);
    static KablammoObject* create(const std::string& identifier, GameObject* object = nullptr);

    bool init(const KablammoObjectData& data, GameObject* object);
    void loadFuse();
    void playFuse();
    void stopFuse();
    void playExplosion();

    void prepareExplosion();
    void explode(float dt);
    void ensureRemoval(float dt);

    CCRect getWorldBoundingBox();

    KablammoObjectData m_data;
    Ref<GameObject> m_object = nullptr;
    CCRect m_worldBoundingBox;
    bool m_setWorldBoundingBox;
    bool m_blewUp;

    static void registerObject(const KablammoObjectData& data);
    static bool identifierExists(const std::string& identifier);
    static const KablammoObjectData& dataFromIdentifier(const std::string& identifier);
    
    static void explodeObject(LevelEditorLayer* editor, GameObject* object, const CCPoint& explosionCenter);

};