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
*       Provides methods for formatting a field for display.
*
*/


// INCLUDE FILES
#include "CPbkFieldFormatter.h"     // This class
#include "PbkFieldFormatterFactory.h"
#include "MPbkPhoneNumberFormat.h"
#include "TPbkContactItemField.h"
#include "CPbkFieldInfo.h"
#include "CPbkSyncronizationConstants.h"
#include <AknUtils.h>
#include <pbkview.rsg>

// ==================== LOCAL FUNCTIONS ====================

/// Unnamed namespace for this-file-only helper funtions
namespace {

// LOCAL CONSTANTS AND MACROS
enum TPanicCode
    {
	EPanicPostCond_ReallocBufferL = 1
    };

#ifdef _DEBUG
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkFieldFormatter");
    User::Panic(KPanicText, aReason);
    }
#endif

const TInt KBufferLength = 128;

}

// ================= MEMBER FUNCTIONS =======================

inline CPbkFieldFormatter::CPbkFieldFormatter()
	{
	}

inline void CPbkFieldFormatter::ConstructL()
	{
	iBuffer = HBufC::NewL(KBufferLength);
	iNumberFormatter = PbkFieldFormatterFactory::CreatePhoneNumberFormatterL(
			KBufferLength);
	}

CPbkFieldFormatter* CPbkFieldFormatter::NewL()
	{
	CPbkFieldFormatter* self = new(ELeave) CPbkFieldFormatter;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CPbkFieldFormatter::~CPbkFieldFormatter()
	{
	delete iTimeFormat;
	delete iNumberFormatter;
	delete iBuffer;
	}

TPtrC CPbkFieldFormatter::FormatFieldContentL
		(const TPbkContactItemField& aField)
	{
    TPtr text(iBuffer->Des());

    switch (aField.PbkFieldType())
        {
        case KStorageTypeText:
			{
            if (aField.PbkFieldId() == EPbkFieldIdPersonalRingingToneIndication)
                {
                // Ringing tone filename is displayed without the extension
                text.Set(ReallocBufferL(KMaxFileName));
                text.Copy(aField.PbkFieldText());
                TParsePtr fileNameParser(text);
                TPtrC baseName(fileNameParser.Name());
				text.Copy(baseName);
                }
            else if (aField.PbkFieldId() == EPbkFieldIdDTMFString)
                {
                TPtrC dtmfNumber(aField.PbkFieldText());
                HBufC* numberBuffer = HBufC::NewLC(dtmfNumber.Length());
                TPtr number = numberBuffer->Des();
                number.Copy(dtmfNumber);
                AknTextUtils::DisplayTextLanguageSpecificNumberConversion(number);
				text.Set(ReallocBufferL(number.Length()));
				text.Copy(number);
                CleanupStack::PopAndDestroy(numberBuffer);
                }
			else if (aField.FieldInfo().IsPhoneNumberField())
				{
				TPtrC phoneNumber(aField.PbkFieldText());
				iNumberFormatter->SetMaxBufferLengthL(phoneNumber.Length());
				TPtrC formattedPhoneNumber(iNumberFormatter->FormatPhoneNumberForDisplay(phoneNumber));
				text.Set(ReallocBufferL(formattedPhoneNumber.Length()));
				text.Copy(formattedPhoneNumber);
				}
			else if (aField.PbkFieldId() == EPbkFieldIdSyncronization)
				{
                // localize syncronization field content
				TPtrC syncFieldText(aField.PbkFieldText());
                if (!syncFieldText.CompareF(KPbkContactSyncPublic))
                    {
                    HBufC* textBuffer = CCoeEnv::Static()->AllocReadResourceLC(
                            R_QTN_CALE_CONFIDENT_PUBLIC);
                    TPtr localizedSyncText = textBuffer->Des();
                    text.Set(ReallocBufferL(localizedSyncText.Length()));
                    text.Copy(localizedSyncText);
                    CleanupStack::PopAndDestroy(textBuffer);
                    }
                else if (!syncFieldText.CompareF(KPbkContactSyncNoSync))
                    {
                    HBufC* textBuffer = CCoeEnv::Static()->AllocReadResourceLC(
                            R_QTN_CALE_CONFIDENT_NONE);
                    TPtr localizedSyncText = textBuffer->Des();
                    text.Set(ReallocBufferL(localizedSyncText.Length()));
                    text.Copy(localizedSyncText);
                    CleanupStack::PopAndDestroy(textBuffer);
                    }
                else
                    {
                    // otherwise sync setting is the default value - private
                    HBufC* textBuffer = CCoeEnv::Static()->AllocReadResourceLC(
                            R_QTN_CALE_CONFIDENT_PRIVATE);
                    TPtr localizedSyncText = textBuffer->Des();
                    text.Set(ReallocBufferL(localizedSyncText.Length()));
                    text.Copy(localizedSyncText);
                    CleanupStack::PopAndDestroy(textBuffer);
                    }
				}
            else
                {
                TPtrC fieldText(aField.PbkFieldText());
                text.Set(ReallocBufferL(fieldText.Length()));
                text.Copy(fieldText);

                // If field is numeric, language specific conversion is needed.
                if (aField.FieldInfo().EditMode() == EPbkFieldEditModeNumeric )
                    {
                    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(text);
                    }
                }
            break;
			}

        case KStorageTypeDateTime:
            {
			TInt error = KErrNone;
			do
				{
				TRAP(error, aField.PbkFieldTime().FormatL(text, *iTimeFormat));
                AknTextUtils::DisplayTextLanguageSpecificNumberConversion(text);
				if (error == KErrOverflow)
					{
					// allocate bigger buffer for formatting text
					text.Set(ReallocBufferL(2*text.MaxLength()));
					}
				else if (error)
					{
					// rethrow other errors
					User::Leave(error);
					}
				} while (error == KErrOverflow);
            break;
            }

        default:
			{
            text.Zero();
			break;
            }
        }
	return text;
	}

void CPbkFieldFormatter::SetTimeFormatL
		(const TDesC& aTimeFormat)
	{
	HBufC* timeFormat = aTimeFormat.AllocL();
	delete iTimeFormat;
	iTimeFormat = timeFormat;
	}

TPtr CPbkFieldFormatter::ReallocBufferL
		(TInt aNewSize)
	{
	const TInt currMaxLength = iBuffer->Des().MaxLength();
	if (aNewSize > currMaxLength)
		{
		iBuffer = iBuffer->ReAllocL(aNewSize);
		}

	//PostCond:
	__ASSERT_DEBUG(iBuffer->Des().MaxLength() >= aNewSize,
        Panic(EPanicPostCond_ReallocBufferL));

	return iBuffer->Des();
	}

//  End of File  
