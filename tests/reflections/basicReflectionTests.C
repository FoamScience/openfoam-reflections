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
    "Unconfigured refletion supports basic reflectable OpenFOAM classes "
    "both in constructive and documentation modes",
    "[cavity][serial]") {

    // Construct skeleton dictionaries for docs/construction
    auto constructibleSkeleton =
        Reflect::reflect<basicReflectedModel, true>::schema(dictionary::null);
    auto docsSkeleton =
        Reflect::reflect<basicReflectedModel, false>::schema(dictionary::null);

    SECTION("compiler defaults are reported for non constexpr default-constructible types") {
        CHECK_FALSE(constructibleSkeleton.get<label>("m") == 15);
        REQUIRE(constructibleSkeleton.get<label>("m") == label());
    }

    SECTION("empty values are reported for non constexpr non default-constructible types") {
        REQUIRE(docsSkeleton.subDict("ndc").get<string>("type") ==
                string("Foam::OFstream"));
    }

    SECTION("onDemand tag and defaults underlying type are reported for pointer types") {
        REQUIRE(docsSkeleton.subDict("name").get<string>("type").startsWith(
            "<<onDemand>>"));
        REQUIRE(docsSkeleton.subDict("name").get<string>("type").endsWith(
            "Foam::word"));
    }

    SECTION("RTS options are reported for a base model class") {
        auto tp = basicReflectedModel::typeName + "Type";
        const auto& sub = docsSkeleton.subDict(tp);
        REQUIRE(sub.get<string>("default").startsWith("\"<<RTSoption>>"));
        REQUIRE(sub.get<string>("default").endsWith(
            "( child2BasicReflectedModel child1BasicReflectedModel )\""));
        REQUIRE(constructibleSkeleton.get<string>(tp).startsWith("\"<<RTSoption>>"));
        REQUIRE(constructibleSkeleton.get<string>(tp).endsWith(
            "( child2BasicReflectedModel child1BasicReflectedModel )\""));
    }

}

TEST_CASE(
    "Configured refletion supports basic reflectable abstract RTS OpenFOAM classes "
    "both in documentation mode",
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
    "Configured refletion supports basic reflectable concrete RTS OpenFOAM classes "
    "both in documentation mode",
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

    dictionary reflectionConfig;
    auto toc = basicReflectedModel::dictionaryConstructorTablePtr_->toc();
    auto concreteType = GENERATE_REF(from_range(toc.begin(), toc.end()));
    word key = basicReflectedModel::typeName+"Type";
    reflectionConfig.set<string>(key, concreteType);
    if (concreteType == "child1BasicReflectedModel")
        reflectionConfig.set("subModelType", "child2BasicReflectedModel");

    auto skel = Reflect::reflect<basicReflectedModel, true>::schema(reflectionConfig);

    SECTION("typeName matches the concrete type") {
        autoPtr<basicReflectedModel> model = basicReflectedModel::New(skel);
        REQUIRE(model->verifyType() == concreteType);
    }

    SECTION("default-constructed members do not match values in skeleton") {
        label mInSkel(skel.get<label>("m"));
        // REMOVE m from skeleton so "standard initialization for m"
        // takes place
        skel.remove("m");
        autoPtr<basicReflectedModel> model = basicReflectedModel::New(skel);
        REQUIRE_FALSE(model->m() == mInSkel);
    }
}
