#include "KablammoObject.hpp"
#include "Utils.hpp"

// CRASHES YOUR FUCKING GAME NERD
static std::vector<int> evilObjects = {
    1964, 1965, 1966, 1967, 1968, 1969, 
    1970, 1971, 1972, 1973, 1974, 1975, 1976, 1977, 1978, 1979, 
    1980, 1981, 1982, 1983, 1984, 1985, 1986, 1987, 1988, 1989,
    1990, 1991, 1992, 1993, 1994, 1995, 1996, 1997, 1998, 1999, 
    2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 
    2010, 2011,
    104,
    4386, 4387, 4388, 4389, 
    4390, 4391, 4392, 4393, 4394, 4395, 4396, 4397, 4398, 4399, 
    4400
};

static void randomizeIDs(LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
    auto str = object->getSaveString(editor);

    const auto& objects = ObjectToolbox::sharedState()->m_allKeys;

    int objectID = 1;

    while (true) {
        auto it = objects.begin();
        int steps = kablammo_utils::randomInRange(0, static_cast<int>(objects.size()) - 1);
        for (int i = 0; i < steps; ++i) ++it;
        objectID = it->first;
        if (std::find(evilObjects.begin(), evilObjects.end(), objectID) == evilObjects.end()) break;
    }

    auto map = kablammo_utils::parseKV(str);
    map[1] = numToString(objectID);

    editor->m_editorUI->pasteObjects(kablammo_utils::buildKV(map), true, true);

    editor->removeObject(object, false);
}

static void blackHole(LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
    auto center = kablammoObj->m_object->getPosition();

    auto pullAction = CCRepeat::create(
        CCSequence::create(
            CallFuncExt::create([editor, center, object] {
                auto pos = object->getPosition();

                float dx = center.x - pos.x;
                float dy = center.y - pos.y;
                float dist = std::sqrt(dx*dx + dy*dy);

                if (dist < 15.0f) {
                    object->stopAllActions();
                    KablammoObject::explodeObject(editor, object, center);
                    return;
                }

                float nx = dx / (dist + 1e-6f);
                float ny = dy / (dist + 1e-6f);

                float strength = 50.0f;
                float radial = strength / (dist + 1.0f);

                float tx = -ny;
                float ty = nx;
                float spiralStrength = 2.f;

                float deltaX = nx * radial + tx * spiralStrength;
                float deltaY = ny * radial + ty * spiralStrength;
                object->setPosition({pos.x + deltaX, pos.y + deltaY});
                kablammo_utils::fixObjectPosition(object, editor);
            }),
            CCDelayTime::create(0.016f),
            nullptr
        ),
        INT_MAX-1
    );
    object->runAction(pullAction);
}

static void gay(LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
    float hue = std::fmod(distance * 3.f, 360.f);

    if (auto main = object->m_baseColor) {
        main->m_colorID = 1011;
        main->m_hsv.h = hue;
        main->m_hsv.absoluteSaturation = true;
        main->m_hsv.s = 255;
        main->m_usesHSV = true;
    }
    if (auto secondary = object->m_detailColor) {
        secondary->m_colorID = 1011;
        secondary->m_hsv.h = hue;
        secondary->m_hsv.absoluteSaturation = true;
        secondary->m_hsv.s = 255;
        secondary->m_usesHSV = true;
    }
}

static void spread(LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
    CCPoint delta = object->getPosition() - kablammoObj->m_object->getPosition();
    CCPoint dir = delta.normalize();
    float dist = delta.getLength();
    const float maxDist = static_cast<CCPoint>(object->getScaledContentSize()).getLength() / 2.f;

    float speed = 200.f * (1.f - std::exp(-dist / 40.f));
    CCPoint offset = dir * speed;

    object->runAction(CCSequence::create(
        CCEaseIn::create(CCMoveBy::create(0.1f + kablammo_utils::randomInRange(0.f, 0.3f) * 0.1f, offset), 0.2f),
        CallFuncExt::create([object, editor] {
            kablammo_utils::fixObjectPosition(object, editor);
        }),
        nullptr
    ));
}

