#include "BoomLayer.hpp"
#include <Geode/utils/base64.hpp>

BoomLayer* BoomLayer::create(float heightOffset) {
    auto ret = new BoomLayer();
    if (ret->init(heightOffset)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool BoomLayer::init(float heightOffset) {
    if (!CCLayerRGBA::init()) return false;
    setTouchEnabled(true);

    m_heightOffset = heightOffset;

    auto background = CCSprite::create("booms.png"_spr);
    setContentSize(background->getScaledContentSize());
    background->setPosition(getContentSize()/2);
    addChild(background);
    setAnchorPoint({0.f, 0.f});
    ignoreAnchorPointForPosition(false);
    auto winSize = CCDirector::get()->getWinSize();
    setPosition({winSize.width + 5, heightOffset + 5});
    setZOrder(1000);

    m_statsArea = TextArea::create("N/A\nRadius: <cg>N/A</c>\nFuse: <cg>N/A</c>", "bigFont.fnt", 0.3f, 75, {0.f, 1.f}, 10, false);
    m_statsArea->setAnchorPoint({0.f, 1.f});
    m_statsArea->setPosition({10, getContentHeight() - 15});

    addChild(m_statsArea);

    float scaleFactor = 0.75f;

    m_descriptionArea = MDTextArea::create("Click an explosive to view information.", {(getContentWidth() - 20) / scaleFactor, 22 / scaleFactor});
    m_descriptionArea->setScale(scaleFactor);
    m_descriptionArea->setAnchorPoint({0.5f, 1.f});
    m_descriptionArea->setPosition({getContentWidth()/2 - 1, getContentHeight() - 53});

    addChild(m_descriptionArea);

    auto layout = RowLayout::create();
    layout->setGap(5);
    layout->setAutoScale(false);

    m_row1 = CCNode::create();
    m_row1->setScale(0.75f);
    m_row1->setContentSize({getContentWidth() / m_row1->getScale(), 30});
    m_row1->setPosition({getContentWidth()/2, 103});
    m_row1->setAnchorPoint({0.5f, 0.f});
    m_row1->setLayout(layout);

    addChild(m_row1);

    m_row2 = CCNode::create();
    m_row2->setScale(0.75f);
    m_row2->setContentSize({getContentWidth() / m_row2->getScale(), 30});
    m_row2->setPosition({getContentWidth()/2, 52});
    m_row2->setAnchorPoint({0.5f, 0.f});
    m_row2->setLayout(layout);

    addChild(m_row2);
    
    auto navMenu = CCMenu::create();
    navMenu->setContentSize({getContentWidth(), 30});
    navMenu->setPosition({getContentWidth()/2, 10});
    navMenu->setAnchorPoint({0.5f, 0.f});
    navMenu->ignoreAnchorPointForPosition(false);

    if (KablammoObject::s_kablammoObjects.size() <= 10) {
        navMenu->setVisible(false);
    }

    addChild(navMenu);

    auto prevSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
    prevSpr->setScale(0.5f);
    auto prevBtn = CCMenuItemSpriteExtra::create(prevSpr, this, menu_selector(BoomLayer::onPrevPage));
    prevBtn->setPosition({getContentWidth()/2 - 30, navMenu->getContentHeight()/2});
    
    navMenu->addChild(prevBtn);

    auto nextSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
    nextSpr->setScale(0.5f);
    nextSpr->setFlipX(true);
    auto nextBtn = CCMenuItemSpriteExtra::create(nextSpr, this, menu_selector(BoomLayer::onNextPage));
    nextBtn->setPosition({getContentWidth()/2 + 30, navMenu->getContentHeight()/2});
    
    navMenu->addChild(nextBtn);

    int totalObjects = static_cast<int>(KablammoObject::s_kablammoObjects.size());
    int totalPages = (totalObjects + 9) / 10;
    m_pageLabel = CCLabelBMFont::create(fmt::format("1 / {}", totalPages).c_str(), "bigFont.fnt");
    m_pageLabel->setScale(0.35f);
    m_pageLabel->setPosition({getContentWidth()/2, 25});

    addChild(m_pageLabel);

    goToPage(0);

    return true;
}

void BoomLayer::onPrevPage(CCObject* sender) {
    if (KablammoObject::s_kablammoObjects.empty()) return;

    int totalObjects = static_cast<int>(KablammoObject::s_kablammoObjects.size());
    int totalPages = (totalObjects + 9) / 10;

    m_page = (m_page - 1 + totalPages) % totalPages;
    goToPage(m_page);
}

void BoomLayer::onNextPage(CCObject* sender) {
    if (KablammoObject::s_kablammoObjects.empty()) return;

    int totalObjects = static_cast<int>(KablammoObject::s_kablammoObjects.size());
    int totalPages = (totalObjects + 9) / 10;

    m_page = (m_page + 1) % totalPages;
    goToPage(m_page);
}

void BoomLayer::goToPage(int page) {
    m_kablammoObjects.clear();
    m_row1->removeAllChildren();
    m_row2->removeAllChildren();
    int totalObjects = static_cast<int>(KablammoObject::s_kablammoObjects.size());
    int totalPages = (totalObjects + 9) / 10;

    m_pageLabel->setString(fmt::format("{} / {}", page+1, totalPages).c_str());

    for (int i = 0; i < 10; i++) {
        int index = i + page * 10;
        if (index < KablammoObject::s_kablammoObjects.size()) {
            const auto& v = KablammoObject::s_kablammoObjects[i + page * 10];

            auto obj = KablammoObject::create(v);

            if (i < 5) {
                m_row1->addChild(obj);
            }
            else if (i < 10) {
                m_row2->addChild(obj);
            }

            m_kablammoObjects.push_back(obj);
        }
    }

    m_row1->updateLayout();
    m_row2->updateLayout();
}

void BoomLayer::registerWithTouchDispatcher() {
    CCDirector::get()
        ->getTouchDispatcher()
        ->addTargetedDelegate(this, getTouchPriority(), true);
}

bool BoomLayer::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent) {
    if (!boundingBox().containsPoint(pTouch->getLocation())) return false;

    bool touchedObject = false;

    for (const auto& obj : m_kablammoObjects) {
        auto childBounds = obj->getWorldBoundingBox();
        if (childBounds.containsPoint(pTouch->getLocation())) {
            obj->setColor({150, 150, 150});
            m_grabbedObject = obj;
            m_draggedObject = KablammoObject::create(obj->m_data);
            m_draggedObject->setVisible(false);
            m_draggedObject->setScale(0.75f);
            auto pos = convertToNodeSpace(pTouch->getLocation());
            m_draggedObject->setPosition(pos);

            addChild(m_draggedObject);
            auto& data = obj->m_data;

            std::string radiusText = data.explosionRadius != 0 ? numToString(data.explosionRadius) : "N/A";

            m_statsArea->setString(fmt::format("{}\nRadius: <cg>{}</c>\nFuse: <cg>{}s</c>", data.name, radiusText, data.explosionFuse));
            m_descriptionArea->setString(data.description.c_str());
            touchedObject = true;
            break;
        }
    }

    if (!touchedObject) {
        m_grabbedObject = nullptr;
        if (m_draggedObject) {
            m_draggedObject->removeFromParent();
            m_draggedObject = nullptr;
        }

        m_statsArea->setString("N/A\nRadius: <cg>N/A</c>\nFuse: <cg>N/A</c>");
        m_descriptionArea->setString("Click an explosive to view information.");
    }

    for (const auto& obj : m_kablammoObjects) {
        if (obj == m_grabbedObject) continue;
        obj->setColor({255, 255, 255});
    }

    return true;
}

void BoomLayer::ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent) {
    if (m_grabbedObject) {
        m_grabbedObject->setVisible(false);
    }
    if (m_draggedObject) {
        auto pos = convertToNodeSpace(pTouch->getLocation());
        m_draggedObject->setPosition(pos);
        m_draggedObject->setVisible(true);
    }
}

