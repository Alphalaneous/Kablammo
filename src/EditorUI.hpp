#pragma once

#include <Geode/Geode.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/modify/EditorUI.hpp>
#include "BoomLayer.hpp"

using namespace geode::prelude;


class $modify(MyEditorUI, EditorUI) {

	struct Fields {
		BoomLayer* m_boomLayer;
		std::set<GameObject*> m_objectsToRemove;
		std::set<GameObject*> m_objectsToFix;
		int m_fragmentsVisible = 0;
	};

	static MyEditorUI* get();

	bool init(LevelEditorLayer* lel);
	void reloadButtonBar(EditButtonBar* buttonBar);
	void createMoveMenu();
	void onKablammo(CCObject* sender);
	void objectUpdate(float dt);
	void addObjectToDelete(GameObject* object);
	bool hasObjectToDelete(GameObject* object);
	void removeObjectToDelete(GameObject* object);
	void addObjectToFix(GameObject* object);
	void removeObjectToFix(GameObject* object);
	void onPlaytest(CCObject* sender);
	void incrementFragmentsVisible();
	void decrementFragmentsVisible();
	bool canAddMoreFragments();
};