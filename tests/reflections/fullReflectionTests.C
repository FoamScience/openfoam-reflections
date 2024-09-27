/*
 * Reaction of the reflection system to OpenFOAM classes with
 * full reflection support where all reflected members are
 * UI elements with descriptions and defaults
*/

#include "argList.H"
#include "catch2/catch_all.hpp"
#include "catch2/catch_message.hpp"
#include "catch2/catch_test_macros.hpp"
#include "subCycleTime.H"
#include "fullReflectedModel.H"

using namespace Foam;

extern Time* timePtr;
extern argList* argsPtr;

TEST_CASE(
    "Unconfigured reflection system supports attributes of UI element members",
    "[cavity][serial]") {
    auto skeleton = Reflect::reflect<fullReflectedModel, false>::schema(dictionary::null);
    SECTION("uiElement members report description") {
        CHECK(skeleton.subDict("m").found("description"));
    }
    SECTION("uiElement members report min if available") {
        CHECK(skeleton.subDict("m").found("min"));
        REQUIRE(skeleton.subDict("m").get<string>("min") == "10");
    }
    SECTION("uiElement members report max if available") {
        CHECK(skeleton.subDict("m").found("max"));
        REQUIRE(skeleton.subDict("m").get<string>("max") == "20");
    }
}

TEST_CASE(
    "Configured reflection system supports attributes of UI element members",
    "[cavity][serial]") {
    word childModel = GENERATE(as<word>(), "childFullReflectedModel");
    dictionary childConfig;
    childConfig.set("fullReflectedModelType", childModel);
    auto skeleton = Reflect::reflect<fullReflectedModel, true>::schema(childConfig);
    autoPtr<fullReflectedModel> ui = fullReflectedModel::New(skeleton);
    auto docsSkel = Reflect::reflect<fullReflectedModel, false>::schema(childConfig);
    SECTION("uiElement members report default-constructed value") {
        REQUIRE(ui->m() == skeleton.get<label>("m"));
        REQUIRE(ui->ht() == skeleton.get<HashTable<word>>("ht"));
    }
    SECTION(
        "child models report uiElement members of their own and of their "
        "base") {
        bool foundAllSubMembers = true;
        for (auto e : {"name", "m", "dir", "ht", "ndc", "subModel"}) {
            foundAllSubMembers = foundAllSubMembers and docsSkel.found(e);
        }
        CHECK(foundAllSubMembers);
    }
}
