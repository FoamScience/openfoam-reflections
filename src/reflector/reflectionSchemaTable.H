#ifndef reflectionSchemaTable_H
#define reflectionSchemaTable_H

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
    /* TODO: better T name to avoid shadowing */                               \
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

#define addToSchemaTable(baseType, childType)                                  \
    baseType::addSchemaToTable<childType> add##childType##SchemaToSchemaTable( \
        childType::typeName)

} // namespace Foam

#endif
