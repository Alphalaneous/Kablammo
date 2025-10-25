#include "EditorUI.hpp"
#include <Geode/utils/base64.hpp>

MyEditorUI* MyEditorUI::get() {
    return static_cast<MyEditorUI*>(EditorUI::get());
}

void MyEditorUI::reloadButtonBar(EditButtonBar* buttonBar) {
    auto rows = GameManager::get()->getIntGameVariable("0049");
    auto cols = GameManager::get()->getIntGameVariable("0050");
    buttonBar->reloadItems(rows, cols);
}

void MyEditorUI::createMoveMenu() {
    EditorUI::createMoveMenu();

    auto btn = getSpriteButton("standard-bomb.png"_spr, menu_selector(MyEditorUI::onKablammo), nullptr, 0.9f);
    btn->setID("kablammo-menu-button"_spr);
    m_editButtonBar->m_buttonArray->addObject(btn);
    reloadButtonBar(m_editButtonBar);
}

void MyEditorUI::incrementFragmentsVisible() {
    m_fields->m_fragmentsVisible += 1;
}

void MyEditorUI::decrementFragmentsVisible() {
    m_fields->m_fragmentsVisible -= 1;
}

bool MyEditorUI::canAddMoreFragments() {
    return m_fields->m_fragmentsVisible <= 64;
}

bool MyEditorUI::init(LevelEditorLayer* lel) {
    if (!EditorUI::init(lel)) return false;

    m_fields->m_boomLayer = BoomLayer::create(m_toolbarHeight);
    addChild(m_fields->m_boomLayer);

    schedule(schedule_selector(MyEditorUI::removeUpdate));
    
    return true;
}

void MyEditorUI::addObjectToDelete(GameObject* object) {
    m_fields->m_objectsToRemove.insert(object);
}

void MyEditorUI::removeUpdate(float dt) {
    for (auto object : m_fields->m_objectsToRemove) {
        if (!object->getGroupDisabled()) {
            object->stopAllActions();
            m_editorLayer->removeObject(object, false);
        }
    }
    m_fields->m_objectsToRemove.clear();
};

void MyEditorUI::onKablammo(CCObject* sender) {
    auto fields = m_fields.self();

    if (fields->m_boomLayer->m_showing) {
        m_fields->m_boomLayer->hide();
    }
    else {
        m_fields->m_boomLayer->show();
    }
}