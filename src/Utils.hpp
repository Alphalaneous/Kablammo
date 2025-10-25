#pragma once

#include <Geode/Geode.hpp>
#include <random>

using namespace geode::prelude;

namespace kablammo_utils {

	static inline std::mt19937 s_rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	static inline std::uniform_real_distribution<float> s_dist01(-1.f, 1.f);

	static std::mt19937& getRNG() {
		static std::mt19937 rng(std::random_device{}());
		return rng;
	}

	// this should be a geode util imo
	template <typename T>
	static inline T randomInRange(T min, T max) {
		if constexpr (std::is_integral_v<T>) {
			std::uniform_int_distribution<T> dist(min, max);
			return dist(getRNG());
		} else if constexpr (std::is_floating_point_v<T>) {
			std::uniform_real_distribution<T> dist(min, max);
			return dist(getRNG());
		} else {
			static_assert(std::is_arithmetic_v<T>, "randomInRange requires numeric type");
		}
	}
	
    static inline std::pair<std::string, std::string> splitIntoPair(const std::string& str) {
        auto split = utils::string::split(str, ":");
        auto& key = split[0];
        if (split.size() < 2) return {key, ""};
        auto value = str.substr(key.size() + 1);

        return {key, value};
    }

	static inline std::map<int,std::string> parseKV(const std::string& str) {
		std::map<int,std::string> result;
		std::stringstream ss(str);
		std::string token;

		bool expectKey = true;
		int key = 0;

		while (std::getline(ss, token, ',')) {
			if (expectKey) {
				key = numFromString<int>(token).unwrapOr(-1);
				expectKey = false;
			} else {
				result[key] = token;
				expectKey = true;
			}
		}

		return result;
	}

	static inline std::string buildKV(const std::map<int,std::string>& map) {
		std::stringstream ss;
		bool first = true;

		for (const auto& [key, value] : map) {
			if (!first) ss << ",";
			ss << key << "," << value;
			first = false;
		}

		return ss.str();
	}

	static inline void fixObjectPosition(GameObject* object, LevelEditorLayer* editor) {
		if (!EditorUI::get()) return;
		if (object->getGroupDisabled()) return;
		object->updateStartValues();
		editor->reorderObjectSection(object);
		editor->m_drawGridLayer->m_sortEffects = true;
		if (object->m_dontIgnoreDuration) {
			static_cast<EffectGameObject*>(object)->m_endPosition = CCPoint{0, 0};
		}

		if (object->isSpeedObject() || object->canReverse()) {
			editor->m_drawGridLayer->m_updateSpeedObjects = true;
		}
	}

	static inline bool pointInPolygon(const std::vector<std::pair<float, float>>& poly, float x, float y) {
		bool inside = false;
		size_t n = poly.size();
		for (size_t i = 0, j = n - 1; i < n; j = i++) {
			float xi = poly[i].first, yi = poly[i].second;
			float xj = poly[j].first, yj = poly[j].second;
			bool intersects = ((yi > y) != (yj > y)) && (x < (xj - xi) * (y - yi) / (yj - yi + 1e-9f) + xi);
			if (intersects) inside = !inside;
		}
		return inside;
	}
};