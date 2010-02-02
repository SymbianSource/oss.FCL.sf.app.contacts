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
*           Provides phonebook call command object methods.
*
*/


// INCLUDE FILES
#include "CPbkCallCmd.h"

#include <CPbkContactItem.h>
#include <TPbkContactItemField.h>
#include <CPbkFieldInfo.h>
#include <Phonebook.hrh>
#include "CPbkCallTypeSelector.h"
#include <pbkdebug.h>

#include <coemain.h>
#include <AiwServiceHandler.h>      // AIW
#include <AiwCommon.hrh>            // AIW



/// Unnamed namespace for local definitions
namespace {

// LOCAL DEBUG CODE
#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_SetupDialData = 1
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkCallCmd");
    User::Panic(KPanicText,aReason);
    }
#endif

}  // namespace


// ================= MEMBER FUNCTIONS =======================

// Default constructor
CPbkCallCmd::CPbkCallCmd(
	    const CPbkContactItem& aContactItem,
		const TPbkContactItemField* aSelectedField,
		const TPbkContactItemField* aFocusedField,
        const TInt aCommandId,
        CAiwServiceHandler& aServiceHandler,
        const CPbkCallTypeSelector& aSelector) :
			iContact(aContactItem),
            iSelectedField(aSelectedField),
            iFocusedField(aFocusedField),
            iCommandId(aCommandId),
            iServiceHandler(aServiceHandler),
            iSelector(aSelector)
	{
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkCallCmd::CPbkCallCmd(0x%x)"), this);
    }

// Static constructor
CPbkCallCmd* CPbkCallCmd::NewL(
	    const CPbkContactItem& aContactItem,
		const TPbkContactItemField* aSelectedField,
		const TPbkContactItemField* aFocusedField,
        const TInt aCommandId,
        CAiwServiceHandler& aServiceHandler,
        const CPbkCallTypeSelector& aSelector)
    {
    CPbkCallCmd* self = new(ELeave) CPbkCallCmd(
        aContactItem,
        aSelectedField,
        aFocusedField,
        aCommandId,
        aServiceHandler,
        aSelector);
    return self;
    }

// Destructor
CPbkCallCmd::~CPbkCallCmd()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkCallCmd::~CPbkCallCmd(0x%x)"), this);
    }

void CPbkCallCmd::ExecuteLD()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkCallCmd::ExecuteLD(0x%x)"), this);

  	CleanupStack::PushL(this);

    // First setup dial data
    SetupDialDataL();
    TAiwDialDataV1Pckg dialDataPckg(iDialData);
    
    // Set parameters for the AIW call
    CAiwGenericParamList& paramList = iServiceHandler.InParamListL();
    TPtrC8 ptr;
    ptr.Set(dialDataPckg);
    TAiwVariant variant(ptr);

    TAiwGenericParam param(EGenericParamCallDialDataV1, variant);
    paramList.AppendL(param);

    // If the command id is EPbkCmdCall it means the call was launched
    // with send key and we therefore must use different AIW command
    // than when the call was selected from the menu
   if (iCommandId == EPbkCmdCall)
       {
        iServiceHandler.ExecuteServiceCmdL(
            KAiwCmdCall,
            paramList,
            iServiceHandler.OutParamListL(), 
            0,
            NULL );
       }
   else // the call was launched from menu
       {
        // Relay the command to AIW for handling
        iServiceHandler.ExecuteMenuCmdL(
            iCommandId,
            paramList,
            iServiceHandler.OutParamListL(), 
            0,      // No options used.
            NULL);  // No need for callback
       }

	// Destroy itself as promised
    CleanupStack::PopAndDestroy(); // this
    }

/**
 * Sets up aDialData for a call to aContact's aField.
 */
inline void CPbkCallCmd::SetupDialDataL()
    {
    // Set basic dial data
    iDialData.SetWindowGroup(CCoeEnv::Static()->RootWin().Identifier());
    iDialData.SetContactId(iContact.Id());
    iDialData.SetRemoveInvalidChars(ETrue);
    if (iFocusedField)
        {
        iDialData.SetContactItemField(*iFocusedField);
        }
    
    if (iCommandId == EPbkCmdCall)
        {
        __ASSERT_DEBUG(iSelectedField,
            Panic(EPanicPreCond_SetupDialData));

        // If the call is launched with send key, we have to fill certain
        // fields by ourselves
        iDialData.SetNumberType(iSelectedField->FieldInfo().FieldId());
        TAiwTelephoneNumber phoneNumber(iSelectedField->Text().
            Left(KAiwTelephoneNumberLength));
        iDialData.SetTelephoneNumber(phoneNumber);
        
        // We also have select call type by ourselves
        iDialData.SetCallType(TAiwCallType( // the cast is safe
            iSelector.SelectCallType(
                iCommandId,
                iContact,
                iFocusedField,
                *iSelectedField)));
        }
    }
        
void CPbkCallCmd::ProcessFinished(MPbkBackgroundProcess& /*aProcess*/)
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkCallCmd::ProcessFinished(0x%x)"), this);
    }

//  End of File  
