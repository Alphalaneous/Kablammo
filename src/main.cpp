#include "KablammoObject.hpp"

$execute {
	KablammoObject::registerObject({
		5,
        1.5,
        "standard-explosion.ogg"_spr,
        "standard-fuse.ogg"_spr,
        "standard",
        "Standard",
        "Removes a radius of objects",
        [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, GameObject* object) {
            editor->removeObject(object, false);
        }
	});
    KablammoObject::registerObject({
		10,
        3,
        "standard-explosion.ogg"_spr,
        "standard-fuse.ogg"_spr,
        "standard2",
        "Standard 2",
        "Removes a radius of objects 2",
        [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, GameObject* object) {
            editor->removeObject(object, false);
        }
	});
    KablammoObject::registerObject({
		15,
        4.5,
        "standard-explosion.ogg"_spr,
        "standard-fuse.ogg"_spr,
        "standard3",
        "Standard 3",
        "Removes a radius of objects 3",
        [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, GameObject* object) {
            editor->removeObject(object, false);
        }
	});
    KablammoObject::registerObject({
		20,
        6,
        "standard-explosion.ogg"_spr,
        "standard-fuse.ogg"_spr,
        "standard4",
        "Standard 4",
        "Removes a radius of objects 4",
        [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, GameObject* object) {
            editor->removeObject(object, false);
        }
	});
    KablammoObject::registerObject({
		25,
        7.5,
        "standard-explosion.ogg"_spr,
        "standard-fuse.ogg"_spr,
        "standard5",
        "Standard 5",
        "Removes a radius of objects 5",
        [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, GameObject* object) {
            editor->removeObject(object, false);
        }
	});
    KablammoObject::registerObject({
		30,
        9,
        "standard-explosion.ogg"_spr,
        "standard-fuse.ogg"_spr,
        "standard6",
        "Standard 6",
        "Removes a radius of objects 6",
        [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, GameObject* object) {
            editor->removeObject(object, false);
        }
	});
    KablammoObject::registerObject({
		35,
        10.5,
        "standard-explosion.ogg"_spr,
        "standard-fuse.ogg"_spr,
        "standard7",
        "Standard 7",
        "Removes a radius of objects 7",
        [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, GameObject* object) {
            editor->removeObject(object, false);
        }
	});
        KablammoObject::registerObject({
		40,
        12,
        "standard-explosion.ogg"_spr,
        "standard-fuse.ogg"_spr,
        "standard8",
        "Standard 8",
        "Removes a radius of objects 8",
        [] (LevelEditorLayer* editor, KablammoObject* kablammoObj, GameObject* object) {
            editor->removeObject(object, false);
        }
	});
}