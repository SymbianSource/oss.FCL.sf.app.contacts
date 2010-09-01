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
* Description:  Converting data according data's vcard uid.
*
*/


#ifndef TVPBKVCARDDATACONVERTER_H
#define TVPBKVCARDDATACONVERTER_H    

// INCLUDES
#include <e32cmn.h> // RPointerArray

// FORWARD DECLARATIONS
class CVPbkVCardContactFieldData;
class TVersitDateTime;
class CParserProperty;
class MVPbkFieldType;
class CBufSeg;
class MVPbkContactAttribute;
                        
// CLASS DECLARATIONS

/**
 * Class creates concrete CVPbkVCardContactFieldData classes according 
 * vcard uids.
 */             
class TVPbkVCardDataConverter
    {
    public:
        TVPbkVCardDataConverter();
    
    public:
        CVPbkVCardContactFieldData* CreateDataLC
            ( CParserProperty& aProperty, 
              RPointerArray<const MVPbkFieldType>& aTypes,
              MVPbkContactAttribute* aAttribute,              
              TInt aCursor );
        const TDesC& GetDesCData( CVPbkVCardContactFieldData& aData );
        const TDesC8& GetBinaryData( CVPbkVCardContactFieldData& aData );
        const TVersitDateTime* GetDateTimeData( CVPbkVCardContactFieldData& aData );
        HBufC8* GetHBufCOfCBufSegL( CBufSeg* aBufSeg ) const;
    };
    
#endif // TVPBKVCARDDATACONVERTER_H

// End of file
