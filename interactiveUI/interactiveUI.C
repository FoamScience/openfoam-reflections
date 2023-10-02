#include "dictionary.H"
#include "refl.H"
#include "uiConcepts.H"
#include "uiBuilder.H"
#include "baseModel.H"
#include "childModel.H"
#include <type_traits>

using namespace Foam;

template<class T>
static dictionary reflectAndBuildDictionary() {
    dictionary dict;
    using pureName = std::remove_cvref_t<T>;
    constexpr auto type = refl::reflect<pureName>();
    constexpr bool isRTSManagedModel = ui::DictionaryRTSModel<pureName>;
    constexpr bool isAbstractModel = std::is_abstract_v<pureName>;
    Info << "Filling dictionary for " << word(type.name.str()) << endl;
    Info << "---- (RTSManaged? -> " << isRTSManagedModel <<  " )" << endl;
    Info << "---- (IsAbstract? -> " << std::is_abstract_v<pureName> <<  " )" << endl;
    if (isRTSManagedModel && isAbstractModel) 
    {
        word concrete;
        Info << "Pick a model from: " << pureName::dictionaryConstructorTablePtr_->toc() << endl;
        std::getline(std::cin, concrete);
        dict.set(word(pureName::typeName + "Type"), concrete);
        // !!!Here, need to get the concrete type from a string!!!!
        //using concreteTypeName = decltype(pureName::dictionaryConstructorTablePtr_->operator[](concrete));
        //constexpr auto concreteType = refl::reflect<concreteTypeName>();
        //for_each(concreteType.members, [&](auto member) {
        //    if constexpr (refl::descriptor::is_field(member)) {
        //        auto name = ui::builder<pureName>::demangle(typeid(typename decltype(member)::value_type).name());
        //        bool isRTSManaged = ui::DictionaryRTSModel<typename decltype(member)::value_type>;
        //        word inName;
        //        Info
        //            << " Type the value of " << word(type.name.str()) << "::"
        //            << word(member.name.str()) << " ( a "
        //            << ui::builder<pureName>::familiarNaming(name) << ", RTSManaged? -> " << isRTSManaged << " )" << endl;
        //        std::getline(std::cin, inName);
        //        dict.set(word(member.name.str()), inName);
        //    }
        //});
    }

    for_each(type.members, [&](auto member) {
        if constexpr (refl::descriptor::is_field(member)) {
            auto name = ui::builder<pureName>::demangle(typeid(typename decltype(member)::value_type).name());
            bool isRTSManaged = ui::DictionaryRTSModel<typename decltype(member)::value_type>;
            word inName;
            Info
                << " Type the value of " << word(type.name.str()) << "::"
                << word(member.name.str()) << " ( a "
                << ui::builder<pureName>::familiarNaming(name) << ", RTSManaged? -> " << isRTSManaged << " )" << endl;
            std::getline(std::cin, inName);
            dict.set(word(member.name.str()), inName);
        }
    });
    return dict;
}


int main (int argc, char *argv[]) {
    Info << reflectAndBuildDictionary<mff::childModel>() << endl;   
    return 0;
}
