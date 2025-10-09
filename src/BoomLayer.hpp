#pragma once

#include <Geode/Geode.hpp>
#include "KablammoObject.hpp"

using namespace geode::prelude;

class BoomLayer : public CCLayerRGBA {
public:
    static BoomLayer* create(float heightOffset);

    void registerWithTouchDispatcher() override;
    bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent) override;
    void ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent) override;
    void ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent) override;
    void ccTouchCancelled(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent) override;

    bool init(float heightOffset);
    void show();
    void hide();

    std::vector<Ref<KablammoObject>> m_kablammoObjects;
    CCNode* m_kablammoContainer;
    KablammoObject* m_grabbedObject;
    KablammoObject* m_draggedObject;
    bool m_showing = false;
    float m_heightOffset = 0;

    TextArea* m_statsArea;
    MDTextArea* m_descriptionArea;
};