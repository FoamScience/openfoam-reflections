#ifndef baseModel_H
#define baseModel_H

#include "IOdictionary.H"
#include "autoPtr.H"
#include "error.H"
#include "reflectionSchemaTable.H"
#include "runTimeSelectionTables.H"
#include "uiBuilder.H"
#include "uiMember.H"
#include "vector.H"

namespace Foam {

namespace mff {

/// @brief Base class for some stuff
/// @hyde-owner elwardi
class baseModel
{
  protected:
    /// Configuration dictionary
    const dictionary& dict_;

    ui::member<autoPtr<word>,
               []() { return autoPtr<word>(new word("key")); },
               []() { return "key member"; }
    > key_;
    //autoPtr<word> key_;

    /// Label
    ui::member<label,
               []() { return 12; },
               []() { return "m coefficient"; },
               []() { return 1; },
               []() { return 15; }> m_;
      // label m_;

      /// Vector
    ui::member<vector,
               []() { return vector(1, 2, 3); },
               []() { return "vv vector"; }
    > vv_;
      //vector vv_;

  public:

    /// Runtime type name
    TypeName("baseModel");

    /// @brief RunTime selection table for baseModels
    declareRunTimeSelectionTable(autoPtr,
                                 baseModel,
                                 dictionary,
                                 (const dictionary& dict),
                                 (dict));

    /// @brief Schema table for baseModels
    declareSchemaTable(baseModel, schemaFromUser, dictionary);

    /// Construct from dictionary
    explicit baseModel(const dictionary& dict);

    /// Move construct
    baseModel(baseModel&& other) = default;

    /// Delete default copy construct
    baseModel(const baseModel& other) = delete;

    /// Destruct baseModels
    virtual ~baseModel();

    /// @return A dynamic clone of this
    virtual autoPtr<baseModel> clone() const = 0;

    /// @return The concrete `baseRTSTemplate` from dictionary
    /// as a pointer to base type
    static autoPtr<baseModel> New(const dictionary& dict);

    /// @return a dictionary reflecting this object
    static dictionary schema(const word& modelType)
    {
        auto cstrIter = schemasPtr_->cfind(modelType);
        dictionary dict;
        dict.set(word(typeName + "Type"), modelType);

        if (!cstrIter.found()) {
            FatalErrorInFunction << modelType
                                 << " was not found in schema table."
                                 << exit(FatalError);
        }

        return dict + cstrIter()();
    }

    /// @return the underlying dictionary
    const dictionary& dict() const;

    /// Deleted move assignment
    baseModel& operator=(baseModel&& other) = delete;

    /// Deleted copy assignment
    baseModel& operator=(const baseModel& other) = delete;

    friend class refl_impl::metadata::type_info__<baseModel>;
};

} /// !namespace mff

} /// !namespace Foam

REFL_AUTO(type(Foam::mff::baseModel),
          field(key_),
          field(m_),
          field(vv_))

#endif
