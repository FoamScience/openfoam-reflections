#ifndef reflectionSchemaTable_H
#define reflectionSchemaTable_H

// refl.H's built-in std::exception/basic_string/tuple/... reflection
// metadata uses an attribute tag named 'debug', ambiguous with OpenFOAM's
// own pervasive Foam::debug. Must be defined before refl.H's FIRST
// inclusion in a translation unit - see also reflectConcepts.H.
#define REFL_NO_STD_SUPPORT
#include "refl.H"
#include "reflectConcepts.H"
#include "reflectDictionary.H"

namespace Foam {

#define declareSchemaTable(baseType, funcName, funcReturn, funcSignature)      \
    /* Type of a function pointer to the schema */                             \
    using schemaBuilderPtr = funcReturn(*) funcSignature;                      \
    /* Type of schema table */                                                 \
    using schemaTable = HashTable<schemaBuilderPtr, word, string::hash>;       \
    /* The schema table */                                                     \
    static schemaTable *schemasPtr_;                                           \
    /* Construction of a schema table */                                       \
    static void constructSchemaTables();                                       \
    /* Destruction of a schema table */                                        \
    static void destroySchemaTables();                                         \
                                                                               \
    /* Subclass to add a schema to the table */                                \
    /* template<Reflect::SelfReflectableModel T> */                            \
    template <class T> struct addSchemaToTable {                               \
        /* Constructor adding schema builder function to table */              \
        explicit addSchemaToTable(const word &lookup = baseType::typeName) {   \
            constructSchemaTables();                                           \
            if (!schemasPtr_->insert(lookup,                                   \
                                     Reflect::reflect<T, false>::funcName)) {  \
                FatalErrorInFunction << "Duplicate entry for " << lookup       \
                                     << " in schema table."                    \
                                     << exit(FatalError);                      \
            };                                                                 \
            if (!schemasPtr_->insert(lookup + "Construct",                     \
                                     Reflect::reflect<T, true>::funcName)) {   \
                FatalErrorInFunction << "Duplicate entry for " << lookup       \
                                     << " in schema table."                    \
                                     << exit(FatalError);                      \
            }                                                                  \
        }                                                                      \
        /* Destructor destorying schema tables */                              \
        ~addSchemaToTable() { destroySchemaTables(); }                         \
        /* Disallow copy construction */                                       \
        addSchemaToTable(const addSchemaToTable &) = delete;                   \
        /* Disallow copy assignment */                                         \
        void operator=(const addSchemaToTable &) = delete;                     \
    }

#define defineSchemaTable(baseType)                                            \
    baseType::schemaTable *baseType::schemasPtr_(nullptr);                     \
    void baseType::constructSchemaTables() {                                   \
        if (baseType::schemasPtr_ == nullptr) {                                \
            baseType::schemasPtr_ = new baseType::schemaTable;                 \
        }                                                                      \
    }                                                                          \
    void baseType::destroySchemaTables() {                                     \
        if (baseType::schemasPtr_) {                                           \
            delete baseType::schemasPtr_;                                      \
            baseType::schemasPtr_ = nullptr;                                   \
        }                                                                      \
    }

/* Same as defineSchemaTable, but for a specific instantiation of a class  */
/* template (e.g. accelerationModel<scalar>) - defining members of one    */
/* instantiation out-of-line is an explicit specialization, which GCC     */
/* requires a leading "template<>" for. */
#define defineTemplateSchemaTable(baseType)                                    \
    template<>                                                                 \
    baseType::schemaTable *baseType::schemasPtr_(nullptr);                     \
    template<>                                                                 \
    void baseType::constructSchemaTables() {                                   \
        if (baseType::schemasPtr_ == nullptr) {                                \
            baseType::schemasPtr_ = new baseType::schemaTable;                 \
        }                                                                      \
    }                                                                          \
    template<>                                                                 \
    void baseType::destroySchemaTables() {                                     \
        if (baseType::schemasPtr_) {                                           \
            delete baseType::schemasPtr_;                                      \
            baseType::schemasPtr_ = nullptr;                                   \
        }                                                                      \
    }

#define reflectTokenPasteImpl(x, y) x##y
#define reflectTokenPaste(x, y) reflectTokenPasteImpl(x, y)

/* childType may be a template-id (e.g. fixedRelaxation<scalar>), which is  */
/* several preprocessor tokens - can't paste it into an identifier. Name   */
/* the table entry off __LINE__ instead; uniqueness only needs one call    */
/* per line, true for every existing and template use. */
#define addToSchemaTable(baseType, childType)                                  \
    baseType::addSchemaToTable<childType>                                      \
        reflectTokenPaste(addSchemaToSchemaTable_, __LINE__)(childType::typeName)

} // namespace Foam

#endif
