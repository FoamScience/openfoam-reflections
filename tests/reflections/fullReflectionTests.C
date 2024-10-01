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
    "Unconfigured refletion supports fully reflectable OpenFOAM classes "
    "both in constructive and documentation modes",
    "[cavity][serial]") {

    // Construct skeleton dictionary for docs
    auto docsSkeleton =
        Reflect::reflect<fullReflectedModel, false>::schema(dictionary::null);
    auto constructibleSkeleton =
        Reflect::reflect<fullReflectedModel, true>::schema(dictionary::null);

    SECTION("uiElement members report their descriptions") {
        CHECK(docsSkeleton.subDict("m").found("description"));
    }

    SECTION("uiElement members report min if available") {
        CHECK(docsSkeleton.subDict("m").found("min"));
        REQUIRE(docsSkeleton.subDict("m").get<string>("min") == "10");
    }

    SECTION("uiElement members report max if available") {
        CHECK(docsSkeleton.subDict("m").found("max"));
        REQUIRE(docsSkeleton.subDict("m").get<string>("max") == "20");
    }

}

TEST_CASE(
    "Configured refletion supports fully reflectable OpenFOAM classes "
    "both in constructive and documentation modes",
    "[cavity][serial]") {

    dictionary reflectionConfig;
    auto toc = fullReflectedModel::dictionaryConstructorTablePtr_->toc();
    auto concreteType = GENERATE_REF(from_range(toc.begin(), toc.end()));
    word key = fullReflectedModel::typeName+"Type";
    reflectionConfig.set<string>(key, concreteType);

    auto constructibleSkeleton =
        Reflect::reflect<fullReflectedModel, true>::schema(reflectionConfig);
    autoPtr<fullReflectedModel> ui = fullReflectedModel::New(constructibleSkeleton);

    auto docsSkeleton =
        Reflect::reflect<fullReflectedModel, false>::schema(reflectionConfig);

    SECTION("uiElement members report the default-constructed value") {
        OStringStream oss;
        REQUIRE(ui->m() == constructibleSkeleton.get<label>("m"));
        REQUIRE(Foam::name(ui->m()) == docsSkeleton.subDict("m").get<string>("default"));
        REQUIRE(ui->dir() == constructibleSkeleton.get<vector>("dir"));
        oss.reset();
        oss << ui->dir();
        REQUIRE(oss.str() == docsSkeleton.subDict("dir").get<string>("default"));
        oss.reset();
        oss << ui->ht();
        REQUIRE(ui->ht() == constructibleSkeleton.get<HashTable<word>>("ht"));
        REQUIRE(oss.str() == docsSkeleton.subDict("ht").get<string>("default"));
    }

    SECTION("uiElement members report changed values from defaults") {
        constructibleSkeleton.set("m", 1111);
        constructibleSkeleton.set("dir", vector{0,0,1});
        autoPtr<fullReflectedModel> nonStandardUI = fullReflectedModel::New(constructibleSkeleton);
        REQUIRE(nonStandardUI->m() == constructibleSkeleton.get<label>("m"));
        REQUIRE(nonStandardUI->dir() == constructibleSkeleton.get<vector>("dir"));
    }

    SECTION("child models report uiElement members of their own and of their base") {
        bool foundAllSubMembers = true;
        for (auto e : {"name", "m", "dir", "ht", "ndc", "subModel"}) {
            foundAllSubMembers = foundAllSubMembers and docsSkeleton.found(e);
        }
        CHECK(foundAllSubMembers);
    }
}
