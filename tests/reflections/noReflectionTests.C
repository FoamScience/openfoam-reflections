/*
 * Reaction of the reflection system to OpenFOAM classes with
 * no reflection support
*/

#include "argList.H"
#include "catch2/catch_all.hpp"
#include "catch2/catch_message.hpp"
#include "subCycleTime.H"
#include "catch2/catch_test_macros.hpp"
#include "OpenFOAMModelReflect.H"

using namespace Foam;

extern Time* timePtr;
extern argList* argsPtr;

TEMPLATE_TEST_CASE_SIG(
    "Unconfigured Refletion supports original OpenFOAM classes through seamless "
    "wrapping both in constructive and documentation modes, penetrating pointer types.",
    "[cavity][serial]",
    ((class TestType, int ii), TestType, ii),
    (vector,0), (vector, 1),
    (tensor,0), (tensor, 1)
) {
    // Ensuring the API largely looks the same if reflecting through
    // a pointer; => side-effect: no reflection for pointer type itself
    using ModelTypeList = refl::type_list<
        OpenFOAMModel<TestType>, 
        autoPtr<OpenFOAMModel<TestType>>
    >;
    using reflectedType = refl::trait::get_t<ii, ModelTypeList>;

    // Construct skeleton dictionaries for docs/construction
    auto constructibleSkeleton =
        Reflect::reflect<reflectedType, true>::schema(dictionary::null);
    auto docsSkeleton =
        Reflect::reflect<reflectedType, false>::schema(dictionary::null);

    SECTION("compiler defaults are reported for non constexpr default-constructible types") {
        CHECK_FALSE(constructibleSkeleton.template get<TestType>("m") == TestType::one);
        REQUIRE(constructibleSkeleton.template get<TestType>("m") == TestType::zero);
    }

    SECTION("empty values are reported for non constexpr non default-constructible types") {
        REQUIRE(docsSkeleton.subDict("ndc").template get<string>("type") ==
                string("Foam::OFstream"));
    }

    SECTION("onDemand tag and defaults underlying type are reported for pointer types") {
        REQUIRE(docsSkeleton.subDict("name").template get<string>("type").startsWith(
            "<<onDemand>>"));
        REQUIRE(docsSkeleton.subDict("name").template get<string>("type").endsWith(
            "Foam::word"));
    }

    SECTION("RTS options are reported for a base model class") {
        word tp;
        if constexpr (requires { reflectedType::element_type::typeName; }) {
            tp = reflectedType::element_type::typeName + "Type";
        } else {
            tp = reflectedType::typeName + "Type";
        }
        const auto& sub = docsSkeleton.subDict(tp);
        REQUIRE(sub.template get<string>("default").startsWith("\"<<RTSoption>>"));
        REQUIRE(sub.template get<string>("default").endsWith(
            string("( Foam::childOpenFOAMModel<Foam::")+TestType::typeName+"> )\""));
        REQUIRE(constructibleSkeleton.template get<string>(tp).startsWith("\"<<RTSoption>>"));
        REQUIRE(constructibleSkeleton.template get<string>(tp).endsWith(
            string("( Foam::childOpenFOAMModel<Foam::")+TestType::typeName+"> )\""));
    }

}

TEMPLATE_TEST_CASE(
    "Configured Refletion supports original OpenFOAM classes through seamless "
    "wrapping both in constructive and documentation modes, penetrating pointer types.",
    "[cavity][serial]",
    vector,
    tensor
) {
    dictionary reflectionConfig;
    using reflectedType = OpenFOAMModel<TestType>;
    auto toc = reflectedType::dictionaryConstructorTablePtr_->toc();
    auto concreteType = GENERATE_REF(from_range(toc.begin(), toc.end()));
    word key = reflectedType::typeName+"Type";
    reflectionConfig.set<string>(key, concreteType);

    // Construct skeleton dictionary for construction
    auto constructibleSkeleton =
        Reflect::reflect<reflectedType, true>::schema(reflectionConfig);

    // Check that construction of object passes...
    autoPtr<reflectedType> objPtr = reflectedType::New(constructibleSkeleton);

    SECTION(
        "child models report their concrete type as value in selection "
        "keyword") {
        CHECK(constructibleSkeleton.template get<string>(key) == concreteType);
    }

    SECTION(
        "child models report reflected members of their base alongside "
        "their own") {
        bool foundAllMembers = true;
        for (auto e : {"name", "m", "dir", "ht", "ndc", "list"}) {
            foundAllMembers = foundAllMembers and constructibleSkeleton.found(e);
        }
        CHECK(foundAllMembers);
        CHECK_FALSE(constructibleSkeleton.found("someDict"));
    }

}
