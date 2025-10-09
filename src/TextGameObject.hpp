#pragma once
#include <Geode/Geode.hpp>
#include <Geode/modify/TextGameObject.hpp>
#include "KablammoObject.hpp"

using namespace geode::prelude;

class $modify(MyTextGameObject, TextGameObject) {
	struct Fields {
		KablammoObject* m_kablammoObject;
		bool m_blewUp = false;
	};

	void setupKablammo();
    gd::string getSaveString(GJBaseGameLayer* p0);
    void customObjectSetup(gd::vector<gd::string>& p0, gd::vector<void*>& p1);
	void updateTextObject(gd::string p0, bool p1);
};