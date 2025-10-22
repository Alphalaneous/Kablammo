#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include "BoomLayer.hpp"

using namespace geode::prelude;


class $modify(MyEditorUI, EditorUI) {

	struct Fields {
		BoomLayer* m_boomLayer;
	};

	static MyEditorUI* get();

	bool init(LevelEditorLayer* lel);
	void reloadButtonBar(EditButtonBar* buttonBar);
	void createMoveMenu();
	void onKablammo(CCObject* sender);
};