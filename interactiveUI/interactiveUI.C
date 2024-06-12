#include "baseModel.H"
#include "childModel.H" // TODO: remove this dependency, it's here for testing only
#include "anotherModel.H" // TODO: remove this dependency, it's here for testing only
#include "crow/middlewares/cors.h"
#include "dictionary.H"
#include "refl.H"
#include "uiBuilder.H"
#include "uiConcepts.H"
#include <crow.h>
#include <string>
#include <type_traits>

using namespace Foam;

template<class T, class B = T>
static dictionary
reflectAndBuildDictionary()
{
    dictionary dict;
    using pureName = std::remove_cvref_t<T>;
    using pureBName = std::remove_cvref_t<B>;
    constexpr auto type = refl::reflect<pureName>();
    constexpr bool isRTSManagedBase = ui::DictionaryRTSModelBase<pureName>;
    constexpr bool isRTSManagedImpl =
      ui::DictionaryRTSModelImpl<pureName, pureBName>;
    constexpr bool isAbstractModel = std::is_abstract_v<pureName>;
    constexpr bool hasSchema = ui::SelfReflectableModel<pureName>;
    Info << "Generating schema dictionary for " << word(type.name.str())
         << endl;
    Info << "---- A base for RTS models? -> " << isRTSManagedBase << endl;
    Info << "---- An implementation for "
         << ui::builder<pureName>::demangle(typeid(pureName).name())
         << " RTS models ? -> " << isRTSManagedImpl << endl;
    Info << "---- Is abstract? -> " << isAbstractModel << endl;
    Info << "---- Can self-reflect? -> " << hasSchema << endl;
    if constexpr (hasSchema) {
        dict = ui::builder<pureName>::schema();
    }
    return dict;
}

template<class T, class B = T>
static crow::json::wvalue
reflectAndBuildClassInfo()
{
    crow::json::wvalue json;
    using pureName = std::remove_cvref_t<T>;
    using pureBName = std::remove_cvref_t<B>;
    constexpr auto type = refl::reflect<pureName>();
    constexpr bool isRTSManagedBase = ui::DictionaryRTSModelBase<pureName>;
    constexpr bool isRTSManagedImpl =
      ui::DictionaryRTSModelImpl<pureName, pureBName>;
    constexpr bool isAbstractModel = std::is_abstract_v<pureName>;
    constexpr bool hasSchema = ui::SelfReflectableModel<pureName>;
    Info << "Generating schema dictionary for " << word(type.name.str())
         << endl;
    Info << "---- A base for RTS models? -> " << isRTSManagedBase << endl;
    Info << "---- An implementation for "
         << ui::builder<pureName>::demangle(typeid(pureName).name())
         << " RTS models ? -> " << isRTSManagedImpl << endl;
    Info << "---- Is abstract? -> " << isAbstractModel << endl;
    Info << "---- Can self-reflect? -> " << hasSchema << endl;
    if constexpr (hasSchema) {
        auto processKey = [](word key) {
            key.removeEnd('_');
            return key;
        };

        {
            crow::json::wvalue tp_json;
            tp_json["key"] = "Class TypeName";
            tp_json["description"] = "No description for this item";
            tp_json["value"] = string(type.name.str());
            json["Class TypeName"] = std::move(tp_json);
        }

        using pureTypeName = std::remove_cvref_t<T>;
        constexpr auto type = refl::reflect<pureTypeName>();
        if constexpr (ui::DictionaryRTSModelBase<pureTypeName>) {
            crow::json::wvalue base_json;
            base_json["key"] = string(pureTypeName::typeName) + "Type";
            base_json["description"] = "No description for this item";
            base_json["value"] =
              ui::builder<T>::template getRTSJSONOptions<pureTypeName>();
            json[string(pureTypeName::typeName) + "Type"] = std::move(base_json);
        }

        for_each(type.members, [&](auto member) {
            crow::json::wvalue member_json;
            member_json["key"] = string(processKey(member.name.str()));
            if constexpr (refl::descriptor::is_field(member)) {
                using memberType = typename decltype(member)::value_type;
                auto memberTypeName = ui::builder<pureTypeName>::demangle(
                  typeid(memberType).name());
                if constexpr (ui::isUIMember<memberType>) {
                    using nestedType = typename memberType::value_type;
                    memberTypeName = ui::builder<nestedType>::demangle(
                      typeid(nestedType).name());
                }
                if constexpr (ui::SomePtr<memberType>) {
                    // If a pointer; check if it is a pointer to a base RTS
                    // class
                    using elementType = typename memberType::element_type;
                    if constexpr (ui::DictionaryRTSModelBase<elementType>) {
                        // dictionary newDict =
                        // ui::builder<elementType>::schemaFromUser();
                        Info << "Choose a Model from"
                             << elementType::schemasPtr_->toc() << endl;
                        word modelType;
                        std::getline(std::cin, modelType);
                        member_json["value"] = typeid(elementType).name();
                        member_json["description"] = "No description for this item";
                    } else {
                        auto ftype = ui::builder<elementType>::demangle(
                          typeid(elementType).name());
                        word fname =
                          ui::builder<elementType>::familiarNaming(ftype);
                        string key = "__optional " + fname + " here__";
                        member_json["value"] = key;
                        member_json["description"] = "No description for this item";
                    }
                }
                if constexpr (ui::isUIMember<memberType>) {
                    using vType = typename memberType::value_type;
                    word ftype =
                      Foam::ui::builder<vType>::familiarNaming(memberTypeName);
                    dictionary dict;
                    if constexpr (ui::SomePtr<vType>) {
                        word key = word("__optional ") + ftype + "__";
                        dict.set("type", key);
                    } else {
                        dict.set("type", memberType::init());
                    }
                    member_json["value"] = dict.lookup("type").toString();
                    member_json["description"] = std::string(memberType::descr());
                } else {
                    word ftype =
                      ui::builder<pureTypeName>::familiarNaming(memberTypeName);
                    word key = word("__value for ") + ftype + "__";
                    json["value"] = key;
                    member_json["description"] = "No description for this item";
                }
            }
            json[processKey(member.name.str())] = std::move(member_json);
        });
    }
    return json;
}

