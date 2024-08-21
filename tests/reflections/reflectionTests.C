#include "argList.H"
#include "catch2/catch_all.hpp"
#include "catch2/catch_message.hpp"
#include "catch2/catch_test_macros.hpp"
#include "origOpenFOAMModel.H"
#include "subCycleTime.H"
#include "uiModel.H"

using namespace Foam;

extern Time* timePtr;
extern argList* argsPtr;

TEST_CASE(
    "Unconfigured Refletion system supports basic patterns for OpenFOAM "
    "class member types both in constructive and documentation modes",
    "[cavity][serial]") {
    auto constructibleSkeleton =
        Reflect::reflect<origOpenFOAMModel, true>::schema(dictionary::null);
    auto skeleton =
        Reflect::reflect<origOpenFOAMModel, false>::schema(dictionary::null);
    SECTION("non constexpr default-constructible reports compiler default") {
        CHECK_FALSE(constructibleSkeleton.get<label>("m") == 15);
        REQUIRE(constructibleSkeleton.get<label>("m") == label());
    }
    SECTION("non constexpr, not-default-constructible reports empty value") {
        REQUIRE(skeleton.subDict("ndc").get<string>("type") ==
                string("Foam::OFstream"));
    }
    SECTION("pointer types report onDemand tag and defaults underlying type") {
        REQUIRE(skeleton.subDict("name").get<string>("type").startsWith(
            "<<onDemand>>"));
        REQUIRE(skeleton.subDict("name").get<string>("type").endsWith(
            "Foam::word"));
    }
    SECTION("RTS options get reported for a base model class") {
        auto tp = origOpenFOAMModel::typeName + "Type";
        const auto& sub = skeleton.subDict(tp);
        REQUIRE(sub.get<string>("default").startsWith("\"<<RTSoption>>"));
        REQUIRE(sub.get<string>("default").endsWith(
            "( origChild1Model origChild2Model )\""));
    }
}

TEST_CASE(
    "Configured docs-mode Refletion system fetches all concrete class members "
    "in an RTS",
    "[cavity][serial]") {
    dictionary childConfig;
    childConfig.set("origOpenFOAMModelType", "origChild2Model");
    auto childSkel =
        Reflect::reflect<origOpenFOAMModel, false>::schema(childConfig);
    SECTION(
        "child models report their concrete type as value in selection "
        "keyword") {
        CHECK(childSkel.subDict("origOpenFOAMModelType").get<word>("default") ==
              "origChild2Model");
    }
    SECTION(
        "child models report reflected members of their base alongside "
        "their own") {
        bool foundAllMembers = true;
        for (auto e : {"name", "m", "dir", "ht", "ndc", "labels"}) {
            foundAllMembers = foundAllMembers and childSkel.isDict(e);
        }
        CHECK(foundAllMembers);                    // child only reflects labels
        CHECK_FALSE(childSkel.found("someDict"));  // not-reflected member don't
    }
}

TEMPLATE_TEST_CASE_SIG(
    "Configured Refletion system supports nested concrete "
    "RTS models in constructive and docs mode",
    "[cavity][serial]",
    ((bool mode), mode),
    false,
    true) {
    dictionary childConfig;
    childConfig.set("origOpenFOAMModelType", "origChild1Model");
    childConfig.set("subModelType", "origChild2Model");
    auto recursiveSkel =
        Reflect::reflect<origOpenFOAMModel, mode>::schema(childConfig);
    CHECK(recursiveSkel.isDict("subModel"));  // subModel is a sub dictionary
    const auto& sub = recursiveSkel.subDict("subModel");
    bool foundAllSubMembers = true;
    for (auto e : {"name", "m", "dir", "ht", "ndc"}) {
        foundAllSubMembers = foundAllSubMembers and sub.found(e);
    }
    CHECK(foundAllSubMembers);
}

TEST_CASE(
    "Unconfigured reflection system is able to build concrete objects "
    "without explicit headers inclusion",
    "[cavity][serial]") {
    word childModel =
        GENERATE(as<word>(), "origChild1Model", "origChild2Model");
    dictionary childConfig;
    childConfig.set("origOpenFOAMModelType", childModel);
    if (childModel == "origChild1Model")
        childConfig.set("subModelType", "origChild2Model");
    auto skel = Reflect::reflect<origOpenFOAMModel, true>::schema(childConfig);
    SECTION("typeName matches the concrete type") {
        autoPtr<origOpenFOAMModel> model = origOpenFOAMModel::New(skel);
        REQUIRE(model->verifyType() == childModel);
    }
    SECTION("But default-constructed member values do not match skeletons") {
        label mInSkel(skel.get<label>("m"));
        skel.remove("m");
        autoPtr<origOpenFOAMModel> model = origOpenFOAMModel::New(skel);
        REQUIRE_FALSE(model->m() == mInSkel);
    }
}

TEST_CASE(
    "Unconfigured reflection system supports attributes of UI element members",
    "[cavity][serial]") {
    auto skeleton = Reflect::reflect<uiModel, false>::schema(dictionary::null);
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
    word childModel = GENERATE(as<word>(), "uiChildModel");
    dictionary childConfig;
    childConfig.set("uiModelType", childModel);
    auto skeleton = Reflect::reflect<uiModel, true>::schema(childConfig);
    autoPtr<uiModel> ui = uiModel::New(skeleton);
    auto docsSkel = Reflect::reflect<uiModel, false>::schema(childConfig);
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
