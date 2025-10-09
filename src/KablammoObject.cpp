#include "KablammoObject.hpp"
#include "Utils.hpp"
#include "Shake.hpp"

std::map<std::string, KablammoObjectData> KablammoObject::s_kablammoObjects;

KablammoObject* KablammoObject::create(KablammoObjectData data, GameObject* object, int channel) {
    auto ret = new KablammoObject();
    if (ret->init(data, object, channel)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

KablammoObject* KablammoObject::create(const std::string& identifier, GameObject* object, int channel) {
    return create(dataFromIdentifier(identifier), object, channel);
}

bool KablammoObject::init(KablammoObjectData data, GameObject* object, int channel) {
    if (!CCSprite::initWithFile(data.identifier.c_str())) {
        if (!CCSprite::initWithFile("bomb.png"_spr)) return false;
    }

    m_data = data;
    m_channel = channel;
    if (object) {
        m_object = object;
        m_object->setUserObject("kablammo-enabled", CCBool::create(true));
    
        CCParticleSystemQuad* particles = CCParticleSystemQuad::createWithTotalParticles(30, false);
        particles->setDuration(-1);
        particles->setLife(1);
        particles->setLifeVar(0.3f);
        particles->setEmissionRate(30);
        particles->setAngle(-90);
        particles->setAngleVar(10);
        particles->setSpeed(30);
        particles->setSpeedVar(10);
        particles->setGravity({-30, 10});
        particles->setRadialAccel(40);
        particles->setRadialAccelVar(10);
        particles->setStartSize(1);
        particles->setEndSize(8);
        particles->setStartSpinVar(90);
        particles->setEndSpinVar(90);
        particles->setStartColor({1, .41, 0, 1});
        particles->setEndColor({0, 0, 0, 0});
        particles->setStartColorVar({0, 0, 0, 0});
        particles->setEndColorVar({0, 0, 0, 0});
        particles->setFrictionSize(1);
        particles->setFrictionRot(1);
        particles->setBlendAdditive(false);
        particles->setDisplayFrame(CCSpriteFrameCache::get()->spriteFrameByName("particle_02_001.png"));
        particles->setPositionType(tCCPositionType::kCCPositionTypeRelative);
        particles->setPosition({getContentWidth() - 5, getContentHeight() - 2});
        particles->setScale(-1);

        addChild(particles);
    }

    loadFuse();

    return true;
}

void KablammoObject::loadFuse() {
    if (m_channel == -1) return;
	FMODAudioEngine::get()->playEffectAdvanced(m_data.fuseSound, 1, 1, 0, 1, false, false, 0, 0, 0, 0, false, 0, false, false, m_channel, 0, 0, 0);
}

void KablammoObject::playFuse() {
    if (m_channel == -1) return;
    stopFuse();
	FMODAudioEngine::get()->playEffectAdvanced(m_data.fuseSound, 1, 1, 1, 1, false, false, 0, 0, 0, 0, true, 0, false, false, m_channel, 0, 0, 0);	
}

void KablammoObject::stopFuse() {
    if (m_channel == -1) return;
    FMODAudioEngine::get()->channelForChannelID(m_channel)->setPaused(true);
}

void KablammoObject::playExplosion() {
    if (m_channel == -1) return;
    FMODAudioEngine::get()->playEffectAsync(m_data.explosionSound);
}

void KablammoObject::ensureRemoval(float dt) {
    if (!m_object) return;
    if (m_blewUp) {
        if (auto editor = LevelEditorLayer::get()) {
            editor->m_editorUI->deselectAll();
            editor->removeObject(m_object, false);
        }
    }
}

void KablammoObject::explode(float dt) {
    if (!m_object) return;
    if (auto editor = LevelEditorLayer::get()) {
        m_blewUp = true;
        editor->m_editorUI->deselectAll();
        kablammo_utils::forEachObjectInRadius(editor, [editor, this](GameObject* object) {
            m_data.objectModifier(editor, this, object);
        }, [](GameObject* obj) -> bool {
            return obj->getUserObject("kablammo-enabled");
        }, m_object, m_data.explosionRadius * 30);

        stopFuse();
        playExplosion();
        
        editor->m_editorUI->deselectAll();
        editor->m_objectLayer->runAction(Shake::create(0.2f, 2));
        editor->removeObject(m_object, false);
    }
    schedule(schedule_selector(KablammoObject::ensureRemoval));
}

void KablammoObject::prepareExplosion() {
    if (!m_object) return;

    float duration = 0.3f;

    auto scale = CCScaleTo::create(duration, 1.5f);
    auto ease = CCEaseBackIn::create(scale);
    auto delay = CCDelayTime::create(m_data.explosionFuse - duration - 0.1f);

    auto sequence = CCSequence::create(delay, ease, nullptr);

    

    runAction(sequence);
    scheduleOnce(schedule_selector(KablammoObject::explode), m_data.explosionFuse);
}

bool KablammoObject::identifierExists(const std::string& identifier) {
	return s_kablammoObjects.find(identifier) != s_kablammoObjects.end();
}

void KablammoObject::registerObject(KablammoObjectData data) {
    s_kablammoObjects[data.identifier] = data;
}

const KablammoObjectData& KablammoObject::dataFromIdentifier(const std::string& identifier) {
    return s_kablammoObjects[identifier];
}