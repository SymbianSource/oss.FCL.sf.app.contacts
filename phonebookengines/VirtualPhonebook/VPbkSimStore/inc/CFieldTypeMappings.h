/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A class to map Virtual Phonebook and SIM
*                phonebook field types.
*
*/


#ifndef VPBKSIMSTORE_CFIELDTYPEMAPPINGS_H
#define VPBKSIMSTORE_CFIELDTYPEMAPPINGS_H

//  INCLUDES
#include <e32base.h>
#include <VPbkSimCntFieldTypes.hrh>
#include <VPbkFieldType.hrh>

// FORWARD DECLARATIONS
class TResourceReader;
class MVPbkFieldType;
class MVPbkFieldTypeList;
class TVPbkFieldVersitProperty;

namespace VPbkEngUtils {
class CTextStore;
}

namespace VPbkSimStore {

class TVersitMapData;
class TNonVersitMapData;

// CLASS DECLARATION

/**
 *  A class to map Virtual Phonebook and SIM phonebook field types.
 *
 */
NONSHARABLE_CLASS( CFieldTypeMappings ): public CBase
    {
    public: // Constructors and destructor
        
        /**
         * Creates a new instance of this class.
         *
         * @param aMasterFieldTypeList  Master field types.
         * @param aTextStore a text store for sharing versit property
         *                   extension names
         * @return  A new instance of this class.
         */
        static CFieldTypeMappings* NewL( 
                const MVPbkFieldTypeList& aMasterFieldTypeList,
                VPbkEngUtils::CTextStore& aTextStore );
        
        /**
         * Destructor.
         */
        virtual ~CFieldTypeMappings();

    public: // Interface
        
        /**
         * Initializes the Versit field type mappings.
         *
         * @param aResReader    Resource reader initialized to an array of
         *                      VERSIT_FIELDTYPE_MAPPING resources.
         * @see FieldTypeMapping.rh
         */
        void InitVersitMappingsL(
                TResourceReader& aResReader );

        /**
         * Initializes the non-Versit field type mappings.
         *
         * @param aResReader    Resource reader initialized to an array of
         *                      NONVERSIT_FIELDTYPE_MAPPING resources.
         * @see FieldTypeMapping.rh
         */
        void InitNonVersitMappingsL(
                TResourceReader& aResReader );

        /**
         * Maps the Virtual Phonebook type to a SIM type.
         *
         * @param aVPbkFieldType    Virtual Phonebook field type.
         * @return  SIM field type.
         */
        TVPbkSimCntFieldType Match( 
                const MVPbkFieldType& aVPbkFieldType ) const;

        /**
         * Finds a mapping for native SIM type from the given field type list.
         *
         * @param aFieldTypeList    The field type list to search.
         * @param aSimFieldType     The SIM field type to map.
         * @return  A field type from the given list or NULL if not found.
         */
        const MVPbkFieldType* Match(
                const MVPbkFieldTypeList& aFieldTypeList,
                TVPbkSimCntFieldType aSimFieldType ) const;
        
        /**
         * Finds a mapping for native SIM type from the master field type list.
         *
         * @param aSimFieldType     The SIM field type to map.
         * @return  A field type from the given list or NULL if not found.
         */
        const MVPbkFieldType* Match( 
            TVPbkSimCntFieldType aSimFieldType ) const;

        /**
         * Finds a mapping for native SIM type for the given
         * field Versit property.
         *
         * @param aVersitProperty   The Versit property to match.
         * @return  SIM field type.
         */
        TVPbkSimCntFieldType Match(
                const TVPbkFieldVersitProperty& aVersitProperty ) const;

        /**
         * Finds a mapping for native SIM type for the given
         * field non-Versit type.
         *
         * @param aNonVersitType   The non-Versit type to match.
         * @return  SIM field type.
         */
        TVPbkSimCntFieldType Match( 
                TVPbkNonVersitFieldType aNonVersitType ) const;

    private: // Implementation
        CFieldTypeMappings(
                const MVPbkFieldTypeList& aMasterFieldTypeList,
                VPbkEngUtils::CTextStore& aTextStore );
        
    private: // Data
        /// Ref: Master field types
        const MVPbkFieldTypeList& iMasterFieldTypeList;
        /// Own: Array of Versit field type mapping data
        RArray<TVersitMapData> iVersitMapData;
        /// Own: Array of non-Versit field type mapping data
        RArray<TNonVersitMapData> iNonVersitMapData;
        /// Ref: A text store for versit property extension names
        VPbkEngUtils::CTextStore& iTextStore;
    };

} // namespace VPbkSimStore

#endif      // VPBKSIMSTORE_CFIELDTYPEMAPPINGS_H
            
// End of File
