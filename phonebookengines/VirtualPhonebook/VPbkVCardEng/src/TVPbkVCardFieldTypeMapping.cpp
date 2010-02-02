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
* Description:  
*
*/


#include "TVPbkVCardFieldTypeMapping.h"

#include <MVPbkFieldType.h>
#include <VPbkUtil.h>

#include <barsread.h>

void TVPbkVCardFieldTypeMapping::InitializeL
        ( TResourceReader& aResReader,
          VPbkEngUtils::CTextStore& aTextStore )
    {
    iVersitProperty.ReadFromResource( aResReader );
    iNonVersitType = static_cast<TVPbkNonVersitFieldType>( aResReader.ReadUint8() );

    if ( iVersitProperty.ExtensionName().Length() > 0 )
        {
        // Store the extension name to the text store and set the property
        // pointer to the stored version.
        iVersitProperty.SetExtensionName(
            aTextStore.AddL( iVersitProperty.ExtensionName() ) );
        }
    }

const MVPbkFieldType* TVPbkVCardFieldTypeMapping::FindMatch
        ( const MVPbkFieldTypeList& aFieldTypeList ) const
    {
    const MVPbkFieldType* result = NULL;

    if (iVersitProperty.Name() != EVPbkVersitNameNone)
        {
        const TInt maxMatchPriority = aFieldTypeList.MaxMatchPriority();
        for (TInt matchPriority = 0; matchPriority <= maxMatchPriority; ++matchPriority)
            {
            result = aFieldTypeList.FindMatch(iVersitProperty, matchPriority);
            if (result)
                {
                break;
                }
            }
        }

    if (!result)
        {
        if (iNonVersitType != EVPbkNonVersitTypeNone)
            {
            result = aFieldTypeList.FindMatch(iNonVersitType);
            }
        }

    return result;
    }

// End of file
