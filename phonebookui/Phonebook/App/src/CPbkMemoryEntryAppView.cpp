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
*           Provides phonebook memory entry app view methods.
*
*/


// INCLUDE FILES
#include "CPbkMemoryEntryAppView.h"
#include "CPbkAppUi.h"
#include "CPbkCommandFactory.h"
#include "CPbkAppGlobals.h"

#include <CPbkContactEngine.h>
#include <CPbkContactChangeNotifier.h>
#include <CPbkContactItem.h>
#include <CPbkPhoneNumberSelect.h>
#include <CPbkDeleteContactQuery.h>
#include <CPbkViewState.h>
#include <SharedDataClient.h>
#include <CPbkFFSCheck.h>

/// Unnamed namespace for local defintions
namespace {

// LOCAL CONSTANTS

// Sufficient amount of memory for contact database modification
const TInt KFreeSpaceForDbCompress = 128*1024; // 128kb

/// Values for TDeletedContactData::iFlags
enum TDeletedContactFlags
    {
    EFlagContactDeleted = 0x01,
    EFlagContactRemoved = 0x02,
    EFlagContactDeletedAndRemoved = EFlagContactDeleted|EFlagContactRemoved
    };

}  // namespace


// ================= MEMBER FUNCTIONS =======================

// CPbkMemoryEntryAppView::TDeletedContactData
inline void CPbkMemoryEntryAppView::TDeletedContactData::SetDeleted()
    {
    iFlags |= EFlagContactDeleted;
    }

inline void CPbkMemoryEntryAppView::TDeletedContactData::SetRemoved()
    {
    iFlags |= EFlagContactRemoved;
    }

inline TBool CPbkMemoryEntryAppView::TDeletedContactData::IsDeletedAndRemoved() const
    {
    return ((iFlags & EFlagContactDeletedAndRemoved)==EFlagContactDeletedAndRemoved);
    }

inline void CPbkMemoryEntryAppView::TDeletedContactData::SetIndex(TInt aIndex)
    {
    iIndex = aIndex;
    }

inline TInt CPbkMemoryEntryAppView::TDeletedContactData::Index() const
    {
    return iIndex;
    }

void CPbkMemoryEntryAppView::TDeletedContactData::Reset()
    {
    iFlags = 0;
    iIndex = -1;
    }


// CPbkMemoryEntryAppView
CPbkMemoryEntryAppView::~CPbkMemoryEntryAppView()
	{
    if (iAllContactsView)
        {
        iAllContactsView->Close(*this);
        }
    delete iContactChangeNotifier;
    delete iPbkFFSCheck;
    if (iSharedDataClient)
        {
        iSharedDataClient->Close();
        delete iSharedDataClient;
        }    
	}

void CPbkMemoryEntryAppView::CmdBackL()
    {
    CPbkViewState* state = GetViewStateLC();
    PbkAppUi()->ActivatePreviousViewL(state);
    CleanupStack::PopAndDestroy(state);
    }

void CPbkMemoryEntryAppView::CmdDeleteMeL()
    {
    CPbkContactItem* ci = ContactItemL();
    if (DoAskDeleteL(*ci))
        {
        // Compress contact database in low disk space situation.
        // This compress guarantees that contact db size decreases
        // when phone user deletes single contact manually several times.
        // Compression is done before delete because compression may release
        // disk space. This extra space may be needed for successful delete 
        // operation.
        TRAPD(err,FFSCheckL());
        if (KErrDiskFull == err)
            {
            //Db comppression does not cancel delete operation.
            TRAP_IGNORE(DbCompactL());
            err = KErrNone;
            }
        User::LeaveIfError(err);
        TRAP(err, Engine()->DeleteContactL(ci->Id(), ETrue))
        if ( KErrNotFound == err )
            {
            // Ignore KErrNotFound which means that somebody got
            // the contact first
            err = KErrNone;
            }
        User::LeaveIfError(err);
        }
    // Previous view is activated from DB event handlers
    }

TBool CPbkMemoryEntryAppView::DoAskDeleteL(CPbkContactItem& aContact)
    {
    CPbkDeleteContactQuery* dlg = CPbkDeleteContactQuery::NewLC();
    return (dlg->RunLD(aContact) != 0);
    }

void CPbkMemoryEntryAppView::ContactChangedL()
    {
    }

void CPbkMemoryEntryAppView::HandleCommandL
        (TInt aCommandId)
    {
    switch (aCommandId)
        {
		case EAknSoftkeyBack:
            {
            CmdBackL();
            break;
            }

        case EPbkCmdDeleteMe:
            {
            CmdDeleteMeL();
            break;
            }

        default:
            {
            // Forward unknown commands to base class
            CPbkAppView::HandleCommandL(aCommandId);
            break;
            }
        }
    }

