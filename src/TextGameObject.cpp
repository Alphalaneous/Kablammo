#include <Geode/Geode.hpp>
#include <Geode/utils/base64.hpp>
#include "TextGameObject.hpp"
#include "Utils.hpp"

using namespace geode::prelude;

void MyTextGameObject::setupKablammo() {
    auto fields = m_fields.self();
    if (fields->m_kablammoObject) return;

    auto pair = kablammo_utils::splitIntoPair(m_text);

    if (pair.first == "kablammo") {
        if (KablammoObject::identifierExists(pair.second)) {
            m_hasSpecialChild = true;

            fields->m_kablammoObject = KablammoObject::create(pair.second, this);

            for (auto child : getChildrenExt()) {
                child->setVisible(false);
            }

            setContentSize(fields->m_kablammoObject->getContentSize());
            fields->m_kablammoObject->setPosition(getContentSize()/2);
            fields->m_kablammoObject->setZOrder(1);
            m_width = getContentWidth();
            m_height = getContentHeight();
            updateOrientedBox();

            addChild(fields->m_kablammoObject);

            queueInMainThread([] {
                if (auto editor = LevelEditorLayer::get()) {
                    editor->m_editorUI->deselectAll();
                }
            });

            fields->m_kablammoObject->playFuse();
            fields->m_kablammoObject->prepareExplosion();
        }
    }
}

gd::string MyTextGameObject::getSaveString(GJBaseGameLayer* p0) {
    if (m_fields->m_kablammoObject) {
        return "";
    }
    return TextGameObject::getSaveString(p0);
}

void MyTextGameObject::customObjectSetup(gd::vector<gd::string>& p0, gd::vector<void*>& p1) {
    TextGameObject::customObjectSetup(p0, p1);
    if (!LevelEditorLayer::get()) return;
    setupKablammo();
}

void MyTextGameObject::updateTextObject(gd::string p0, bool p1) {
    TextGameObject::updateTextObject(p0, p1);
    if (!LevelEditorLayer::get()) return;
    setupKablammo();
}