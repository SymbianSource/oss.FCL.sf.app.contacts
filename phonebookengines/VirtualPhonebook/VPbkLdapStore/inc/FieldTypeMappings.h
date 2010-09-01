/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  CFieldTypeMappings - Field type mapping to versit types.
*
*/


#ifndef __FIELDTYPEMAPPINGS_H__
#define __FIELDTYPEMAPPINGS_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class TResourceReader;
class MVPbkFieldType;
class MVPbkFieldTypeList;

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

/**
*  CFieldTypeMappings - ldap field type mapping to versit types.
*  
*/
class CFieldTypeMappings : public CBase
    {
    public:     // CFieldTypeMappings public constructors and destructor
        /**
        @function   NewL
        @discussion Create CFieldTypeMappings object
        @param      aMasterFieldTypeList Master field types
        @return     Pointer to instance of CFieldTypeMappings
        */
        static CFieldTypeMappings* NewL(
                const MVPbkFieldTypeList& aMasterFieldTypeList);

        /**
        @function   NewLC
        @discussion Create CFieldTypeMappings object
        @param      aMasterFieldTypeList Master field types
        @return     Pointer to instance of CFieldTypeMappings
        */
        static CFieldTypeMappings* NewLC(
                const MVPbkFieldTypeList& aMasterFieldTypeList);

        /**
        @function   ~CFieldTypeMappings
        @discussion CFieldTypeMappings destructor
        */
        ~CFieldTypeMappings();

    public:     // CFieldTypeMappings public methods
        /**
        @function   InitVersitMappingsL
        @discussion Initializes the versit field type mappings.
        @param      aReader Resource reader
        */
        void InitVersitMappingsL(TResourceReader& aReader);

        /**
        @function   InitNonVersitMappingsL
        @discussion Initializes the non-versit field type mappings.
        @param      aReader Resource reader
        */
        void InitNonVersitMappingsL(TResourceReader& aReader);

        /**
        @function   FieldTypeCount
        @discussion Returns the number of field types in this list.
        @return     The number of field types
        */
        TInt FieldTypeCount() const;

        /**
        @function   Match
        @discussion Returns the field by type
        @param      aFieldType Field type
        @return     Field type
        */
        const MVPbkFieldType* Match(TInt aFieldType) const;

    private:    // CFieldTypeMappings private constructors
        /**
        @function   CFieldTypeMappings
        @discussion CFieldTypeMappings default contructor
        @param      aMasterFieldTypeList Master field types
        */
        CFieldTypeMappings(
            const MVPbkFieldTypeList& aMasterFieldTypeList);

        /**
        @function   ConstructL
        @discussion Perform the second stage construction of CFieldTypeMappings 
        */
        void ConstructL();

    private:    // CFieldTypeMappings private member variables

        /// Master field types reference
        const MVPbkFieldTypeList& iMasterFieldTypeList;

        // Data types
        class TVersitMapData;
        class TNonVersitMapData;

        /// Array of versit field type mapping data
        RArray<TVersitMapData> iVersitMapData;

        /// Array of nonversit field type mapping data
        RArray<TNonVersitMapData> iNonVersitMapData;
    };
} // End of namespace LDAPStore
#endif // __FIELDTYPEMAPPINGS_H__
