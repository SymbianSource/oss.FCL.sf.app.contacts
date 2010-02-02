/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Iterates contact field's fieldtype and data parsed from 
*                CParserProperty.
*
*/

#ifndef CVPBKVCARDCONTACTFIELDITERATOR_H
#define CVPBKVCARDCONTACTFIELDITERATOR_H

// INCLUDES
#include <e32base.h>
#include <VPbkFieldType.hrh>

// FORWARD DECLARATIONS
class CVPbkVCardContactFieldData;
class MVPbkContactAttribute;
class MVPbkFieldType;
class CParserProperty;
class CVPbkVCardData;
class TVPbkFieldVersitProperty;

// CLASS DECLARATIONS    
/**
 * Class has fieldtype-data pairs which could be iterated.
 */
NONSHARABLE_CLASS( CVPbkVCardContactFieldIterator ): public CBase
    {
    public: 
        static CVPbkVCardContactFieldIterator* NewLC
            ( CParserProperty& aProperty, CVPbkVCardData& aData );
            
        ~CVPbkVCardContactFieldIterator();
        
    public:
        void AppendVersitProperty( TVPbkFieldVersitProperty aVersitProperty );    
        void AppendAttribute( MVPbkContactAttribute* aAttribute );
        
        TBool HasNext();
        CVPbkVCardContactFieldData* NextLC();
        CVPbkVCardContactFieldData* CurrentLC();
        TInt FindVCardFieldL(
            TVPbkFieldTypeName aVCardFieldType, 
            TVPbkSubFieldType aVCardSubFieldType );
        
    private:
        CVPbkVCardContactFieldIterator( CParserProperty& aProperty, CVPbkVCardData& aData );
        const MVPbkFieldType* MatchFieldType
            ( TVPbkFieldVersitProperty& aProperty );
            
    private:
        /// Ref: property which holds data
        CParserProperty& iProperty;
        /// Own: field types parsed from iProperty
        RPointerArray< const MVPbkFieldType > iFieldTypes;
        /// Own: vcard field types 
        RArray<TVPbkFieldTypeName> iVCardFieldTypes;
        /// Own: vcard sub field types 
        RArray<TVPbkSubFieldType> iVCardSubFieldTypes;
        /// Ref: vcard engine data
        CVPbkVCardData& iData;
        /// Own: cursor pointing current index of iterator
        TInt iCursor;
        /// Ref: contact attribute
        MVPbkContactAttribute* iAttribute;
    };

#endif // CVPBKVCARDCONTACTFIELDITERATOR_H

// End of file
