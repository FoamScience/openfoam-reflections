/*
 * Reaction of the reflection system to OpenFOAM classes with
 * basic reflection support but all members are types with 0-lvl
 * of reflection support
*/

#include "argList.H"
#include "catch2/catch_all.hpp"
#include "catch2/catch_message.hpp"
#include "catch2/catch_test_macros.hpp"
#include "subCycleTime.H"
#include "basicReflectedModel.H"

using namespace Foam;

extern Time* timePtr;
extern argList* argsPtr;

TEST_CASE(
    "Unconfigured Refletion supports basic patterns for OpenFOAM "
    "class member types both in constructive and documentation modes",
    "[cavity][serial]") {
    auto constructibleSkeleton =
        Reflect::reflect<basicReflectedModel, true>::schema(dictionary::null);
    auto skeleton =
        Reflect::reflect<basicReflectedModel, false>::schema(dictionary::null);
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
        auto tp = basicReflectedModel::typeName + "Type";
        const auto& sub = skeleton.subDict(tp);
        REQUIRE(sub.get<string>("default").startsWith("\"<<RTSoption>>"));
        REQUIRE(sub.get<string>("default").endsWith(
            "( child2BasicReflectedModel child1BasicReflectedModel )\""));
    }
}

TEST_CASE(
    "Configured docs-mode Refletion system fetches all concrete class members "
    "in an RTS",
    "[cavity][serial]") {
    dictionary childConfig;
    childConfig.set("basicReflectedModelType", "child2BasicReflectedModel");
    auto childSkel =
        Reflect::reflect<basicReflectedModel, false>::schema(childConfig);
    SECTION(
        "child models report their concrete type as value in selection "
        "keyword") {
        CHECK(childSkel.subDict("basicReflectedModelType").get<word>("default") ==
              "child2BasicReflectedModel");
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
    childConfig.set("basicReflectedModelType", "child1BasicReflectedModel");
    childConfig.set("subModelType", "child2BasicReflectedModel");
    auto recursiveSkel =
        Reflect::reflect<basicReflectedModel, mode>::schema(childConfig);
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
        GENERATE(as<word>(), "child1BasicReflectedModel", "child2BasicReflectedModel");
    dictionary childConfig;
    childConfig.set("basicReflectedModelType", childModel);
    if (childModel == "child1BasicReflectedModel")
        childConfig.set("subModelType", "child2BasicReflectedModel");
    auto skel = Reflect::reflect<basicReflectedModel, true>::schema(childConfig);
    SECTION("typeName matches the concrete type") {
        autoPtr<basicReflectedModel> model = basicReflectedModel::New(skel);
        REQUIRE(model->verifyType() == childModel);
    }
    SECTION("But default-constructed member values do not match skeletons") {
        label mInSkel(skel.get<label>("m"));
        skel.remove("m");
        autoPtr<basicReflectedModel> model = basicReflectedModel::New(skel);
        REQUIRE_FALSE(model->m() == mInSkel);
    }
}
