#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include "BoomLayer.hpp"

using namespace geode::prelude;


class $modify(MyEditorUI, EditorUI) {

	struct Fields {
		int m_currentChannel = 10000;
		BoomLayer* m_boomLayer;
	};

	static MyEditorUI* get();

	bool init(LevelEditorLayer* lel);
	int getCurrentChannel();
	void incrementChannel();
	void reloadButtonBar(EditButtonBar* buttonBar);
	void createMoveMenu();
	void onKablammo(CCObject* sender);
};