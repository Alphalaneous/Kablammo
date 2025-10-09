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

    m_kablammoContainer = CCNode::create();
    m_kablammoContainer->setContentSize({getContentWidth(), 130});
    m_kablammoContainer->setAnchorPoint({0.5f, 0.f});
    m_kablammoContainer->setPosition({getContentWidth()/2 - 1, -2});

    auto kablammoContainerLayout = ColumnLayout::create();
    kablammoContainerLayout->setGap(29);
    kablammoContainerLayout->setAxisReverse(true);
    kablammoContainerLayout->setAxisAlignment(AxisAlignment::End);
    kablammoContainerLayout->setAutoScale(false);

    m_kablammoContainer->setLayout(kablammoContainerLayout);

    auto layout = RowLayout::create();
    layout->setGap(5);
    layout->setAutoScale(false);
    
    addChild(m_kablammoContainer);

    auto currentNode = CCNode::create();
    currentNode->setScale(0.75f);
    currentNode->setContentSize({getContentWidth() / currentNode->getScale(), 30});
    currentNode->setAnchorPoint({0.5f, 0.f});
    currentNode->setLayout(layout);

    m_kablammoContainer->addChild(currentNode);

    int count = 0;

    for (const auto& [k, v] : KablammoObject::s_kablammoObjects) {
        if (count == 5) {
            currentNode->updateLayout();
            currentNode = CCNode::create();
            currentNode->setScale(0.75f);
            currentNode->setContentSize({getContentWidth() / currentNode->getScale(), 30});
            currentNode->setAnchorPoint({0.5f, 0.f});
            currentNode->setLayout(layout);
            m_kablammoContainer->addChild(currentNode);
            count = 0;
        }
        auto obj = KablammoObject::create(v);
        currentNode->addChild(obj);
        m_kablammoObjects.push_back(obj);
        count++;
    }

    currentNode->updateLayout();
    m_kablammoContainer->updateLayout();

    return true;
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
        auto childBounds = obj->boundingBox();
        childBounds.origin = obj->getParent()->convertToWorldSpace(childBounds.origin);
        childBounds.size *= obj->getParent()->getScale();
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
            m_statsArea->setString(fmt::format("{}\nRadius: <cg>{}</c>\nFuse: <cg>{}s</c>", data.name, data.explosionRadius, data.explosionFuse));
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
            std::string obj = fmt::format("1,914,2,{},3,{},31,{},24,11", posX, posY, utils::base64::encode(fmt::format("kablammo:{}", m_grabbedObject->m_data.identifier)));
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