int
main(int argc, char* argv[])
{
    mff::baseModel::debug = 1;
    Info << "Choose a Model from available baseModels:" << endl;
    Info << mff::baseModel::schemasPtr_->toc() << endl;
    word modelType;
    std::getline(std::cin, modelType);
    Info << mff::baseModel::schema(modelType) << endl;
    // Info << "--------------------------------------" << endl;
    // Info << "The baseModel is reflected as:" << endl;
    // Info << reflectAndBuildDictionary<mff::baseModel>() << endl;
    // Info << "--------------------------------------" << endl;
    // Info << reflectAndBuildDictionary<mff::childModel, mff::baseModel>() <<
    // endl; Info << "--------------------------------------" << endl;
    crow::App<crow::CORSHandler> app;
    auto& cors = app.get_middleware<crow::CORSHandler>();
    cors.global()
      .headers("X-Custom-Header", "Upgrade-Insecure-Requests")
      .methods("POST"_method, "GET"_method)
      .origin("*");
    CROW_ROUTE(app, "/")([]() { return "Hello world"; });

    // Endpoint to get all classes
    CROW_ROUTE(app, "/classes/mff__baseModel")
    ([]() {
        return crow::response(reflectAndBuildClassInfo<mff::baseModel>());
    });

    CROW_ROUTE(app, "/classes/mff__childModel")
    ([]() {
        return crow::response(reflectAndBuildClassInfo<mff::childModel>());
    });

    CROW_ROUTE(app, "/classes/mff__anotherModel")
    ([]() {
        return crow::response(reflectAndBuildClassInfo<mff::anotherModel>());
    });

    // Endpoint to get details of a specific class by name
    // CROW_ROUTE(app, "/class/<string>")
    //([](const std::string& className) {
    //    auto classInfo = reflectAndBuildClassInfo(className); // Function to
    //    get class info by name if (!classInfo.has_value()) {
    //        return crow::response(404);
    //    }
    //    return crow::response(generate_class_info(classInfo.value()));
    //});

    app.port(18080).run();
    return 0;
}
