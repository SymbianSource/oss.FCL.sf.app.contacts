/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 contact editor strategy for editing contact.
*
*/


#include "CPbk2ContactEditorEditContact.h"

// Phonebook 2
#include "CPbk2InputAbsorber.h"
#include <Pbk2UIControls.rsg>
#include <TPbk2ContactEditorParams.h>
#include <CPbk2PresentationContact.h>
#include <CPbk2GeneralConfirmationQuery.h>
#include <MPbk2ContactNameFormatter.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>
#include <csxhelp/phob.hlp.hrh>
#include <Pbk2UID.h>
#include <MPbk2ContactEditorEventObserver.h>
#include <CPbk2ApplicationServices.h>

// Virtual Phonebook
#include <MVPbkContactStore.h>
#include <MVPbkStoreContact.h>
#include <MVPbkFieldType.h>
#include <MVPbkContactFieldData.h>
#include <VPbkEng.rsg>

// System includes
#include <avkon.rsg>
#include <eikon.hrh>


#ifdef _DEBUG

/// Unnamed namespace for local definitions
namespace  {

enum TPanicCode
    {
    ESaveContactL_PreCond,
    EContactOperationCompleted_PreCond,
    EContactOperationFailed_PreCond,
    EDeleteContactL_PreCond
    };

void Panic( TInt aReason )
    {
    _LIT( KPanicText, "CPbk2ContactEditorEditContact");
    User::Panic(KPanicText, aReason);
    }

} /// namespace

#endif // _DEBUG