void BoomLayer::ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent) {
    if (m_grabbedObject) {
        m_grabbedObject->setVisible(true);
    }
    if (m_draggedObject) {
        m_draggedObject->removeFromParent();
        m_draggedObject = nullptr;
    }

    if (boundingBox().containsPoint(pTouch->getLocation())) return;

    if (auto editorUI = EditorUI::get()) {

        CCPoint localPosAR = editorUI->m_editorLayer->m_objectLayer->convertToNodeSpace(pTouch->getLocation());
        float posX = localPosAR.x;
        float posY = localPosAR.y - editorUI->m_toolbarHeight / editorUI->m_positionSlider->getScale();
        
        if (m_grabbedObject) {
            std::string obj = fmt::format("1,914,2,{},3,{},21,1011,31,{},24,11", posX, posY, utils::base64::encode(fmt::format("kablammo:{}", m_grabbedObject->m_data.identifier)));
            editorUI->pasteObjects(obj, true, true);
            editorUI->updateButtons();
            editorUI->updateObjectInfoLabel();
        }
    }
}

void BoomLayer::ccTouchCancelled(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent) {
    for (const auto& obj : m_kablammoObjects) {
        obj->setColor({255, 255, 255});
    }
    if (m_grabbedObject) {
        m_grabbedObject->setVisible(true);
    }
    if (m_draggedObject) {
        m_draggedObject->removeFromParent();
        m_draggedObject = nullptr;
    }
}

void BoomLayer::show() {

    auto winSize = CCDirector::get()->getWinSize();

    if (m_grabbedObject) {
        m_grabbedObject->setVisible(true);
        m_grabbedObject = nullptr;
    }

    for (const auto& obj : m_kablammoObjects) {
        obj->setColor({255, 255, 255});
    }
    m_statsArea->setString("N/A\nRadius: <cg>N/A</c>\nFuse: <cg>N/A</c>");
    m_descriptionArea->setString("Click an explosive to view information.");

    auto moveTo = CCMoveTo::create(0.3f, {winSize.width - getContentWidth() - 10, m_heightOffset + 5});
    auto ease = CCEaseBackOut::create(moveTo);

    stopAllActions();
    runAction(ease);
    m_showing = true;

}

void BoomLayer::hide() {

    auto winSize = CCDirector::get()->getWinSize();

    auto moveTo = CCMoveTo::create(0.3f, {winSize.width + 5, m_heightOffset + 5});
    auto ease = CCEaseOut::create(moveTo, 0.2f);
    
    stopAllActions();
    runAction(ease);
    m_showing = false;
}