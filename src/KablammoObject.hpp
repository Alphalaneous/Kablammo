#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class KablammoObject;

enum class SearchShape {
    Circle,
    Square,
    Diamond,
    Triangle,
    Star
};

struct KablammoObjectData {
	using ObjectsModifier = std::function<void(LevelEditorLayer*, KablammoObject*, float distance, GameObject*)>;
	using OnAction = std::function<void(LevelEditorLayer*, KablammoObject*)>;
    SearchShape searchShape = SearchShape::Circle;
	float explosionRadius = 0.f;
    bool skipRadiusCheck = false;
	float explosionFuse = 0.f;
	std::string explosionSound = "standard-explosion.ogg"_spr;
	std::string fuseSound = "standard-fuse.ogg"_spr;
    std::string bombSprite = "standard-bomb.png"_spr;
    std::string identifier = "unknown";
    std::string name = "Unknown";
    std::string description = "No Description";
    std::string modifierText = "";
	ObjectsModifier objectModifier = nullptr;
    OnAction onExplode = nullptr;
	OnAction onHold = nullptr;
	OnAction onRelease = nullptr;
};

class KablammoObject : public CCSprite {
public:

    static std::vector<KablammoObjectData> s_kablammoObjects;

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
    GameObject* m_object = nullptr;
    CCRect m_worldBoundingBox;
    bool m_setWorldBoundingBox;
    bool m_blewUp;


    // to the judges, I am making it like this cuz I might make it an api at some point
    static geode::Result<> registerObject(const KablammoObjectData& data);
    static bool identifierExists(const std::string& identifier);
    static geode::Result<const KablammoObjectData&> dataFromIdentifier(const std::string& identifier);
    static void safeDeleteObject(LevelEditorLayer* editor, GameObject* object);
    
    static void explodeObject(LevelEditorLayer* editor, GameObject* object, const CCPoint& explosionCenter);
    static void forEachObjectInRadius(GJBaseGameLayer* gjbgl, std::function<void(GameObject*, float distance)> const& callback, std::function<bool(GameObject*)> const& skipped, GameObject* center, float radius, bool skipRadiusCheck = false, SearchShape searchShape = SearchShape::Circle);
};