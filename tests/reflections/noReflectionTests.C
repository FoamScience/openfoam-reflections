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

TEMPLATE_TEST_CASE(
    "Unconfigured Refletion supports original OpenFOAM classes through "
    "wrapping both in constructive and documentation modes",
    "[cavity][serial]",
    vector, tensor) {
    using reflectedType = Reflect::wrapAbstract<OpenFOAMModel<TestType>>;
    auto constructibleSkeleton =
        Reflect::reflect<reflectedType, true>::schema(dictionary::null);
    auto skeleton =
        Reflect::reflect<reflectedType, false>::schema(dictionary::null);
    SECTION("non constexpr default-constructible reports compiler default") {
        CHECK_FALSE(constructibleSkeleton.template get<TestType>("m") == TestType::one);
        REQUIRE(constructibleSkeleton.template get<TestType>("m") == TestType::zero);
    }
    SECTION("non constexpr, not-default-constructible reports empty value") {
        REQUIRE(skeleton.subDict("ndc").template get<string>("type") ==
                string("Foam::OFstream"));
    }
    SECTION("pointer types report onDemand tag and defaults underlying type") {
        REQUIRE(skeleton.subDict("name").template get<string>("type").startsWith(
            "<<onDemand>>"));
        REQUIRE(skeleton.subDict("name").template get<string>("type").endsWith(
            "Foam::word"));
    }
    SECTION("RTS options get reported for a base model class") {
        auto tp = reflectedType::wrappedType::typeName + "Type";
        const auto& sub = skeleton.subDict(tp);
        REQUIRE(sub.template get<string>("default").startsWith("\"<<RTSoption>>"));
        REQUIRE(sub.template get<string>("default").endsWith(
            string("( Foam::childOpenFOAMModel<Foam::")+TestType::typeName+"> )\""));
    }
}