static void scaleSpread(LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
    CCPoint delta = object->getPosition() - kablammoObj->m_object->getPosition();
    float baseDist = delta.getLength();
    CCPoint dir = delta.normalize();

    const float radius = object->getScaledContentSize().width / 2.f;
    float moveDist = 200.f * (1.f - std::exp(-baseDist / 40.f));

    float scaleFactor = (radius + moveDist) / radius;
    scaleFactor = std::clamp(scaleFactor, 1.f, 4.f);

    CCPoint offset = dir * moveDist;
    float duration = 0.1f + kablammo_utils::randomInRange(0.f, 0.3f) * 0.1f;

    object->runAction(CCSequence::create(
        CCSpawn::create(
            CCEaseIn::create(CCMoveBy::create(duration, offset), 0.2f),
            CCEaseIn::create(CCScaleBy::create(duration, scaleFactor), 0.2f),
            nullptr
        ),
        CallFuncExt::create([object, editor, scaleFactor] {
            object->m_scaleX = object->getScaleX();
            object->m_scaleY = object->getScaleY();
            kablammo_utils::fixObjectPosition(object, editor);
        }),
        nullptr
    ));
}

$execute {
	(void) KablammoObject::registerObject({
		.explosionRadius = 5,
        .explosionFuse = 1.5,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "standard-bomb.png"_spr,
        .identifier = "standard",
        .name = "Standard",
        .description = "Destroys Objects!",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            KablammoObject::explodeObject(editor, object, kablammoObj->m_object->getPosition());
        }
	});

    (void) KablammoObject::registerObject({
		.explosionRadius = 5,
        .explosionFuse = 1.5,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "spread-bomb.png"_spr,
        .identifier = "spread",
        .name = "Spread",
        .description = "Spreads objects out!",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            spread(editor, kablammoObj, distance, object);
        }
	});

    (void) KablammoObject::registerObject({
		.explosionRadius = 5,
        .explosionFuse = 4,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "scale-spread-bomb.png"_spr,
        .identifier = "scale-spread",
        .name = "Scale Spread",
        .description = "Scales and Spreads objects out!",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            scaleSpread(editor, kablammoObj, distance, object);
        }
	});

    (void) KablammoObject::registerObject({
		.explosionRadius = 10,
        .explosionFuse = 0.5,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "offset-bomb.png"_spr,
        .identifier = "offset",
        .name = "Offset",
        .description = "Slightly offsets the position of objects!",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            object->setPositionX(object->getPositionX() + kablammo_utils::randomInRange(-2.f, 2.f));
            object->setPositionY(object->getPositionY() + kablammo_utils::randomInRange(-2.f, 2.f));
            kablammo_utils::fixObjectPosition(object, editor);
        }
	});

    (void) KablammoObject::registerObject({
		.explosionRadius = 5,
        .explosionFuse = 1,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "offset-scale-bomb.png"_spr,
        .identifier = "offset-scale",
        .name = "Offset Scale",
        .description = "Offsets the scale of objects!",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            object->setScaleX(object->getScaleX() * kablammo_utils::randomInRange(-2.f, 2.f));
            object->setScaleY(object->getRScaleY() * kablammo_utils::randomInRange(-2.f, 2.f));
            // ts pmo, why are these separate???
            object->m_scaleX = object->getScaleX();
            object->m_scaleY = object->getScaleY();
            kablammo_utils::fixObjectPosition(object, editor);
        }
	});

    (void) KablammoObject::registerObject({
		.explosionRadius = 5,
        .explosionFuse = 1.5,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "gay-bomb.png"_spr,
        .identifier = "gay",
        .name = "Gay Bomb",
        .description = "Makes the objects around it fruity!",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            gay(editor, kablammoObj, distance, object);
        }
	});

    (void) KablammoObject::registerObject({
		.explosionRadius = 5,
        .explosionFuse = 1.5,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "random-id-bomb.png"_spr,
        .identifier = "random-id",
        .name = "Random ID",
        .description = "Replaces the object ID with a random value!",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            randomizeIDs(editor, kablammoObj, distance, object);
        }
	});

    (void) KablammoObject::registerObject({
		.explosionRadius = 2.5,
        .explosionFuse = 2,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "where-bomb.png"_spr,
        .identifier = "where",
        .name = "Where",
        .description = "Objects continue to move randomly forever...",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            object->runAction(
                CCRepeat::create(
                    CCSequence::create(
                        CallFuncExt::create([object, editor] {
                            int xOffset = kablammo_utils::randomInRange(-1, 1) * 30;
                            int yOffset = kablammo_utils::randomInRange(-1, 1) * 30;
                            object->setPosition({object->getPositionX() + xOffset, object->getPositionY() + yOffset});
                            kablammo_utils::fixObjectPosition(object, editor);
                        }),
                        CCDelayTime::create(0.25),
                        nullptr
                    ),
                    INT_MAX
                )
            );
        }
	});

    (void) KablammoObject::registerObject({
		.explosionRadius = 2.5,
        .explosionFuse = 2,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "spin-bomb.png"_spr,
        .identifier = "spin",
        .name = "SPIIINN",
        .description = "Objects continue to spin forever...",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            object->runAction(
                CCRepeat::create(
                    CCRotateBy::create(1, 360),
                    INT_MAX
                )
            );
        }
	});

    (void) KablammoObject::registerObject({
		.explosionRadius = 0,
        .skipRadiusCheck = true,
        .explosionFuse = 2,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "devil-vortex-bomb.png"_spr,
        .identifier = "devil-vortex",
        .name = "Devil Vortex",
        .description = "Gotta balance out the objects!",
        .onExplode = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj) {

            std::vector<std::string> newObjects;

            static std::string_view devilVortex = "1,678,2,-278.75,3,101.75,64,1,155,3,156,4,22,2;1,94,2,-278.75,3,101.75,64,1,155,3,34,1;1,94,2,-188.75,3,101.75,64,1,155,3,128,0.75,129,0.75;1,679,2,-188.75,3,101.75,64,1,155,3,156,4,22,2;1,94,2,-158.75,3,41.75,64,1,155,3;1,678,2,-158.75,3,41.75,64,1,155,3,156,4,22,2;1,94,2,-128.75,3,101.75,64,1,155,3,128,0.5,129,0.5;1,680,2,-128.75,3,101.75,64,1,155,3,156,4,22,2;1,94,2,-8.75,3,41.75,64,1,155,3,128,0.5,129,0.5;1,680,2,-8.75,3,41.75,64,1,155,3,156,4,22,2;1,94,2,-68.75,3,41.75,64,1,155,3,128,0.75,129,0.75;1,679,2,-68.75,3,41.75,64,1,155,3,156,4,22,2;1,678,2,-68.75,3,11.75,64,1,155,3,156,4,22,2;1,94,2,-68.75,3,11.75,64,1,155,3;1,94,2,81.25,3,11.75,64,1,155,3,128,0.5,129,0.5;1,680,2,81.25,3,11.75,64,1,155,3,156,4,22,2;1,94,2,21.25,3,11.75,64,1,155,3,128,0.75,129,0.75;1,679,2,21.25,3,11.75,64,1,155,3,156,4,22,2;1,678,2,21.25,3,11.75,64,1,155,3,156,4,22,2;1,94,2,21.25,3,11.75,64,1,155,3;1,678,2,111.25,3,-18.25,64,1,155,3,156,4,22,2;1,94,2,111.25,3,-18.25,64,1,155,3;1,679,2,111.25,3,11.75,64,1,155,3,156,4,22,2;1,94,2,111.25,3,11.75,64,1,155,3,128,0.75,129,0.75;1,94,2,141.25,3,-78.25,64,1,155,3;1,678,2,141.25,3,-78.25,64,1,155,3,156,4,22,2;1,94,2,171.25,3,-108.25,64,1,155,3;1,678,2,171.25,3,-108.25,64,1,155,3,156,4,22,2;1,679,2,201.25,3,-18.25,64,1,155,3,156,4,22,2;1,94,2,201.25,3,-18.25,64,1,155,3,128,0.75,129,0.75;1,94,2,171.25,3,-108.25,64,1,155,3;1,678,2,171.25,3,-108.25,64,1,155,3,156,4,22,2;1,94,2,171.25,3,11.75,64,1,155,3,128,0.5,129,0.5;1,680,2,171.25,3,11.75,64,1,155,3,156,4,22,2;1,94,2,291.25,3,-78.25,64,1,155,3,128,0.5,129,0.5;1,94,2,261.25,3,-108.25,64,1,155,3,128,0.75,129,0.75;1,94,2,231.25,3,-138.25,64,1,155,3;1,678,2,231.25,3,-138.25,64,1,155,3,156,4,22,2;1,94,2,231.25,3,-78.25,64,1,155,3,128,0.75,129,0.75;1,679,2,231.25,3,-78.25,64,1,155,3,156,4,22,2;1,679,2,261.25,3,-108.25,64,1,155,3,156,4,22,2;1,94,2,261.25,3,-108.25,64,1,155,3,128,0.75,129,0.75;1,680,2,291.25,3,-78.25,64,1,155,3,156,4,22,2;1,680,2,261.25,3,-18.25,64,1,155,3,156,4,22,2;1,94,2,261.25,3,-18.25,64,1,155,3,128,0.5,129,0.5;1,679,2,261.25,3,-108.25,64,1,155,3,156,4,22,2;1,680,2,321.25,3,-78.25,64,1,155,3,156,4,22,2;1,680,2,351.25,3,-108.25,64,1,155,3,156,4,22,2;1,680,2,321.25,3,-108.25,64,1,155,3,156,4,22,2;1,94,2,321.25,3,-108.25,64,1,155,3,128,0.5,129,0.5;1,680,2,321.25,3,-108.25,64,1,155,3,156,4,22,2;1,94,2,321.25,3,-108.25,64,1,155,3,128,0.5,129,0.5;1,94,2,321.25,3,-138.25,64,1,155,3,128,0.75,129,0.75;1,94,2,381.25,3,-138.25,64,1,155,3,128,0.5,129,0.5;";
            static auto devilVortexArray = utils::string::split(std::string(devilVortex), ";");

            for (const auto& v : devilVortexArray) {
                auto map = kablammo_utils::parseKV(std::string(v));
                map[2] = numToString(numFromString<float>(map[2]).unwrapOr(0) + kablammoObj->m_object->getPositionX());
                map[3] = numToString(numFromString<float>(map[3]).unwrapOr(0) + kablammoObj->m_object->getPositionY());
                newObjects.push_back(kablammo_utils::buildKV(map));
            }

            std::stringstream ss;
            bool first = true;

            for (const auto& value : newObjects) {
                if (!first) ss << ";";
                ss << value;
                first = false;
            }

            editor->m_editorUI->pasteObjects(ss.str(), true, true);
        }
	});

    (void) KablammoObject::registerObject({
		.explosionRadius = 10,
        .explosionFuse = 2.5,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "standard-bomb.png"_spr,
        .identifier = "standard-2x",
        .name = "Standard 2x",
        .description = "Destroys Objects!",
        .modifierText = "2x",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            KablammoObject::explodeObject(editor, object, kablammoObj->m_object->getPosition());
        }
	});

    (void) KablammoObject::registerObject({
		.explosionRadius = 10,
        .explosionFuse = 2.5,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "gay-bomb.png"_spr,
        .identifier = "gay-2x",
        .name = "Gay Bomb 2x",
        .description = "Makes the objects around it fruity!",
        .modifierText = "2x",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            gay(editor, kablammoObj, distance, object);
        }
	});

    (void) KablammoObject::registerObject({
		.explosionRadius = 10,
        .explosionFuse = 2.5,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "random-id-bomb.png"_spr,
        .identifier = "random-id-2x",
        .name = "Random ID 2x",
        .description = "Replaces the object ID with a random value!",
        .modifierText = "2x",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            randomizeIDs(editor, kablammoObj, distance, object);
        }
	});

    (void) KablammoObject::registerObject({
        .searchShape = SearchShape::Square,
		.explosionRadius = 5,
        .explosionFuse = 1.5,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "square-bomb.png"_spr,
        .identifier = "square",
        .name = "Square",
        .description = "Destroys Objects but Square!",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            KablammoObject::explodeObject(editor, object, kablammoObj->m_object->getPosition());
        }
	});

    (void) KablammoObject::registerObject({
        .searchShape = SearchShape::Diamond,
		.explosionRadius = 5,
        .explosionFuse = 1.5,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "diamond-bomb.png"_spr,
        .identifier = "diamond",
        .name = "Diamond",
        .description = "Destroys Objects but Diamond!",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            KablammoObject::explodeObject(editor, object, kablammoObj->m_object->getPosition());
        }
	});

    (void) KablammoObject::registerObject({
        .searchShape = SearchShape::Triangle,
		.explosionRadius = 5,
        .explosionFuse = 1.5,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "triangle-bomb.png"_spr,
        .identifier = "triangle",
        .name = "Triangle",
        .description = "Destroys Objects but Triangle!",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            KablammoObject::explodeObject(editor, object, kablammoObj->m_object->getPosition());
        }
	});

    (void) KablammoObject::registerObject({
        .searchShape = SearchShape::Star,
		.explosionRadius = 5,
        .explosionFuse = 1.5,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "star-bomb.png"_spr,
        .identifier = "star",
        .name = "Star",
        .description = "Destroys Objects but Star!",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            KablammoObject::explodeObject(editor, object, kablammoObj->m_object->getPosition());
        }
	});

    (void) KablammoObject::registerObject({
		.explosionRadius = 15,
        .explosionFuse = 5,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "black-hole-bomb.png"_spr,
        .identifier = "black-hole",
        .name = "Black Hole",
        .description = "Everything will be consumed.",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            blackHole(editor, kablammoObj, distance, object);
        }
	});

    (void) KablammoObject::registerObject({
		.explosionRadius = 5,
        .explosionFuse = 1.5,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "spin-down-bomb.png"_spr,
        .identifier = "spin-down",
        .name = "Spin Down",
        .description = "Decreases each object ID by 1",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            auto str = object->getSaveString(editor);

            auto& objects = ObjectToolbox::sharedState()->m_allKeys;

            int objectID = 1;

            while (true) {
                std::map<int, gd::string>::iterator it = objects.find(object->m_objectID);
                if (it != objects.end()) {
                    if (it != objects.begin()) {
                        std::map<int, gd::string>::iterator prevIt = it;
                        --prevIt;
                        objectID = prevIt->first;
                    } else {
                        std::map<int, gd::string>::iterator lastIt = objects.end();
                        --lastIt;
                        objectID = lastIt->first;
                    }
                }

                if (std::find(evilObjects.begin(), evilObjects.end(), objectID) == evilObjects.end())
                    break;
            }
            
            auto map = kablammo_utils::parseKV(str);
            map[1] = numToString(objectID);

            editor->m_editorUI->pasteObjects(kablammo_utils::buildKV(map), true, true);

            editor->removeObject(object, false);
        }
	});

    (void) KablammoObject::registerObject({
		.explosionRadius = 5,
        .explosionFuse = 1.5,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "spin-up-bomb.png"_spr,
        .identifier = "spin-up",
        .name = "Spin Up",
        .description = "Increases each object ID by 1",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            auto str = object->getSaveString(editor);

            auto& objects = ObjectToolbox::sharedState()->m_allKeys;

            int objectID = 1;

            while (true) {
                std::map<int, gd::string>::iterator it = objects.find(object->m_objectID);
                if (it != objects.end()) {
                    if (it != objects.begin()) {
                        std::map<int, gd::string>::iterator nextIt = it;
                        ++nextIt;
                        if (nextIt != objects.end()) {
                            objectID = nextIt->first;
                        } else {
                            objectID = objects.begin()->first;
                        }
                    } else {
                        objectID = objects.begin()->first;
                    }
                }

                if (std::find(evilObjects.begin(), evilObjects.end(), objectID) == evilObjects.end())
                    break;
            }
            
            auto map = kablammo_utils::parseKV(str);
            map[1] = numToString(objectID);

            editor->m_editorUI->pasteObjects(kablammo_utils::buildKV(map), true, true);

            editor->removeObject(object, false);
        }
	});
    
    (void) KablammoObject::registerObject({
		.explosionRadius = 9999,
        .skipRadiusCheck = true,
        .explosionFuse = 10,
        .explosionSound = "boing-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "best-bomb.png"_spr,
        .identifier = "fail",
        .name = "BEST BOMB",
        .description = "Destroys everything ever!!!11!!",
        .modifierText = "999x",
        .onExplode = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj) {
            float currentAngle = editor->m_gameState.m_cameraAngle;
            auto rotateAction = CCRepeat::create(
                CCSequence::create(
                    CallFuncExt::create([editor, currentAngle] {
                        editor->m_gameState.m_cameraAngle += 24.f;
                    }),
                    CCDelayTime::create(0.016f),
                    nullptr
                ), 15
            );
            editor->runAction(rotateAction);
        }
	});

    (void) KablammoObject::registerObject({
        .searchShape = SearchShape::Square,
		.explosionRadius = 10,
        .explosionFuse = 2.5,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "square-bomb.png"_spr,
        .identifier = "square-2x",
        .name = "Square 2x",
        .description = "Destroys Objects but Square!",
        .modifierText = "2x",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            KablammoObject::explodeObject(editor, object, kablammoObj->m_object->getPosition());
        }
	});

    (void) KablammoObject::registerObject({
        .searchShape = SearchShape::Diamond,
		.explosionRadius = 10,
        .explosionFuse = 2.5,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "diamond-bomb.png"_spr,
        .identifier = "diamond-2x",
        .name = "Diamond 2x",
        .description = "Destroys Objects but Diamond!",
        .modifierText = "2x",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            KablammoObject::explodeObject(editor, object, kablammoObj->m_object->getPosition());
        }
	});

    (void) KablammoObject::registerObject({
        .searchShape = SearchShape::Triangle,
		.explosionRadius = 10,
        .explosionFuse = 2.5,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "triangle-bomb.png"_spr,
        .identifier = "triangle-2x",
        .name = "Triangle 2x",
        .description = "Destroys Objects but Triangle!",
        .modifierText = "2x",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            KablammoObject::explodeObject(editor, object, kablammoObj->m_object->getPosition());
        }
	});

    (void) KablammoObject::registerObject({
        .searchShape = SearchShape::Star,
		.explosionRadius = 10,
        .explosionFuse = 2.5,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "star-bomb.png"_spr,
        .identifier = "star-2x",
        .name = "Star 2x",
        .description = "Destroys Objects but Star!",
        .modifierText = "2x",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            KablammoObject::explodeObject(editor, object, kablammoObj->m_object->getPosition());
        }
	});

    (void) KablammoObject::registerObject({
		.explosionRadius = 10,
        .explosionFuse = 2.5,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "spread-bomb.png"_spr,
        .identifier = "spread-2x",
        .name = "Spread 2x",
        .description = "Spreads objects out!",
        .modifierText = "2x",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            spread(editor, kablammoObj, distance, object);
        }
	});

    (void) KablammoObject::registerObject({
		.explosionRadius = 10,
        .explosionFuse = 7,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "scale-spread-bomb.png"_spr,
        .identifier = "scale-spread-2x",
        .name = "Scale Spread 2x",
        .description = "Scales and Spreads objects out!",
        .modifierText = "2x",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            scaleSpread(editor, kablammoObj, distance, object);
        }
	});

    (void) KablammoObject::registerObject({
		.explosionRadius = 7.5,
        .explosionFuse = 1,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "black-hole-bomb.png"_spr,
        .identifier = "black-hole-0.5x",
        .name = "Micro Hole",
        .description = "Everything will be consumed.",
        .modifierText = "0.5x",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            blackHole(editor, kablammoObj, distance, object);
        }
	});

    (void) KablammoObject::registerObject({
		.explosionRadius = 15,
        .explosionFuse = 4,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "standard-bomb.png"_spr,
        .identifier = "standard-3x",
        .name = "Standard 3x",
        .description = "Destroys Objects!",
        .modifierText = "3x",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            KablammoObject::explodeObject(editor, object, kablammoObj->m_object->getPosition());
        }
	});

    (void) KablammoObject::registerObject({
		.explosionRadius = 15,
        .explosionFuse = 4,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "gay-bomb.png"_spr,
        .identifier = "gay-3x",
        .name = "Gay Bomb 3x",
        .description = "Makes the objects around it fruity!",
        .modifierText = "3x",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            gay(editor, kablammoObj, distance, object);
        }
	});

    (void) KablammoObject::registerObject({
		.explosionRadius = 15,
        .explosionFuse = 4,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "random-id-bomb.png"_spr,
        .identifier = "random-id-3x",
        .name = "Random ID 3x",
        .description = "Replaces the object ID with a random value!",
        .modifierText = "3x",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            randomizeIDs(editor, kablammoObj, distance, object);
        }
	});

    (void) KablammoObject::registerObject({
        .searchShape = SearchShape::Square,
		.explosionRadius = 15,
        .explosionFuse = 4,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "square-bomb.png"_spr,
        .identifier = "square-3x",
        .name = "Square 3x",
        .description = "Destroys Objects but Square!",
        .modifierText = "3x",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            KablammoObject::explodeObject(editor, object, kablammoObj->m_object->getPosition());
        }
	});

    (void) KablammoObject::registerObject({
        .searchShape = SearchShape::Diamond,
		.explosionRadius = 15,
        .explosionFuse = 4,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "diamond-bomb.png"_spr,
        .identifier = "diamond-3x",
        .name = "Diamond 3x",
        .description = "Destroys Objects but Diamond!",
        .modifierText = "3x",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            KablammoObject::explodeObject(editor, object, kablammoObj->m_object->getPosition());
        }
	});

    (void) KablammoObject::registerObject({
        .searchShape = SearchShape::Triangle,
		.explosionRadius = 15,
        .explosionFuse = 4,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "triangle-bomb.png"_spr,
        .identifier = "triangle-3x",
        .name = "Triangle 3x",
        .description = "Destroys Objects but Triangle!",
        .modifierText = "3x",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            KablammoObject::explodeObject(editor, object, kablammoObj->m_object->getPosition());
        }
	});

    (void) KablammoObject::registerObject({
        .searchShape = SearchShape::Star,
		.explosionRadius = 15,
        .explosionFuse = 4,
        .explosionSound = "standard-explosion.ogg"_spr,
        .fuseSound = "standard-fuse.ogg"_spr,
        .bombSprite = "star-bomb.png"_spr,
        .identifier = "star-3x",
        .name = "Star 3x",
        .description = "Destroys Objects but Star!",
        .modifierText = "3x",
        .objectModifier = [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, float distance, GameObject* object) {
            KablammoObject::explodeObject(editor, object, kablammoObj->m_object->getPosition());
        }
	});
}