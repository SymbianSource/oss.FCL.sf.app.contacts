/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*       Provides methods for analyzing if the contact field has 
*       speed dial and voice tag.
*
*/

#include "CPbkFieldAnalyzer.h"
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <featmgr.h>


// ================= MEMBER FUNCTIONS =======================
CPbkFieldAnalyzer::CPbkFieldAnalyzer(  const CPbkFieldArray& aArray  ) :
    iFieldArray( aArray )
    {

    }

CPbkFieldAnalyzer::~CPbkFieldAnalyzer()
    {
    // Do nothing.
    }


CPbkFieldAnalyzer* CPbkFieldAnalyzer::NewL(
    const CPbkFieldArray& aArray  )
    {
    CPbkFieldAnalyzer* self = 
        new(ELeave) CPbkFieldAnalyzer( aArray );
    
    return self;
    }

TBool CPbkFieldAnalyzer::HasSpeedDialL( const TPbkContactItemField& aField ) const
    {
    TBool result = EFalse;
    
    if ( IsFieldOwned(aField) )
        {
        const CContentType& contentType =  aField.ItemField().ContentType();
        const TInt fieldtypeCount = contentType.FieldTypeCount();
        for (TInt i = 0; i < fieldtypeCount; ++i)
            {
            TFieldType fieldType = contentType.FieldType(i);
            switch (fieldType.iUid)
                {
                case KUidSpeedDialOneValue:			// FALLTHROUGH
                case KUidSpeedDialTwoValue:			// FALLTHROUGH
                case KUidSpeedDialThreeValue:		// FALLTHROUGH
                case KUidSpeedDialFourValue:		// FALLTHROUGH
                case KUidSpeedDialFiveValue:		// FALLTHROUGH
                case KUidSpeedDialSixValue:			// FALLTHROUGH
                case KUidSpeedDialSevenValue:		// FALLTHROUGH
                case KUidSpeedDialEightValue:		// FALLTHROUGH
                case KUidSpeedDialNineValue:
                    {
                    result = ETrue;
                    break;
                    }
                }
            }
        }
    
    return result;
    }

TBool CPbkFieldAnalyzer::HasVoiceTagL( const TPbkContactItemField& aField ) const
    {
    TPbkContactItemField* voiceTagField = NULL;
    
    // Deprecated function
    if ( !FeatureManager::FeatureSupported( KFeatureIdSind ) )
        {
        // scan the fields for a match in voice tag field for field id.
        const TInt fieldCount = iFieldArray.Count();
        for (TInt i = 0; i < fieldCount; ++i)
            {
            const TPbkContactItemField& field = iFieldArray[i];
            if (field.ItemField().ContentType().ContainsFieldType(
                    KUidContactsVoiceDialField))
                {
                voiceTagField = const_cast<TPbkContactItemField*>(&field);
                break;
                }
            }
        }
    
    if ( voiceTagField )
        {
        return ( !voiceTagField->IsEmptyOrAllSpaces() && voiceTagField->IsSame( aField ) );
        }
    else
        {
        return EFalse;
        }
    }

TBool CPbkFieldAnalyzer::IsFieldOwned(  const TPbkContactItemField& aField  ) const
    {
    for ( TInt index = 0; index < iFieldArray.Count(); ++index )
        {
        if( iFieldArray[index].IsSame(aField) )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// end of file.