// --------------------------------------------------------------------------
// CPbk2ContactEditorEditContact::CPbk2ContactEditorEditContact
// --------------------------------------------------------------------------
//
inline CPbk2ContactEditorEditContact::CPbk2ContactEditorEditContact(
        CPbk2PresentationContact* aContact) :
            CActive(CActive::EPriorityStandard),
            iContact(aContact)
    {
    CActiveScheduler::Add(this);
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorEditContact::~CPbk2ContactEditorEditContact
// --------------------------------------------------------------------------
//
CPbk2ContactEditorEditContact::~CPbk2ContactEditorEditContact()
    {
    Cancel();
    delete iIdleDelete;
    delete iInputAbsorber;
    if( iAppServices )
    	{
    	Release( iAppServices );
    	}
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorEditContact::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactEditorEditContact::ConstructL()
    {
    iContact->AddSupportedTemplateFieldsL();
    iInputAbsorber = CPbk2InputAbsorber::NewL(R_AVKON_SOFTKEYS_OPTIONS_DONE);
    
    /*
     * In social phonebook case, this class is used outside from Pbk2 context
     * which causes that Pbk2AppUi is not valid
     */
    iAppServices = CPbk2ApplicationServices::InstanceL();
    iNameFormatter = &iAppServices->NameFormatter();
    
    //Is empty when create edit dialog
    iIsEmpty = IsActualContactEmpty();
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorEditContact::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactEditorEditContact* CPbk2ContactEditorEditContact::NewL
        ( TPbk2ContactEditorParams& /*aParams*/,
          CPbk2PresentationContact* aContact )
    {
    CPbk2ContactEditorEditContact* self =
        new ( ELeave ) CPbk2ContactEditorEditContact( aContact );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorEditContact::RunL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorEditContact::RunL()
    {
    MPbk2ContactEditorEventObserver* observer = iObserver;
    iObserver = NULL;
    observer->ContactEditorOperationCompleted
        ( iLatestResult, iEditorParams );
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorEditContact::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorEditContact::DoCancel()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorEditContact::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2ContactEditorEditContact::RunError( TInt /*aError*/ )
    {
    // No leaving code in RunL
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorEditContact::SaveContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorEditContact::SaveContactL
        ( MPbk2ContactEditorEventObserver& aObserver,
          MPbk2ContactEditorEventObserver::TParams& aParams )
    {
    __ASSERT_DEBUG(!iObserver || iObserver == &aObserver,
        Panic(ESaveContactL_PreCond));

    // If we have already observer then we have also delete or commit
    // operation under construction
    if ( !iObserver )
        {
        iObserver = &aObserver;
        iEditorParams = aParams;
        iContact->CommitL(*this);
        iInputAbsorber->Wait();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorEditContact::DeleteContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorEditContact::DeleteContactL(
    MPbk2ContactEditorEventObserver& aObserver,
    MPbk2ContactEditorEventObserver::TParams& aParams)
    {
    __ASSERT_DEBUG(!iObserver || iObserver == &aObserver,
        Panic(EDeleteContactL_PreCond));

    // If we have already observer then we have also delete or commit
    // operation under construction
    if ( !iObserver )
        {
        iObserver = &aObserver;
        iEditorParams = aParams;
        // If the "end key" was pressed the confirmation query is not shown, the
		// contact is deleted straight away.
		if ( iEditorParams.iFlags & MPbk2ContactEditorEventObserver::EPbk2EditorKeyCode 
            && ( iEditorParams.iKeyCode == EKeyEscape || iEditorParams.iKeyCode == EEikBidCancel ) )
            {
            iContact->DeleteL(*this);
            iInputAbsorber->Wait();
            }
        else
            {
            // Delete the contact if the real contact in store is empty 
            if (iIsEmpty || iEditorParams.iCommandId == EEikBidCancel)
                {
                iContact->DeleteL(*this);
                }
            else
                {
                iQuery = CPbk2GeneralConfirmationQuery::NewL();
                TInt ret = iQuery->ExecuteLD(*iContact, R_QTN_QUERY_COMMON_CONF_DELETE);
                
                // iQuery could be stopped and set null by StopQuery()
                if ( iQuery )
                    {
                    iQuery = NULL;
                    
                    if (ret)
                        {
                        iContact->DeleteL(*this);
                        }
                    else 
                        {
                        CancelDeleteL();
                        }
                    }
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorEditContact::DefaultTitle
// --------------------------------------------------------------------------
//
const TDesC& CPbk2ContactEditorEditContact::DefaultTitle() const
    {
    return iNameFormatter->UnnamedText();
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorEditContact::SwitchContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorEditContact::SwitchContactL
        (CPbk2PresentationContact* aContact)
    {
    iContact = aContact;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorEditContact::GetHelpContext
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorEditContact::GetHelpContext
        ( TCoeHelpContext &aContext ) const
    {
    aContext.iMajor.iUid = KPbk2UID3;
    aContext.iContext = KHLP_CCA_EDITOR;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorEditContact::StopQuery
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorEditContact::StopQuery()
    {
    TBool ret = EFalse;
    if ( iQuery )
        {
        ret = ETrue;
        iQuery->StopQuery();
        iQuery = NULL;
        
        CancelDeleteL();
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorEditContact::ContactOperationCompleted
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorEditContact::ContactOperationCompleted
        ( MVPbkContactObserver::TContactOpResult aResult )
    {
    __ASSERT_DEBUG(iObserver &&
        aResult.iOpCode == MVPbkContactObserver::EContactCommit ||
        aResult.iOpCode == MVPbkContactObserver::EContactDelete,
        Panic(EContactOperationCompleted_PreCond));

    iInputAbsorber->StopWait();
    iLatestResult = aResult;

    // We have to notify success asynchronously, because the
    // all contacts view needs some time to get restructured
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorEditContact::ContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorEditContact::ContactOperationFailed
        ( MVPbkContactObserver::TContactOp aOpCode,
          TInt aErrorCode, TBool aErrorNotified )
    {
    __ASSERT_DEBUG(iObserver, Panic(EContactOperationFailed_PreCond));

    iInputAbsorber->StopWait();
    MPbk2ContactEditorEventObserver* observer = iObserver;
    iObserver = NULL;

    MPbk2ContactEditorEventObserver::TFailParams failParams;
    failParams.iCloseEditor = EFalse;
    failParams.iErrorNotified = aErrorNotified;
    failParams.iErrorCode = KErrNone;

    observer->ContactEditorOperationFailed
        ( aOpCode, aErrorCode, iEditorParams, failParams );
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorEditContact::IdleDeleteCallBack
// --------------------------------------------------------------------------
//
TInt CPbk2ContactEditorEditContact::IdleDeleteCallBack(TAny* aSelf)
    {
    static_cast<CPbk2ContactEditorEditContact*>(aSelf)->HandleIdleDelete();
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorEditContact::HandleIdleDelete
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorEditContact::HandleIdleDelete()
    {
    iInputAbsorber->StopWait();
    MPbk2ContactEditorEventObserver* observer = iObserver;
    iObserver = NULL;
    MVPbkContactObserver::TContactOpResult result;
    // Send commit event, since contact was not deleted. Nor it was
    // commited, but lets emulate commit.
    result.iOpCode = MVPbkContactObserver::EContactCommit;
    result.iStoreContact = NULL;
    result.iExtension = NULL;
    observer->ContactEditorOperationCompleted( result, iEditorParams );
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorEditContact::CancelDeleteL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorEditContact::CancelDeleteL()
    {
    // Notify caller asynchronously about delete cancellation
    if (!iIdleDelete)
        {
        iIdleDelete = CIdle::NewL(CActive::EPriorityStandard);
        }
    else
        {
        iIdleDelete->Cancel();
        }
    iIdleDelete->Start(TCallBack(&IdleDeleteCallBack, this));
    iInputAbsorber->Wait();
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorEditContact::IsActualContactEmpty
// check the actual store contact this contact maps to 
// return true if the real contact in store is empty
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorEditContact::IsActualContactEmpty()
    {
    TInt fieldCount = iContact->Fields().FieldCount();
    const MVPbkStoreContactFieldCollection& fieldSet = iContact->Fields();
    for (TInt i=0; i < fieldCount; ++i)
        {
        const MVPbkStoreContactField& field = fieldSet.FieldAt(i);
        
        //Skip field check for image,ringtone and Syncronization fields
        const MVPbkFieldType* fieldType = field.BestMatchingFieldType();
        TInt fieldTypeId = fieldType->FieldTypeResId();
        if (fieldTypeId != R_VPBK_FIELD_TYPE_RINGTONE && 
            fieldTypeId != R_VPBK_FIELD_TYPE_THUMBNAILPIC && 
            fieldTypeId != R_VPBK_FIELD_TYPE_CALLEROBJIMG && 
            fieldTypeId != R_VPBK_FIELD_TYPE_SYNCCLASS)
            {
            if (!field.FieldData().IsEmpty())
                {
                // There was a field, which was not empty (or all spaces),
                // that's enough for us, the contact gets sent
                return EFalse;
                }
            }
        }
    return ETrue;           
    }
    
// End of File
