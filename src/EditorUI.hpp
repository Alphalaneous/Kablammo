#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include "BoomLayer.hpp"

using namespace geode::prelude;


class $modify(MyEditorUI, EditorUI) {

	struct Fields {
		BoomLayer* m_boomLayer;
		std::set<GameObject*> m_objectsToRemove;
		int m_fragmentsVisible = 0;
	};

	static MyEditorUI* get();

	bool init(LevelEditorLayer* lel);
	void reloadButtonBar(EditButtonBar* buttonBar);
	void createMoveMenu();
	void onKablammo(CCObject* sender);
	void removeUpdate(float dt);
	void addObjectToDelete(GameObject* object);
	void incrementFragmentsVisible();
	void decrementFragmentsVisible();
	bool canAddMoreFragments();
};