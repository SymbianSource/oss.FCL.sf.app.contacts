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
* Description:  Parsing one CParserProperty into the TVPbkFieldVersitProperty
*
*/


#ifndef CVPBKVCARDPROPERTYPARSER_H
#define CVPBKVCARDPROPERTYPARSER_H

// INCLUDES
#include <e32base.h>
#include "TVPbkFieldVersitProperty.h"

// FORWARD DECLARATIONS
class CVPbkVCardContactFieldIterator;
class CVPbkVCardData;
class CVPbkVCardAttributeHandler;
class CParserProperty;

// CLASS DECLARATIONS
/**
 * Class is parsing CParserProperty into the TVPbkFieldVersitProperty
 * classes.
 */
NONSHARABLE_CLASS( CVPbkVCardPropertyParser ): public CBase
    {
    public:
        static CVPbkVCardPropertyParser* NewL(
            CVPbkVCardData& aData, CVPbkVCardAttributeHandler& aAttributeHandler);
        
        ~CVPbkVCardPropertyParser();
        
    public:
        CVPbkVCardContactFieldIterator* ParseNextL( CParserProperty* aProperty );
        
    private:
        CVPbkVCardPropertyParser(
            CVPbkVCardData& aData, CVPbkVCardAttributeHandler& aAttributeHandler);
        void ConstructL();
        CVPbkVCardContactFieldIterator* ComposeVersitPropertyL();
        TBool Ignore( CParserProperty* aProperty );
        void ResolveNameL( CParserProperty* aProperty, 
            CVPbkVCardContactFieldIterator& aIterator );
        void ResolveParamsL( CParserProperty* aProperty,
            CVPbkVCardContactFieldIterator& aIterator );
        void ResolveSubfieldsL( CParserProperty* aProperty,
            CVPbkVCardContactFieldIterator& aIterator );
        HBufC8* ConvertToUpperCaseLC( const TDesC8& aOriginalDesC );

    private:
        /// Own: versit property where to parse
        TVPbkFieldVersitProperty iVersitProperty;
        /// Ref: currently active parser property
        CParserProperty* iCurrentProperty;
        /// Ref: vcard engine data
        CVPbkVCardData& iData;
        /// Ref: contact attribute handler
        CVPbkVCardAttributeHandler& iAttributeHandler;
    };
    
#endif  // CVPBKVCARDPROPERTYPARSER_H    
// End of file    
