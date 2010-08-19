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
* Description:  Phonebook 2 contact editor custom field.
*
*/


// INCLUDE FILES
#include "CPbk2ContactEditorReadonlyField.h"
#include <AknUtils.h>
#include <AknDef.h>
#include <CPbk2AppUiBase.h>
#include <Pbk2UIControls.hrh>
#include <Pbk2Commands.hrh>
#include <MPbk2ContactEditorField2.h>
#include <MPbk2ContactEditorUIField.h>
#include <MVPbkStoreContactField.h>
#include <MVPbkContactFieldData.h>


/// Unnamed namespace for local definitions
namespace
    {

    enum TContactEditorPanicCodes
        {
        EEditorTouchFeedbackMissing
        };

    void Panic(TContactEditorPanicCodes aReason)
        {
        _LIT( KPanicText, "CPbk2_editor_ReadonlyField" );
        User::Panic(KPanicText, aReason);
        }
    } /// namespace


// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CCPbk2ContactEditorReadonlyField::CCPbk2ContactEditorReadonlyField()
// ---------------------------------------------------------------------------
//
CPbk2ContactEditorReadonlyField::CPbk2ContactEditorReadonlyField()
    {
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorReadonlyField::~CPbk2ContactEditorReadonlyField()
// -----------------------------------------------------------------------------
//
CPbk2ContactEditorReadonlyField::~CPbk2ContactEditorReadonlyField()
    {
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorReadonlyField::InitializeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CPbk2ContactEditorReadonlyField::InitializeL(
        MPbk2ContactEditorUIField* aPbk2ContactEditorField)
    {
    ActivateL();
    iPbk2ContactEditorUIField = aPbk2ContactEditorField;
    iTouchFeedbackRef = MTouchFeedback::Instance();
    __ASSERT_ALWAYS( iTouchFeedbackRef, Panic( EEditorTouchFeedbackMissing ));
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorReadonlyField::InitializeL
// -----------------------------------------------------------------------------
//
void CPbk2ContactEditorReadonlyField::InitializeL(
        MPbk2ContactEditorField* aPbk2ContactEditorField)
    {
    ActivateL();
    iPbk2ContactEditorField = aPbk2ContactEditorField;
    iTouchFeedbackRef = MTouchFeedback::Instance();
    __ASSERT_ALWAYS( iTouchFeedbackRef, Panic( EEditorTouchFeedbackMissing ));
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorReadonlyField::OfferKeyEventL()
// -----------------------------------------------------------------------------
//
TKeyResponse CPbk2ContactEditorReadonlyField::OfferKeyEventL(
        const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    TKeyResponse exitCode = EKeyWasNotConsumed;
    if (aType == EEventKey)
        {
        switch (aKeyEvent.iCode)
            {
            case EKeyOK:
                //no op
                break;
            case EKeyEnter:
                {
                exitCode = EKeyWasConsumed;
                if (iPbk2ContactEditorField)
                    {
                    MPbk2ContactEditorField2 * tempContactEditorField =
                        reinterpret_cast<MPbk2ContactEditorField2*> (
                            iPbk2ContactEditorField->ContactEditorFieldExtension(
                                KMPbk2ContactEditorFieldExtension2Uid));

                    tempContactEditorField->HandleCustomFieldCommandL(
                            EPbk2CmdEditorHandleCustomSelect);
                    }
                else if (iPbk2ContactEditorUIField)
                    {
                    iPbk2ContactEditorUIField->HandleCustomFieldCommandL(
                            EPbk2CmdEditorHandleCustomSelect);
                    }

                }
                break;

            case EKeyDelete:
            case EKeyBackspace:
                {
                if (iPbk2ContactEditorField && !iPbk2ContactEditorField->ContactField().FieldData().IsEmpty() )
                    {
                    MPbk2ContactEditorField2* tempContactEditorField =
                        reinterpret_cast<MPbk2ContactEditorField2*> (iPbk2ContactEditorField->ContactEditorFieldExtension(
                            KMPbk2ContactEditorFieldExtension2Uid));
                    tempContactEditorField-> HandleCustomFieldCommandL(
                            EPbk2CmdEditorHandleCustomRemove);
                    exitCode = EKeyWasConsumed;
                    }

                }
                break;

            default:
                break;
            }
        }
    else if (aType == EEventKeyDown)
        {
        }

    if (exitCode != EKeyWasConsumed)
        {
        exitCode = CEikEdwin::OfferKeyEventL(aKeyEvent, aType);
        }

    return exitCode;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorReadonlyField::SetTextL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPbk2ContactEditorReadonlyField::SetTextL(const TDesC* aData)
    {
    // Shorten data  and take max length of head of text, 
    // otherwise edwin takes tail of text. 
    CTextLayout* layout = const_cast<CTextLayout *> (iTextView->Layout());

    if (!layout->Truncating())
        {
        layout->SetTruncating(TRUE);
        }

    TInt maxLen = iTextLimit;
    HBufC* stripBuf = HBufC::NewLC(maxLen);
    TPtr stripPtr = stripBuf->Des();
    stripPtr.Append(aData->Left(maxLen));
    // Replace some common unicode control characters with spaces
    // as Edwin can't show them correctly. 
    // - normal ASCII line break (\n) 
    // - normal ASCII tab (\t)
    // - paragraph separator (0x2029)
    // - line break (0x2028)
    _LIT(KParagraphCharacter, "\n\t\x2029\x2028");
    TChar space(' ');
    AknTextUtils::ReplaceCharacters(stripPtr, KParagraphCharacter, space);
    CEikEdwin::SetTextL(stripBuf);
    CleanupStack::PopAndDestroy(stripBuf);
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorReadonlyField::HandlePointerEventL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorReadonlyField::HandlePointerEventL(
        const TPointerEvent& aPointerEvent)
    {
    //If overriding HandlePointerEventL(), the implementation must include a
    //base call to CCoeControl's HandlePointerEventL().
    CCoeControl::HandlePointerEventL(aPointerEvent);

    if (AknLayoutUtils::PenEnabled())
        {
        switch (aPointerEvent.iType)
            {
            //When stylus dragged two EButton1Up events received per one EButton1Up
            //event. Prevent EButton1Up processing if stylus dragged out of this 
            //control's rect.
            case TPointerEvent::EButton1Down:
                {
                iTouchFeedbackRef->InstantFeedback(ETouchFeedbackBasic);
                iButton1UpReceived = EFalse;
                break;
                }
            case TPointerEvent::EButton1Up:
                {
                TInt y = aPointerEvent.iPosition.iY;
                if (Rect().iTl.iY <= y && y <= Rect().iBr.iY
                        && !iButton1UpReceived)
                    {
                    iTouchFeedbackRef->InstantFeedback(                    		
                    		this, 
                    		ETouchFeedbackBasic, 
                    		ETouchFeedbackVibra, 
                    		aPointerEvent);
                    
                    if (iPbk2ContactEditorField)
                        {
                        MPbk2ContactEditorField2* tempContactEditorField =
                            reinterpret_cast<MPbk2ContactEditorField2*> (iPbk2ContactEditorField->ContactEditorFieldExtension(
                                KMPbk2ContactEditorFieldExtension2Uid));

                        tempContactEditorField-> HandleCustomFieldCommandL(
                                EPbk2CmdEditorHandleCustomSelect);
                        }
                    else if (iPbk2ContactEditorUIField)
                        {
                        iPbk2ContactEditorUIField-> HandleCustomFieldCommandL(
                                EPbk2CmdEditorHandleCustomSelect);
                        }
                    }
                iButton1UpReceived = ETrue;
                break;
                }
            default:;
            }
        }
    }

//  End of File
