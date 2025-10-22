#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class Shake : public CCActionInterval {
protected:
    CCPoint m_startPos;
    CCPoint m_prevOffset;
    float m_strength;
    float m_interval;
    float m_timeSinceLast;
    Shake();

public:
    static Shake* create(float duration, float strength, float interval = 0.016f);
    bool initWithDuration(float duration, float strength, float interval);

    virtual void startWithTarget(CCNode* target);
    virtual void update(float time);
    virtual void stop();
};