void CPbkMemoryEntryAppView::DoActivateL
        (const TVwsViewId& /*aPrevViewId*/,
        TUid /*aCustomMessageId*/,
        const TDesC8& /*aCustomMessage*/)
    {
    if (!iContactChangeNotifier)
        {
        iContactChangeNotifier = CPbkContactChangeNotifier::NewL(*Engine(),this);
        }
    if (!iAllContactsView)
        {
        CContactViewBase* allContactsView = &Engine()->AllContactsView();
        allContactsView->OpenL(*this);
        iAllContactsView = allContactsView;
        }
    }

void CPbkMemoryEntryAppView::DoDeactivate()
    {
    if (iAllContactsView)
        {
        iAllContactsView->Close(*this);
        iAllContactsView = NULL;
        }
    delete iContactChangeNotifier;
    iContactChangeNotifier = NULL;
    }

TBool CPbkMemoryEntryAppView::HandleCommandKeyL
        (const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    if (aType == EEventKey)
        {
        switch (aKeyEvent.iCode)
            {
             case EKeyBackspace:  // Clear key
                {
                HandleCommandL(EPbkCmdDeleteMe);
                return ETrue;
                }
            case EKeyOK:
                {
                HandleCommandL(EPbkCmdContextMenu);
                return ETrue;
                }
            default:
                {
                break;
                }
            }
        }

    // Key was not handled here
    return EFalse;
    }

void CPbkMemoryEntryAppView::HandleDatabaseEventL(TContactDbObserverEvent aEvent)
    {
    if (aEvent.iContactId==ContactItemId())
        {
        switch (aEvent.iType)
            {
            case EContactDbObserverEventContactChanged:
                {
                ContactChangedL();
                break;
                }

            case EContactDbObserverEventContactDeleted:
                {
                iDeletedContactData.SetDeleted();
                HandleContactDeletedL();
                break;
                }

            default:
                {
                break;
                }
            }
        }
    }

void CPbkMemoryEntryAppView::HandleContactViewEvent
        (const CContactViewBase& aView,const TContactViewEvent& aEvent)
    {
    if (&aView==iAllContactsView && aEvent.iContactId==ContactItemId())
        {
        switch (aEvent.iEventType)
            {
            case TContactViewEvent::EItemRemoved:
                {
                iDeletedContactData.SetRemoved();
                iDeletedContactData.SetIndex(aEvent.iInt);
                TRAP_IGNORE(HandleContactDeletedL());
                break;
                }

            default:
                {
                break;
                }
            }
        }
    }

void CPbkMemoryEntryAppView::HandleContactDeletedL()
    {
    // Ensure that delete event has been received by both HandleDatabaseEventL
    // and HandleContactViewEvent. HandleContactViewEvent receives a 
    // EItemRemoved and EItemAdded events when a contact is merely renamed.
    if (iDeletedContactData.IsDeletedAndRemoved())
        {
        // Calculate the contact to focus
        TContactItemId focusId = KNullContactId;
        TInt index = iDeletedContactData.Index();
        iDeletedContactData.Reset();
        if (index >= 0)
            {
            const TInt lastIndex = iAllContactsView->CountL() - 1;
            if (index > lastIndex)
                {
                index = lastIndex;
                }
            if (index >= 0)
                {
                focusId = iAllContactsView->AtL(index);
                }
            }

        // Activate previous view
        if (focusId != KNullContactId)
            {
            CPbkViewState* viewState = CPbkViewState::NewLC();
            viewState->SetFocusedContactId(focusId);
            PbkAppUi()->ActivatePreviousViewL(viewState);
            CleanupStack::PopAndDestroy(viewState);
            }
        else
            {
            PbkAppUi()->ActivatePreviousViewL();
            }
        }
    }

inline void CPbkMemoryEntryAppView::FFSCheckL()
    {
    if (!iPbkFFSCheck)
        {
        iPbkFFSCheck = CPbkFFSCheck::NewL();
        }
    // Leaves with KErrDiskFull if free disk space is below critical level.
    iPbkFFSCheck->FFSClCheckL();
    }

inline void CPbkMemoryEntryAppView::RequestFreeDiskSpaceLC()
    {
    if (!iSharedDataClient)
        {
        iSharedDataClient = new (ELeave) RSharedDataClient();
        User::LeaveIfError(iSharedDataClient->Connect());
        }
    iSharedDataClient->RequestFreeDiskSpaceLC(KFreeSpaceForDbCompress);    
    }

inline void CPbkMemoryEntryAppView::DbCompactL()
    {
    RequestFreeDiskSpaceLC();    
    Engine()->Database().CompactL();
    CleanupStack::PopAndDestroy();// RequestFreeDiskSpaceLC    
    }

//  End of File  
