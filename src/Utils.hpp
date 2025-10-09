#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace kablammo_utils {

    static inline std::pair<std::string, std::string> splitIntoPair(const std::string& str) {
        auto split = utils::string::split(str, ":");
        auto& key = split[0];
        if (split.size() < 2) return {key, ""};
        auto value = str.substr(key.size() + 1);

        return {key, value};
    }

    static inline void forEachObjectInRadius(GJBaseGameLayer* gjbgl, std::function<void(GameObject*)> const& callback, std::function<bool(GameObject*)> const& skipped, GameObject* center, float radius) {
		if (!gjbgl || !center) return;

		auto centerPos = center->getPosition();
		float radiusSq = radius * radius;

		int sectionCount = gjbgl->m_sections.empty() ? -1 : static_cast<int>(gjbgl->m_sections.size());
		for (int i = gjbgl->m_leftSectionIndex; i <= gjbgl->m_rightSectionIndex && i < sectionCount; ++i) {
			auto leftSection = gjbgl->m_sections[i];
			if (!leftSection) continue;

			int leftSectionSize = static_cast<int>(leftSection->size());
			for (int j = gjbgl->m_bottomSectionIndex; j <= gjbgl->m_topSectionIndex && j < leftSectionSize; ++j) {
				auto section = leftSection->at(j);
				if (!section) continue;

				int sectionSize = gjbgl->m_sectionSizes[i]->at(j);
				for (int k = 0; k < sectionSize; ++k) {
					auto obj = section->at(k);
					if (!obj) continue;

					auto pos = obj->getPosition();
					float dx = pos.x - centerPos.x;
					float dy = pos.y - centerPos.y;
					float distSq = dx * dx + dy * dy;

					if (distSq <= radiusSq) {
                        if (skipped && skipped(obj)) continue;
						if (callback) callback(obj);
					}
				}
			}
		}
	}
};