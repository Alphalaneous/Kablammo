#pragma once

#include <Geode/Geode.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <cmath>
#include <random>

using namespace geode::prelude;

namespace kablammo_utils {

	// AI generated.
	template <typename K, typename V>
	class OrderedMap {
	public:
		using Pair = std::pair<const K&, V&>;
		using ConstPair = std::pair<const K&, const V&>;

		void insert(const K& key, const V& value) {
			if (!contains(key)) {
				m_order.push_back(key);
				m_indexMap[key] = m_order.size() - 1;
			}
			m_data[key] = value;
		}

		void erase(const K& key) {
			auto it = m_data.find(key);
			if (it == m_data.end()) return;

			size_t index = m_indexMap[key];
			m_order.erase(m_order.begin() + index);
			m_data.erase(it);
			m_indexMap.erase(key);

			for (size_t i = index; i < m_order.size(); ++i)
				m_indexMap[m_order[i]] = i;
		}

		bool contains(const K& key) const {
			return m_data.find(key) != m_data.end();
		}

		size_t size() const { return m_order.size(); }
		bool empty() const { return m_order.empty(); }

		geode::Result<const K&> keyAt(size_t index) const {
			if (index >= m_order.size())
				return geode::Err("Index out of range");
			return geode::Ok(std::cref(m_order[index]));
		}

		geode::Result<V&> valueAt(size_t index) {
			if (index >= m_order.size())
				return geode::Err("Index out of range");
			return geode::Ok(std::ref(m_data[m_order[index]]));
		}

		geode::Result<const V&> valueAt(size_t index) const {
			if (index >= m_order.size())
				return geode::Err("Index out of range");
			return geode::Ok(std::cref(m_data.at(m_order[index])));
		}

		geode::Result<Pair> atIndex(size_t index) {
			if (index >= m_order.size())
				return geode::Err("Index out of range");
			const K& key = m_order[index];
			return geode::Ok(Pair{key, m_data[key]});
		}

		geode::Result<ConstPair> atIndex(size_t index) const {
			if (index >= m_order.size())
				return geode::Err("Index out of range");
			const K& key = m_order[index];
			return geode::Ok(ConstPair{key, m_data.at(key)});
		}

		V& operator[](const K& key) {
			if (!contains(key)) {
				m_order.push_back(key);
				m_indexMap[key] = m_order.size() - 1;
			}
			return m_data[key];
		}

		const V& operator[](const K& key) const { return m_data.at(key); }

		auto begin() { return m_order.begin(); }
		auto end() { return m_order.end(); }
		auto begin() const { return m_order.begin(); }
		auto end() const { return m_order.end(); }

	private:
		std::unordered_map<K, V> m_data;
		std::unordered_map<K, size_t> m_indexMap;
		std::vector<K> m_order;
	};

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

	// yoinked and modified from eclipse with permission, just grabs objects from visible section, my changes add distance checks
    static inline void forEachObjectInRadius(GJBaseGameLayer* gjbgl, std::function<void(GameObject*, float distance)> const& callback, std::function<bool(GameObject*)> const& skipped, GameObject* center, float radius, bool skipRadiusCheck) {
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
					float dist = std::sqrt(distSq);

					if (distSq <= radiusSq || skipRadiusCheck) {
                        if (skipped && skipped(obj)) continue;
						if (callback) callback(obj, dist);
					}
				}
			}
		}
	}
};