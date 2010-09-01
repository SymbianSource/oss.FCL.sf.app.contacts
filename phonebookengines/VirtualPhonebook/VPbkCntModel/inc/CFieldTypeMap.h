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
* Description:  The virtual phonebook field factory
*
*/



#ifndef CFIELDTYPEMAP_H
#define CFIELDTYPEMAP_H


// INCLUDES
#include <e32base.h>
#include <VPbkFieldType.hrh>
#include <cntdef.h>

// FORWARD DECLARATIONS
class TResourceReader;
class MVPbkFieldTypeList;
class MVPbkFieldType;
class CContactItemField;
class CContentType;
class TVPbkFieldVersitProperty;
namespace VPbkEngUtils
    {
    class CTextStore;
    }

namespace VPbkCntModel {

// CLASS DECLARATIONS

/**
 * A Contact Model field -> Virtual Phonebook field type mapper.
 */
NONSHARABLE_CLASS( CFieldTypeMap ): public CBase
    {
    public:  // Constructor and destructor
        /**
         * Creates a new instance of this class.
         *
         * @param aMasterFieldTypeList Master field type list of vpbk.
         * @return a new instance of this class.
         */
        static CFieldTypeMap* NewL( const MVPbkFieldTypeList& 
                aMasterFieldTypeList, VPbkEngUtils::CTextStore& aTextStore );

        /**
         * Initializes the versit name field type mappings.
         *
         * @param aResReader    resource reader initialized to an array of
         *                      VERSIT_FIELDNAME_MAPPING resources.
         * @see FieldTypeMapping.rh
         */
        void InitVersitNameMappingsL( TResourceReader& aResReader );

        /**
         * Initializes the versit parameter field type mappings.
         *
         * @param aResReader    resource reader initialized to an array of
         *                      VERSIT_FIELDPARAMETER_MAPPING resources.
         * @see FieldTypeMapping.rh
         */
        void InitVersitParameterMappingsL( TResourceReader& aResReader );

        /**
         * Initializes the non-versit field type mappings.
         *
         * @param aResReader    resource reader initialized to an array of
         *                      NONVERSIT_FIELDTYPE_MAPPING resources.
         * @see FieldTypeMapping.rh
         */
        void InitNonVersitMappingsL( TResourceReader& aResReader );

        /**
         * Destructor.
         */
        ~CFieldTypeMap();

    public:  // New functions

        /**
         * Returns a generic field type for a Contact Model field's type.
         * NULL is returned if no field type is found.
         *
         * @param aField            Contact Model field to match.
         * @param aMatchPriority    the matching priority to use.
         * @precond InitVersitMappingsL and InitNonVersitMappingsL have been
         *          called.
         */
        const MVPbkFieldType* GenericFieldType
            ( const CContactItemField& aField,
            TInt aMatchPriority ) const;

        /**
         * Returns a generic field type for a Contact Model field's type.
         * Can also be used with a versit name uid. Should only be used
         * to find contact view sort order elements.
         * NULL is returned if no field type is found.
         *
         * @param aFieldType        Contact Model field type to match.
         * @param aMatchPriority    the matching priority to use.
         * @precond InitVersitMappingsL and InitNonVersitMappingsL have been
         *          called.
         */
        const MVPbkFieldType* FindByCntModelFieldType
            ( TFieldType aFieldType,
            TInt aMatchPriority ) const;

        /**
         * Gets the Virtual Phonebook versit type of a Contact Model field.
         *
         * @param aContentType  Content type to match
         * @return the field's versit type.
         */
        TVPbkFieldVersitProperty VersitType
            ( const CContentType& aContentType ) const;

        /**
         * Gets the Virtual Phonebook non-versit type of a Contact Model field.
         *
         * @param aContentType  Content type to match
         * @return the field's non-versit type.
         */
        TVPbkNonVersitFieldType NonVersitType
            ( const CContentType& aContentType ) const;

        /**
         * Returns the maximum matching priority.
         */
        TInt MaxMatchPriority() const;

    private:  // Constructors
        CFieldTypeMap( const MVPbkFieldTypeList& aMasterFieldTypeList,
                VPbkEngUtils::CTextStore& aTextStore );
        void ConstructL( TResourceReader& aResReader );

    public:  // Implementation
        class TVersitNameMapData;
        class TVersitParameterMapData;
        class TNonVersitMapData;

    private:  // Data        
        ///Ref: Master field type list of vpbk.
        const MVPbkFieldTypeList& iMasterFieldTypeList;
        ///Ref: Master list of property name extensions
        VPbkEngUtils::CTextStore& iTextStore;
        /// Array of versit name field type mapping data
        RArray<TVersitNameMapData> iVersitNameMapData;
        /// Array of versit parameter field type mapping data
        RArray<TVersitParameterMapData> iVersitParameterMapData;
        /// Array of nonversit field type mapping data
        RArray<TNonVersitMapData> iNonVersitMapData;
    };

}  // namespace VPbkCntModel

#endif  // CFIELDTYPEMAP_H
//End of file


