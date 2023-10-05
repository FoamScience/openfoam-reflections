#ifndef reflectionSchemaTable_H
#define reflectionSchemaTable_H

#include "refl.H"

namespace Foam
{

#define declareSchemaTable(baseType, funcName, funcReturn) \
    /* Type of a function pointer to the schema */ \
    using schemaBuilderPtr = funcReturn (*) (); \
    /* Type of schema table */ \
    using schemaTable = HashTable<schemaBuilderPtr, word, string::hash>; \
    /* The schema table */ \
    static schemaTable* schemasPtr_; \
    /* Construction of a schema table */ \
    static void constructSchemaTables(); \
    /* Destruction of a schema table */ \
    static void destroySchemaTables(); \
 \
    /* Subclass to add a schema to the table */ \
    template<ui::SelfReflectableModel T> \
    struct addSchemaToTable { \
        /* Constructor adding schema builder function to table */ \
        explicit addSchemaToTable(const word& lookup = baseType::typeName){ \
            constructSchemaTables(); \
            if (!schemasPtr_->insert(lookup, ui::builder<T>::funcName)) { \
                FatalErrorInFunction \
                << "Duplicate entry for " << lookup << " in schema table." \
                << exit(FatalError); \
            } \
        } \
        /* Destructor destorying schema tables */ \
        ~addSchemaToTable() { \
            destroySchemaTables();   \
        } \
        /* Disallow copy construction */ \
        addSchemaToTable(const addSchemaToTable&) = delete; \
        /* Disallow copy assignment */ \
        void operator= (const addSchemaToTable&) = delete; \
    }

#define defineSchemaTable(baseType) \
    baseType::schemaTable* \
    baseType::schemasPtr_(nullptr) 

#define addToSchemaTable(baseType, childType) \
    baseType::addSchemaToTable<childType> \
    add##childType##SchemaToSchemaTable(childType::typeName)

} /// !namespace Foam

#endif
