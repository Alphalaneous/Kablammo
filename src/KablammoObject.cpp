#include "KablammoObject.hpp"
#include "Utils.hpp"
#include "Shake.hpp"

kablammo_utils::OrderedMap<std::string, KablammoObjectData> KablammoObject::s_kablammoObjects;

KablammoObject* KablammoObject::create(const KablammoObjectData& data, GameObject* object) {
    auto ret = new KablammoObject();
    if (ret->init(data, object)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

KablammoObject* KablammoObject::create(const std::string& identifier, GameObject* object) {
    return create(dataFromIdentifier(identifier), object);
}

bool KablammoObject::init(const KablammoObjectData& data, GameObject* object) {
    if (!CCSprite::initWithFile(fmt::format("{}-bomb.png"_spr, data.identifier).c_str())) {
        if (!CCSprite::initWithFile("standard-bomb.png"_spr)) return false;
    }

    m_data = data;
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

    return true;
}


void KablammoObject::playFuse() {
    if (!m_object) return;
	FMODAudioEngine::get()->playEffectAdvanced(m_data.fuseSound, 1.f, 1.f, 1.f, 1.f, false, false, 0, m_data.explosionFuse * 1000, 0, 0, false, 0, false, false, 0, 0, 0.f, 0);	
}

void KablammoObject::playExplosion() {
    if (!m_object) return;
    FMODAudioEngine::get()->playEffectAdvanced(m_data.explosionSound, 1.f, 0.f, 2.f, 0.f, false, false, 0, 0, 0, 0, false, 0, false, true, 0, 0, 0.f, 0);

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

        kablammo_utils::forEachObjectInRadius(editor, [editor, this](GameObject* object, float distance) {
            if (m_data.objectModifier) m_data.objectModifier(editor, this, distance, object);
        }, [](GameObject* obj) -> bool {
            return obj->getUserObject("kablammo-enabled");
        }, m_object, m_data.explosionRadius * 30, m_data.skipRadiusCheck);

        if (m_data.onExplode) m_data.onExplode(editor, this);

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
	return s_kablammoObjects.contains(identifier);
}

void KablammoObject::registerObject(const KablammoObjectData& data) {
    s_kablammoObjects[data.identifier] = data;
}

const KablammoObjectData& KablammoObject::dataFromIdentifier(const std::string& identifier) {
    return s_kablammoObjects[identifier];
}

CCRect KablammoObject::getWorldBoundingBox() {
    if (!m_setWorldBoundingBox) {
        auto bounds = boundingBox();
        bounds.origin = getParent()->convertToWorldSpace(bounds.origin);
        bounds.size *= getParent()->getScale();
        m_worldBoundingBox = bounds;
        m_setWorldBoundingBox = true;
    }
    return m_worldBoundingBox;
}


// this code is AI assisted
void KablammoObject::explodeObject(LevelEditorLayer* editor, GameObject* object, const CCPoint& explosionCenter) {
    auto objectsArray = CCArray::create();
    auto newObject = editor->m_editorUI->spriteFromObjectString(
        object->getSaveString(editor), false, false, 1, objectsArray, nullptr, nullptr
    );
    editor->updateObjectColors(objectsArray);

    const auto size = newObject->getScaledContentSize();
    newObject->setPosition(size / 2);
    newObject->setScaleX(object->getScaleX());
    newObject->setScaleY(object->getScaleY());

    auto renderTexture = CCRenderTexture::create(size.width, size.height);
    renderTexture->beginWithClear(0, 0, 0, 0);
    newObject->visit();
    renderTexture->end();
    auto texture = renderTexture->getSprite()->getTexture();

    const float scaleX = object->getScaleX() / object->m_pixelScaleX;
    const float scaleY = object->getScaleY() / object->m_pixelScaleY;
    const auto scaledSize = object->getScaledContentSize();
    const float area = (scaledSize.width / object->m_pixelScaleX) * (scaledSize.height / object->m_pixelScaleY);
    const int baseFragments = 6;
    const int numFragments = std::max(baseFragments, static_cast<int>(baseFragments * (area / 900.f)));

    const float fragFraction = 0.3f;
    const float maxDist = static_cast<CCPoint>(size).getLength() / 2.f;

    for (int i = 0; i < numFragments; ++i) {
        const float w = size.width * fragFraction;
        const float h = size.height * fragFraction;
        const float x = kablammo_utils::randomInRange(0, 1) * (size.width - w);
        const float y = kablammo_utils::randomInRange(0, 1) * (size.height - h);

        auto frag = CCSprite::createWithTexture(texture, CCRectMake(x, y, w, h));
        frag->setPosition(object->getPosition() + CCPoint(x + w/2 - size.width/2, y + h/2 - size.height/2));
        frag->setScaleX(scaleX);
        frag->setScaleY(scaleY);
        frag->setRotation(object->getRotation());
        editor->m_objectLayer->addChild(frag);

        CCPoint delta = frag->getPosition() - explosionCenter;
        CCPoint dir = delta.normalize();
        float dist = delta.getLength();

        float speed = 200.f * (1.f - std::exp(-dist / 40.f));
        CCPoint offset = dir * speed;

        float t = std::pow(dist / maxDist, 0.75f);
        float rotSpeed = (1.f - t) * (500.f + kablammo_utils::randomInRange(0, 1) * 360.f);
        if (kablammo_utils::randomInRange(0, 1) > 0.5f) rotSpeed = -rotSpeed;

        frag->runAction(CCSequence::create(
            CCSpawn::create(
                CCMoveBy::create(0.4f + kablammo_utils::randomInRange(0, 1) * 0.2f, offset),
                CCFadeOut::create(0.5f),
                CCRotateBy::create(0.5f, rotSpeed),
                nullptr
            ),
            CCCallFuncN::create(frag, callfuncN_selector(CCNode::removeFromParent)),
            nullptr
        ));
    }

    queueInMainThread([object = Ref(object), editor = Ref(editor)] {
        editor->removeObject(object, false);
    });
}