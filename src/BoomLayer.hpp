#pragma once

#include <Geode/Geode.hpp>
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
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
    void onPrevPage(CCObject* sender);
    void onNextPage(CCObject* sender);

    bool init(float heightOffset);
    void show();
    void hide();
    void goToPage(int page);

    std::vector<Ref<KablammoObject>> m_kablammoObjects;
    KablammoObject* m_grabbedObject;
    KablammoObject* m_draggedObject;
    bool m_showing = false;
    float m_heightOffset = 0;
    int m_page = 0;

    TextArea* m_statsArea;
    MDTextArea* m_descriptionArea;
    CCNode* m_row1;
    CCNode* m_row2;
    CCLabelBMFont* m_pageLabel;
};