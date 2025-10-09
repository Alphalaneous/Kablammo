#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class KablammoObject;

struct KablammoObjectData {
	using ObjectsModifier = std::function<void(LevelEditorLayer*, KablammoObject*, GameObject*)>;
	using OnAction = std::function<void(LevelEditorLayer*, KablammoObject*)>;
	float explosionRadius;
	float explosionFuse;
	std::string explosionSound;
	std::string fuseSound;
    std::string identifier;
    std::string name;
    std::string description;
	ObjectsModifier objectModifier;
	OnAction onHold;
	OnAction onRelease;
};

class KablammoObject : public CCSprite {
public:

    static std::map<std::string, KablammoObjectData> s_kablammoObjects;

    static KablammoObject* create(KablammoObjectData data, GameObject* object = nullptr, int channel = -1);
    static KablammoObject* create(const std::string& identifier, GameObject* object = nullptr, int channel = -1);

    bool init(KablammoObjectData data, GameObject* object, int channel);
    void loadFuse();
    void playFuse();
    void stopFuse();
    void playExplosion();

    void prepareExplosion();
    void explode(float dt);
    void ensureRemoval(float dt);

    KablammoObjectData m_data;
    Ref<GameObject> m_object = nullptr;
    int m_channel = -1;
    bool m_blewUp;

    static void registerObject(KablammoObjectData data);
    static bool identifierExists(const std::string& identifier);
    static const KablammoObjectData& dataFromIdentifier(const std::string& identifier);
    

};