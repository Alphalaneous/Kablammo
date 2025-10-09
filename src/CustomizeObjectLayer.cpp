#include <Geode/Geode.hpp>
#include <Geode/modify/CustomizeObjectLayer.hpp>
#include "Utils.hpp"

using namespace geode::prelude;

class $modify(MyCustomizeObjectLayer, CustomizeObjectLayer) {

	struct Fields {
		bool m_isKablammo;
	};

	bool init(GameObject* object, cocos2d::CCArray* objectArray) {
		if (!CustomizeObjectLayer::init(object, objectArray)) return false;
		setTextBtn();
		return true;
	};

	void setTextBtn() {
		auto fields = m_fields.self();
		if (auto textObject = typeinfo_cast<TextGameObject*>(m_targetObject)) {
			auto pair = kablammo_utils::splitIntoPair(textObject->m_text);
			if (pair.first == "kablammo") {
				fields->m_isKablammo = true;
				if (!m_textButton) return;
				m_textButton->setVisible(false);
			}
		}
	}

	void onSelectMode(cocos2d::CCObject* sender) {
		auto fields = m_fields.self();

        if (!fields->m_isKablammo) return CustomizeObjectLayer::onSelectMode(sender);
		setTextBtn();
	}
};