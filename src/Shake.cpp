#include "Shake.hpp"
#include "Utils.hpp"

Shake::Shake() : m_strength(0.f), m_interval(0.f), m_timeSinceLast(0.f), m_prevOffset(CCPoint{0, 0}) {}

Shake* Shake::create(float duration, float strength, float interval) {
    auto ret = new Shake();
    if (ret->initWithDuration(duration, strength, interval)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool Shake::initWithDuration(float duration, float strength, float interval) {
    if (!CCActionInterval::initWithDuration(duration)) return false;

    m_strength = strength;
    m_interval = interval;
    m_timeSinceLast = 0.f;
    m_prevOffset = CCPointZero;
    return true;
}

void Shake::startWithTarget(CCNode* target) {
    CCActionInterval::startWithTarget(target);
    if (target) m_startPos = target->getPosition();
}

void Shake::update(float dt) {
    m_timeSinceLast += dt;

    if (m_timeSinceLast >= m_interval) {
        m_timeSinceLast = 0.f;

        if (m_pTarget) m_pTarget->setPosition(m_startPos);

        float randX = kablammo_utils::randomInRange(0, 1) * m_strength;
        float randY = kablammo_utils::randomInRange(0, 1) * m_strength;
        m_prevOffset = CCPoint{randX, randY};

        if (m_pTarget) m_pTarget->setPosition(m_startPos + m_prevOffset);
    }
}

void Shake::stop() {
    if (m_pTarget) m_pTarget->setPosition(m_startPos);
    CCActionInterval::stop();
}