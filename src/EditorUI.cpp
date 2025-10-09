#include "EditorUI.hpp"
#include <Geode/utils/base64.hpp>

MyEditorUI* MyEditorUI::get() {
    return static_cast<MyEditorUI*>(EditorUI::get());
}

// too lazy to do the fmod properly, it works, I do this so I can pause specific sounds
int MyEditorUI::getCurrentChannel() {
    return m_fields->m_currentChannel;
}

void MyEditorUI::incrementChannel() {
    m_fields->m_currentChannel++;
}

void MyEditorUI::reloadButtonBar(EditButtonBar* buttonBar) {
    auto rows = GameManager::get()->getIntGameVariable("0049");
    auto cols = GameManager::get()->getIntGameVariable("0050");
    buttonBar->reloadItems(rows, cols);
}

void MyEditorUI::createMoveMenu() {
    EditorUI::createMoveMenu();

    auto btn = getSpriteButton("edit_eDamageCircle_001.png", menu_selector(MyEditorUI::onKablammo), nullptr, 0.9f);
    btn->setID("kablammo-test"_spr);
    m_editButtonBar->m_buttonArray->addObject(btn);
    reloadButtonBar(m_editButtonBar);
}

bool MyEditorUI::init(LevelEditorLayer* lel) {
    if (!EditorUI::init(lel)) return false;

    m_fields->m_boomLayer = BoomLayer::create(m_toolbarHeight);
    addChild(m_fields->m_boomLayer);

    return true;
}


void MyEditorUI::onKablammo(CCObject* sender) {
    /*auto winSize = CCDirector::get()->getWinSize();
    CCPoint localPosAR = m_editorLayer->m_objectLayer->convertToNodeSpaceAR(winSize/2);
    float posX = localPosAR.x;
    float posY = localPosAR.y - m_toolbarHeight/2;
    
    std::string obj = fmt::format("1,914,2,{},3,{},31,{}", posX, posY, utils::base64::encode(fmt::format("kablammo:{}", "standard")));
    pasteObjects(obj, true, true);
    updateButtons();
    updateObjectInfoLabel();*/

    auto fields = m_fields.self();

    if (fields->m_boomLayer->m_showing) {
        m_fields->m_boomLayer->hide();
    }
    else {
        m_fields->m_boomLayer->show();
    }
}