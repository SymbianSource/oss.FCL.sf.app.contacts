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
*       Represents a connection to the Phonebook contact database
*
*/


// INCLUDE FILES
#include "CPbkContactEngine.h"
#include <cntdb.h>       // CContactDatabase class
#include <cntitem.h>     // ContactItem class
#include <barsc.h>       // RResourceFile
#include <barsread.h>    // TResourceReader
#include <shareddataclient.h>    // RSharedDataClient
#include <featmgr.h>	// Feature manager
#include <ecom/ecom.h>
#include <PbkUID.h>
#include <bautils.h>    // BaflUtils

#include <pbkeng.rsg>    // Engine resources
#include "CPbkFieldsInfo.h"
#include "CPbkFieldInfo.h"
#include "CPbkContactItem.h"
#include "CPbkContactIter.h"
#include "MPbkContactDbObserver.h"
#include "CPbkContactChangeNotifier.h"
#include "CPbkIdleFinder.h"
#include <PbkEngUtils.h>
#include <CPbkConstants.h>
#include "CPbkEngineExtension.h"

#include "CContactDbConnection.h"
#include <MPbkContactNameFormat.h>
#include "CPbkDeleteContacts.h"
#include "CPbkIdleProcessRunner.h"
#include "PbkDataCaging.hrh"
#include "cpbkenginelocalstorage.h"

#include <PbkDebug.h>  // Phonebook debugging support
#include "PbkProfiling.h"

/// Unnamed namespace for local definitons
namespace {

// LOCAL CONSTANTS AND MACROS

_LIT(KCntModelResFileName, "cntmodel.rsc");
_LIT(KCntModelResFileDrive, "z:");
_LIT8(KSettingsVisibleCompareStr, "*1*");

enum TPanicCode
    {
    EPanicContactNotGroup = 1,
    EPanicPreCond_AddObserverL,
    EPanicPostCond_AddObserverL,
    EPanicPreCond_RemoveObserver,
    EPanicPostCond_RemoveObserver,
    EPanicObserverNotFoundInRemove,
    EPanicPreCond_ConstructL,
    EPanicPreCond_UnnamedTitle,
    EPanicPreCond_CreateEmptyContactL,
    EPanicFieldInfoNotFound,
    EPanicLogic_ConstructL,
    EPanicSetTlsFails,
    EPanicSINDHandlerNotFound // obsolete
    };


// ==================== LOCAL FUNCTIONS ====================

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkContactEngine");
    User::Panic(KPanicText, aReason);
    }

/**
 * Returns true if aError is a fatal database open error.
 */
TBool IsFatalDbOpenError(TInt aError);

/**
 * Removes all information that should not be duplicated from aContactItem.
 */
void RemoveNonDuplicableContactInformation(CContactItem& aContactItem);

}  // namespace


// ================= MEMBER FUNCTIONS =======================

inline CPbkContactEngine::CPbkContactEngine() :
    iFreeSpaceRequiredToDelete(8*1024)  // 8 kB by default
    // CBase::operator new(TLeave) will reset other members
	{
	}

EXPORT_C CPbkContactEngine* CPbkContactEngine::NewL(RFs* aFs/*=NULL*/)
	{
	CPbkContactEngine* self = new(ELeave) CPbkContactEngine;
	CleanupStack::PushL(self);
	self->ConstructL(NULL, EFalse, aFs);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CPbkContactEngine* CPbkContactEngine::NewL
        (const TDesC& aFileName, TBool aReplace/*=EFalse*/, RFs* aFs/*=NULL*/)
    {
	CPbkContactEngine* self = new(ELeave) CPbkContactEngine;
	CleanupStack::PushL(self);
	self->ConstructL(&aFileName, aReplace, aFs);
	CleanupStack::Pop(self);
	return self;
    }

EXPORT_C CPbkContactEngine* CPbkContactEngine::ReplaceL
        (RFs* aFs/*=NULL*/)
    {
	CPbkContactEngine* self = new(ELeave) CPbkContactEngine;
	CleanupStack::PushL(self);
	self->ConstructL(NULL, ETrue, aFs);
	CleanupStack::Pop(self);
	return self;
    }

CPbkContactEngine::~CPbkContactEngine()
	{
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkContactEngine(%x)::~CPbkContactEngine"), this);

    // Check if this is the Tls-registered instance and remove it from Tls
    CPbkEngineLocalStorage* storage =
                    reinterpret_cast<CPbkEngineLocalStorage*> (Dll::Tls());
    if ( storage )
        {
        // check that this instance is the same than stored instance
        if ( &storage->iEngine == this )
            {
            // delete the storage instance.
            delete storage;
            Dll::SetTls( NULL );
            }

        }

    if (iSharedDataClient)
        {
        iSharedDataClient->Close();
        delete iSharedDataClient;
        }

	delete iDbConnection;
    delete iObservers;
    delete iPbkFieldsInfo;
    delete iPbkConstants;
	delete iExtension;

	FeatureManager::UnInitializeLib();
    iOwnFs.Close();

    // Cleanup ECom session
    REComSession::FinalClose();
	}

EXPORT_C CPbkContactEngine* CPbkContactEngine::Static()
    {
    CPbkEngineLocalStorage* storage =
        reinterpret_cast<CPbkEngineLocalStorage*> ( Dll::Tls( ) );

    if ( storage )
        {
        return &storage->iEngine;
        }
    else
        {
        return NULL;
        }
    }

inline void CPbkContactEngine::ConnectFsL(RFs* aRfs)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkContactEngine::ConnectFsL(0x%x)"), this);

    if (aRfs)
        {
        iFs = *aRfs;
        }
    else
        {
	    User::LeaveIfError(iOwnFs.Connect());
        iFs = iOwnFs;
        }

    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkContactEngine::ConnectFsL(0x%x) succesful"), this);
    }

inline void CPbkContactEngine::ReadResourcesL(TBool& aSettingsVisibility)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
		("CPbkContactEngine::ReadResourcesL(0x%x)"), this);
    __PBK_PROFILE_START(PbkProfiling::EInitEngineResources);

    // Open the resource files
    RResourceFile pbkResFile;
    PbkEngUtils::FindAndOpenDefaultResourceFileLC(iFs, pbkResFile);
    RResourceFile cntModelResFile;
    PbkEngUtils::FindAndOpenResourceFileLC
        (iFs, KCntModelResFileName, KDC_CONTACTS_RESOURCE_DIR, KCntModelResFileDrive,
        cntModelResFile);

    // Read fields info array
    iPbkFieldsInfo = CPbkFieldsInfo::NewL(pbkResFile, cntModelResFile);

    // load phonebook constants
    iPbkConstants = CPbkConstants::NewL(pbkResFile);

    // read the name ordering settings menu item visibility value:
    HBufC8* buffer = pbkResFile.AllocReadLC(R_PBK_NAME_ORDERING_SETTINGS_VISIBILITY);

    if ( buffer->MatchF(KSettingsVisibleCompareStr) == 0 )
        {
        aSettingsVisibility = ETrue;
        }

    // Close resource files
    CleanupStack::PopAndDestroy(3);

    __PBK_PROFILE_END(PbkProfiling::EInitEngineResources);
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
		("CPbkContactEngine::ReadResourcesL(0x%x) succesful"), this);
    }

inline void CPbkContactEngine::CreateDbConnectionL
        (const TDesC* aFileName,
		TBool aReplace,
		TInt& aDbOpenError,
		TBool aSettingsVisible)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
		("CPbkContactEngine::CreateDbConnectionL(0x%x)"), this);

    // Create contact database connection object
    CContactDbConnection::TParams params;
    params.iDbFileName = aFileName;
    params.iReplaceExistingDb = aReplace;
    params.iContactDbObserver = this;
    params.iFs = iFs;
    params.iFieldsInfo = iPbkFieldsInfo;
    params.iExtension = iExtension;
    params.iDbOpenError = &aDbOpenError;
	params.iUnnamedTitle = &iPbkConstants->UnnamedTitle();
    params.iEngine = this;

    iDbConnection = CContactDbConnection::NewL(params, aSettingsVisible);

    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
		("CPbkContactEngine::CreateDbConnectionL(0x%x) succesful"), this);
    }

void CPbkContactEngine::ConstructL
        (const TDesC* aFileName,
		TBool aReplace, RFs* aRfs)
	{
    __ASSERT_DEBUG(
        !iDbConnection && !iPbkFieldsInfo,
        Panic(EPanicPreCond_ConstructL));

    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
		("CPbkContactEngine::ConstructL(0x%x)"), this);

    ConnectFsL(aRfs);

	FeatureManager::InitializeLibL();

	// Shared data objects have to created after ConnectFsL,
	// but before ReadResourcesL
    iSharedDataClient = new (ELeave) RSharedDataClient;
    User::LeaveIfError(iSharedDataClient->Connect());

    iExtension = CPbkEngineExtension::NewL(iFs);

    TBool settingsVisible(EFalse);

    ReadResourcesL(settingsVisible);
    TInt dbOpenError = KErrNone;
    TRAPD(dbConnectError, CreateDbConnectionL(aFileName, aReplace, dbOpenError, settingsVisible));
    if (IsFatalDbOpenError(dbOpenError))
        {
        __ASSERT_DEBUG(dbOpenError == dbConnectError, Panic(EPanicLogic_ConstructL));
        // Map all fatal database open errors to KErrCorrupt
        User::Leave(KErrCorrupt);
        }
    if (dbConnectError != KErrNone)
        {
        if (dbConnectError == KErrCorrupt)
            {
            // Convert KErrCorrupt to another error code to avoid clients
            // mixing this error up with database corruption
            dbConnectError = KErrGeneral;
            }
        User::Leave(dbConnectError);
        }

    if (!Dll::Tls())
        {
        CPbkEngineLocalStorage* storage = CPbkEngineLocalStorage::NewL(*this);
        TInt ret = Dll::SetTls(storage);  // Takes ownership
        __ASSERT_DEBUG( ret == KErrNone, Panic( EPanicSetTlsFails ) );
        }

    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
		("CPbkContactEngine::ConstructL(0x%x) succesful"), this);
	}

EXPORT_C CPbkContactItem* CPbkContactEngine::CreateEmptyContactL()
    {
    // Create an empty (ie. no fields) contact card
    CContactCard* card = CContactCard::NewLC();
    __ASSERT_DEBUG(card->CardFields().Count()==0,
        Panic(EPanicPreCond_CreateEmptyContactL));

    // Loop through Phonebook field infos
    const TInt count = FieldsInfo().Count();
    for (TInt i=0; i < count; ++i)
        {
        const CPbkFieldInfo* fi = FieldsInfo()[i];
        if (fi->TemplateField())
            {
            // Create a CContactItemField for fields belonging
			// to the default template
            CContactItemField* field = fi->CreateFieldL();
            CleanupStack::PushL(field);
            // Add the field to the contact card
            card->AddFieldL(*field);
            CleanupStack::Pop(field);
            }
        }

    // Create phonebook contact item from card and return it.
    CPbkContactItem* pbkItem = CPbkContactItem::NewL(card,
		FieldsInfo(), ContactNameFormat());
    CleanupStack::Pop(); // card
    return pbkItem;
    }

EXPORT_C const CPbkFieldsInfo& CPbkContactEngine::FieldsInfo()
    {
    return *iPbkFieldsInfo;
    }

EXPORT_C RFs& CPbkContactEngine::FsSession() const
    {
    return const_cast<RFs&>(iFs);
    }

EXPORT_C CPbkContactIter* CPbkContactEngine::CreateContactIteratorLC
        (TBool aUseMinimalRead/*=EFalse*/)
    {
    iDbConnection->CancelCompress();
    return CPbkContactIter::NewLC(*this, aUseMinimalRead);
    }

EXPORT_C CPbkContactChangeNotifier*
    CPbkContactEngine::CreateContactChangeNotifierL
        (MPbkContactDbObserver *aObserver)
    {
    return CPbkContactChangeNotifier::NewL(*this, aObserver);
    }

EXPORT_C const TDesC& CPbkContactEngine::UnnamedTitle() const
    {
    __ASSERT_DEBUG(iPbkConstants, Panic(EPanicPreCond_UnnamedTitle));
    return iPbkConstants->UnnamedTitle();
    }

EXPORT_C TContactItemId CPbkContactEngine::AddNewContactL
        (CPbkContactItem& aContact, TBool aImmediateNotify/*=EFalse*/)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
	("CPbkContactEngine(%x)::AddNewContactL(aContact=%x, aImmediateNotify=%d)"),
        this, &aContact, aImmediateNotify);

    iDbConnection->CancelCompress();
    // Prepare contact for saving to the DB
    aContact.PrepareForSaveL();
    TContactItemId cid = iDbConnection->Database().AddNewContactL
		(aContact.ContactItem());
    // Undo PrepareForSaveL
    aContact.PrepareAfterLoadL();

    // Send immediate event
    SendImmidiateEventToAllObservers(EContactDbObserverEventContactAdded,
        cid, aImmediateNotify);

    return cid;
    }

EXPORT_C CPbkContactItem* CPbkContactEngine::ReadContactL
        (TContactItemId aContactId,
        const CPbkFieldIdArray* aFieldTypes/*=NULL*/)
    {
    CPbkContactItem* item = ReadContactLC(aContactId, aFieldTypes);
    CleanupStack::Pop();  // item
    return item;
    }

EXPORT_C CPbkContactItem* CPbkContactEngine::ReadContactLC
        (TContactItemId aContactId,
        const CPbkFieldIdArray* aFieldTypes/*=NULL*/)
    {
    iDbConnection->CancelCompress();
    CContactItem* item = NULL;
    if (aFieldTypes)
        {
        CContactItemViewDef* viewDef =
            FieldsInfo().CreateContactItemViewDefLC(*aFieldTypes);
        item = iDbConnection->Database().ReadContactL(aContactId, *viewDef);
        CleanupStack::PopAndDestroy(viewDef);
        CleanupStack::PushL(item);
        }
    else
        {
        item = iDbConnection->Database().ReadContactLC(aContactId);
        }
    CPbkContactItem* pbkItem = CPbkContactItem::NewL
        (item, FieldsInfo(), ContactNameFormat());
    // CPbkContactItem takes ownership of item
    CleanupStack::Pop(); // item
    CleanupStack::PushL(pbkItem);

    // Prep empty fields for use in the application.
    // See also CommitContactL.
    pbkItem->PrepareAfterLoadL();

    iDbConnection->CompressL();
    return pbkItem;
    }

EXPORT_C CPbkContactItem* CPbkContactEngine::ReadMinimalContactLC
        (TContactItemId aContactId)
    {
    iDbConnection->CancelCompress();
    CContactItem* item =
        iDbConnection->Database().ReadMinimalContactLC(aContactId);
    CPbkContactItem* pbkItem = CPbkContactItem::NewL
        (item, FieldsInfo(), ContactNameFormat());
    // CPbkContactItem takes ownership of item
    CleanupStack::Pop(); // item
    CleanupStack::PushL(pbkItem);

    // Prep empty fields for use in the application.
    // See also CommitContactL.
    pbkItem->PrepareAfterLoadL();

    iDbConnection->CompressL();
    return pbkItem;
    }

EXPORT_C CPbkContactItem* CPbkContactEngine::OpenContactL
        (TContactItemId aContactId)
    {
    PBK_DEBUG_PRINT(
        PBK_DEBUG_STRING("CPbkContactEngine(%x)::OpenContactL(aContactId=%d)"),
        this, aContactId);

    CPbkContactItem* pbkItem = OpenContactLCX(aContactId);
    CleanupStack::Pop(2);  // pbkItem, lock
    return pbkItem;
    }

EXPORT_C CPbkContactItem* CPbkContactEngine::OpenContactLCX
        (TContactItemId aContactId)
    {
    PBK_DEBUG_PRINT(
        PBK_DEBUG_STRING("CPbkContactEngine(%x)::OpenContactLCX(aContactId=%d)"),
        this, aContactId);

    iDbConnection->CancelCompress();
    CContactItem* item = iDbConnection->Database().OpenContactLX(aContactId);
    CleanupStack::PushL(item);
    CPbkContactItem* pbkItem =
        CPbkContactItem::NewL(item, FieldsInfo(), ContactNameFormat());
    // CPbkContactItem takes ownership of item
    CleanupStack::Pop(item);
    CleanupStack::PushL(pbkItem);

    // Prep empty fields for use in the application.
    // See also CommitContactL.
    pbkItem->PrepareAfterLoadL();

    return pbkItem;
    }

EXPORT_C void CPbkContactEngine::CommitContactL
        (CPbkContactItem& aContact, TBool aImmediateNotify/*=EFalse*/)
    {
    PBK_DEBUG_PRINT(
        PBK_DEBUG_STRING(
        "CPbkContactEngine(%x)::CommitContactL(aContact=%x, aImmediateNotify=%d)"),
        this, &aContact, aImmediateNotify);

    iDbConnection->CancelCompress();
    // Prepare contact for saving to the DB
    aContact.PrepareForSaveL();
    iDbConnection->Database().CommitContactL(aContact.ContactItem());
    // Undo PrepareForSaveL
    aContact.PrepareAfterLoadL();

    // Send immediate event
    SendImmidiateEventToAllObservers(EContactDbObserverEventContactChanged,
        aContact.ContactItem().Id(), aImmediateNotify);
    }

EXPORT_C void CPbkContactEngine::CloseContactL
        (TContactItemId aContactId)
    {
    PBK_DEBUG_PRINT(
        PBK_DEBUG_STRING("CPbkContactEngine(%x)::CloseContactL(aContactId=%d)"),
        this, aContactId);

    iDbConnection->CancelCompress();
    iDbConnection->Database().CloseContactL(aContactId);
    }

EXPORT_C void CPbkContactEngine::DeleteContactL
        (TContactItemId aContactId, TBool aImmediateNotify/*=EFalse*/)
    {
    PBK_DEBUG_PRINT(
        PBK_DEBUG_STRING(
        "ENTER: CPbkContactEngine(%x)::DeleteContactL(aContactId=%d, aImmediateNotify=%d)"),
        this, aContactId, aImmediateNotify);

    doDeleteContactL(aContactId);

    // Send immediate event
    SendImmidiateEventToAllObservers(EContactDbObserverEventContactDeleted,
        aContactId, aImmediateNotify);
    }

EXPORT_C void CPbkContactEngine::DeleteContactGroupL
        (TContactItemId aContactId, TBool aImmediateNotify/*=EFalse*/)
    {
    PBK_DEBUG_PRINT(
        PBK_DEBUG_STRING(
        "ENTER: CPbkContactEngine(%x)::DeleteContactL(aContactId=%d, aImmediateNotify=%d)"),
        this, aContactId, aImmediateNotify);

    doDeleteContactL(aContactId);

    // Send immediate event
    SendImmidiateEventToAllObservers(EContactDbObserverEventGroupDeleted,
        aContactId, aImmediateNotify);
    }


EXPORT_C void CPbkContactEngine::DeleteContactsL
        (const CContactIdArray& aContactIds, TBool aImmediateNotify/*=EFalse*/)
    {
    PBK_DEBUG_PRINT(
        PBK_DEBUG_STRING(
        "ENTER: CPbkContactEngine(%x)::DeleteContactsL(count=%d, aImmediateNotify=%d)"),
        this, aContactIds.Count(), aImmediateNotify);

    iDbConnection->CancelCompress();
    iDbConnection->Database().DeleteContactsL(aContactIds);

    SendImmidiateEventToAllObservers(EContactDbObserverEventUnknownChanges,
        KNullContactId, aImmediateNotify);
    }

EXPORT_C void CPbkContactEngine::DeleteContactsOnBackgroundL
        (const CContactIdArray& aContactIds)
    {
    iDbConnection->CancelCompress();
    // Shared data client is connected during construction
    CPbkDeleteContacts* process = CPbkDeleteContacts::NewLC(*this,aContactIds, *iSharedDataClient);
    CPbkIdleProcessRunner* runner =
        CPbkIdleProcessRunner::NewL(CActive::EPriorityIdle);
    CleanupStack::PushL(runner);
    runner->SynchronousExecuteL(*process);

    if (process->DeletedCount() > 0)
        {
        SendImmidiateEventToAllObservers(EContactDbObserverEventUnknownChanges,
            KNullContactId, ETrue);
        }
    CleanupStack::PopAndDestroy(2);  // runner, process
    }

EXPORT_C CContactGroup* CPbkContactEngine::CreateContactGroupL
        (const TDesC& aGroupLabel,
        TBool aInTransaction)
    {
    iDbConnection->CancelCompress();
    CContactGroup* group =
        static_cast<CContactGroup*>
        (iDbConnection->Database().CreateContactGroupL(aGroupLabel, aInTransaction));

    // Send immediate event
    SendImmidiateEventToAllObservers(EContactDbObserverEventGroupAdded,
        group->Id(), ETrue);

    return group;
    }

EXPORT_C void CPbkContactEngine::AddContactToGroupL
        (TContactItemId aItemId,
        TContactItemId aGroupId)
    {
    iDbConnection->CancelCompress();
    iDbConnection->Database().AddContactToGroupL(aItemId, aGroupId);
    }


EXPORT_C void CPbkContactEngine::RemoveContactFromGroupL
        (TContactItemId aItemId,
        TContactItemId aGroupId)
    {
    iDbConnection->CancelCompress();
    iDbConnection->Database().RemoveContactFromGroupL(aItemId, aGroupId);
    }

EXPORT_C CContactGroup* CPbkContactEngine::ReadContactGroupL
        (TContactItemId aId)
    {
    iDbConnection->CancelCompress();
    CContactItem* item = iDbConnection->Database().ReadContactL(aId);
    __ASSERT_ALWAYS(item->Type() == KUidContactGroup,
        Panic(EPanicContactNotGroup));
    return static_cast<CContactGroup*>(item);
    }

EXPORT_C CContactGroup* CPbkContactEngine::OpenContactGroupL
        (TContactItemId aId)
    {
    iDbConnection->CancelCompress();
    CContactItem* item = iDbConnection->Database().OpenContactL(aId);
    __ASSERT_ALWAYS(item->Type() == KUidContactGroup,
        Panic(EPanicContactNotGroup));
    return static_cast<CContactGroup*>(item);
    }

EXPORT_C CContactGroup* CPbkContactEngine::OpenContactGroupLCX
        (TContactItemId aId)
    {
    iDbConnection->CancelCompress();
    CContactItem* item = iDbConnection->Database().OpenContactLX(aId);
    __ASSERT_ALWAYS(item->Type() == KUidContactGroup,
        Panic(EPanicContactNotGroup));
    CContactGroup* group = static_cast<CContactGroup*>(item);
    CleanupStack::PushL(group);
    return group;
    }

EXPORT_C void CPbkContactEngine::CommitContactGroupL
        (CContactGroup& aGroup,
        TBool aImmediateNotify/*=EFalse*/)
    {
    iDbConnection->CancelCompress();
    iDbConnection->Database().CommitContactL(aGroup);

    SendImmidiateEventToAllObservers(EContactDbObserverEventGroupChanged,
        aGroup.Id(), aImmediateNotify);
    }

EXPORT_C TContactItemId CPbkContactEngine::DuplicateContactL(TContactItemId aId,
        TBool aImmediateNotify/*=EFalse*/)
    {
    PBK_DEBUG_PRINT(
        PBK_DEBUG_STRING(
        "CPbkContactEngine(%x)::DuplicateContactL(aId=%d, aImmediateNotify=%d)"),
        this, aId, aImmediateNotify);

    iDbConnection->CancelCompress();
    CContactItem* item = iDbConnection->Database().ReadContactLC(
        aId, *iDbConnection->Database().AllFieldsView());
    // remove all non duplicable contact information
    RemoveNonDuplicableContactInformation(*item);

    TContactItemId dupId = iDbConnection->Database().AddNewContactL(*item);
    CleanupStack::PopAndDestroy();  // item

    // Send immediate event
    SendImmidiateEventToAllObservers(EContactDbObserverEventContactAdded,
        dupId, aImmediateNotify);
    return dupId;
    }

EXPORT_C void CPbkContactEngine::SetFieldAsSpeedDialL
        (CPbkContactItem& aItem,
        TInt aFieldIndex,
        TInt aSpeedDialPosition)
    {
    iDbConnection->CancelCompress();
    // find correct CContactItemField index
    CContactItemField& field = aItem.CardFields()[aFieldIndex].ItemField();
    for (TInt i = 0; i < aItem.ContactItem().CardFields().Count(); ++i)
        {
        if (&field == &(aItem.ContactItem().CardFields()[i]))
            {
            aFieldIndex = i;
            break;
            }
        }

    // Prepare contact for saving to the DB
    aItem.PrepareForSaveL();

    iDbConnection->Database().SetFieldAsSpeedDialL(
        aItem.ContactItem(), aFieldIndex, aSpeedDialPosition);

    // Undo PrepareForSaveL
    aItem.PrepareAfterLoadL();

    // update the aItem's field set
    aItem.UpdateFieldSetL(*iPbkFieldsInfo);

    // CContactDatabase::SetFieldAsSpeedDialL does not send notifications
    // Send immediate event
    SendImmidiateEventToAllObservers(EContactDbObserverEventContactChanged,
        aItem.Id(), ETrue);
    }

EXPORT_C CContactDatabase& CPbkContactEngine::Database()
    {
    return iDbConnection->Database();
    }

EXPORT_C CContactViewBase& CPbkContactEngine::AllContactsView()
    {
    return iDbConnection->AllContactsView();
    }

EXPORT_C CContactViewBase& CPbkContactEngine::AllGroupsViewL()
    {
    return iDbConnection->AllGroupsViewL();
    }

EXPORT_C CContactViewBase& CPbkContactEngine::FilteredContactsViewL
        (TInt aFilter)
    {
    return iDbConnection->FilteredContactsViewL(aFilter);
    }

EXPORT_C HBufC* CPbkContactEngine::GetContactTitleL
        (const CPbkContactItem& aItem) const
    {
    // Delegate call to iContactNameFormatter
    return ContactNameFormat().GetContactTitleL(aItem);
    }

EXPORT_C void CPbkContactEngine::SetCompressUi(MPbkCompressUi* aCompressUi)
    {
    iDbConnection->SetCompressUi(aCompressUi);
    }

EXPORT_C TBool CPbkContactEngine::CheckCompress()
    {
    return iDbConnection->CheckCompress();
    }

EXPORT_C void CPbkContactEngine::CompressL()
    {
    iDbConnection->CompressL();
    }

EXPORT_C void CPbkContactEngine::CancelCompress()
    {
    iDbConnection->CancelCompress();
    }

EXPORT_C void CPbkContactEngine::CheckFileSystemSpaceAndCompressL()
    {
    iDbConnection->CheckFileSystemSpaceAndCompressL();
    }

EXPORT_C HBufC* CPbkContactEngine::GetContactTitleOrNullL
        (const MPbkFieldDataArray& aContactData)
    {
    // Delegate call to iContactNameFormatter
    return ContactNameFormat().GetContactTitleOrNullL(aContactData);
    }

EXPORT_C TBool CPbkContactEngine::IsTitleField(TPbkFieldId aFieldId) const
    {
    // Delegate call to iContactNameFormatter
    return ContactNameFormat().IsTitleField(aFieldId);
    }

EXPORT_C MPbkContactNameFormat& CPbkContactEngine::ContactNameFormat() const
    {
    return iDbConnection->ContactNameFormatter();
    }

EXPORT_C TContactItemId CPbkContactEngine::GetSpeedDialFieldL
        (TInt aSpeedDialPosition,
        TDes& aPhoneNumber) const
    {
    iDbConnection->CancelCompress();
    return iDbConnection->Database().GetSpeedDialFieldL
        (aSpeedDialPosition,aPhoneNumber);
    }

EXPORT_C void CPbkContactEngine::RemoveSpeedDialFieldL
        (TContactItemId aContactId,
        TInt aSpeedDialPosition)
    {
    iDbConnection->CancelCompress();
    iDbConnection->Database().RemoveSpeedDialFieldL(
        aContactId, aSpeedDialPosition);
    }

EXPORT_C TBool CPbkContactEngine::IsSpeedDialAssigned
        (const CPbkContactItem& aItem, TInt aFieldIndex) const
    {
    const CContentType& contentType =
        aItem.CardFields()[aFieldIndex].ItemField().ContentType();
    TBool result = EFalse;

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
    return result;
    }

EXPORT_C CContactIdArray* CPbkContactEngine::MatchPhoneNumberL
        (const TDesC& aNumber, const TInt aMatchLengthFromRight)
    {
    iDbConnection->CancelCompress();
    return iDbConnection->Database().MatchPhoneNumberL
        (aNumber,aMatchLengthFromRight);
    }

EXPORT_C CContactIdArray* CPbkContactEngine::FindLC
        (const TDesC& aText,
        const CPbkFieldIdArray* aFieldTypes/*=NULL*/)
    {
    iDbConnection->CancelCompress();
    CContactItemFieldDef* fieldDef =
        FieldsInfo().CreateContactItemFieldDefLC(aFieldTypes);

    // Call contact db's FindLC
    CContactIdArray* result = iDbConnection->Database().FindLC(aText, fieldDef);
    CleanupStack::Pop();  // result

    // Cleanup and return
    CleanupStack::PopAndDestroy(fieldDef);
    CleanupStack::PushL(result);
    return result;
    }

EXPORT_C CPbkIdleFinder* CPbkContactEngine::FindAsyncL
        (const TDesC& aText,
        const CPbkFieldIdArray* aFieldTypes/*=NULL*/,
        MIdleFindObserver* aObserver/*=NULL*/)
    {
    iDbConnection->CancelCompress();
    CContactItemFieldDef* fieldDef =
        FieldsInfo().CreateContactItemFieldDefLC(aFieldTypes);

    // Call contact db's FindAsyncL
    CIdleFinder* finder =
        iDbConnection->Database().FindAsyncL(aText, fieldDef, aObserver);
    CleanupStack::PushL(finder);

    // Pack result and fieldDef array into a CPbkIdleFinder
    CPbkIdleFinder* pbkFinder = new(ELeave) CPbkIdleFinder(finder, fieldDef);

    // Cleanup and return
    CleanupStack::Pop(2);  // finder, fieldDef
    return pbkFinder;
    }

/**
 * Distributes CContactDatabase observer events in Phonebook.
 */
void CPbkContactEngine::HandleDatabaseEventL(TContactDbObserverEvent aEvent)
    {
    PBK_DEBUG_PRINT(
        PBK_DEBUG_STRING(
        "CPbkContactEngine(%x)::HandleDatabaseEventL(), type=%d, id=%d, conn=%d"),
        this, aEvent.iType, aEvent.iContactId, aEvent.iConnectionId);

    // Forward the event to all observers of this engine object
    SendEventToAllObservers(aEvent);
    }

void CPbkContactEngine::SendEventToAllObservers
        (const TContactDbObserverEvent& aEvent)
    {
    if (iObservers)
        {
        TInt i;

        // First forward the standard event to all observers
        for (i = iObservers->Count()-1; i >= 0; --i)
            {
            #ifndef _DEBUG
            TRAP_IGNORE((*iObservers)[i]->HandleDatabaseEventL(aEvent));
            #else
            TRAPD(ignore, (*iObservers)[i]->HandleDatabaseEventL(aEvent));
            if (ignore != KErrNone)
                {
                RDebug::Print(
                    _L("Leave %d occured in MPbkContactDbObserver(%x)::HandleDatabaseEventL()"),
                    ignore, (*iObservers)[i]);
                }
            #endif
            }

        // Next tell all observers that all observers have had their
        // HandleDatabaseEventL function called.
        for (i = iObservers->Count()-1; i >= 0; --i)
            {
            #ifndef _DEBUG
            TRAP_IGNORE((*iObservers)[i]->DatabaseEventHandledL(aEvent));
            #else
            TRAPD(ignore, (*iObservers)[i]->DatabaseEventHandledL(aEvent));
            if (ignore != KErrNone)
                {
                RDebug::Print(
                    _L("Leave %d occured in MPbkContactDbObserver(%x)::DatabaseEventHandledL()"),
                    ignore, (*iObservers)[i]);
                }
            #endif
            }
        }

    PBK_DEBUG_PRINT(
        PBK_DEBUG_STRING(
        "CPbkContactEngine(%x)::SendEventToAllObservers() executed. type=%d, id=%d, conn=%d"),
        this, aEvent.iType, aEvent.iContactId, aEvent.iConnectionId);
    }

void CPbkContactEngine::SendImmidiateEventToAllObservers
        (TContactDbObserverEventType aEventType,
        TContactItemId aContactId,
        TBool aSendEvent)
    {
    if (aSendEvent)
        {
        TContactDbObserverEvent event;
        event.iType = aEventType;
        event.iContactId = aContactId;
        event.iConnectionId = iDbConnection->Database().ConnectionId();
        SendEventToAllObservers(event);
        }
    }

void CPbkContactEngine::AddObserverL(MPbkContactDbObserver* aObserver)
    {
    // PreCond
    __ASSERT_DEBUG(aObserver, Panic(EPanicPreCond_AddObserverL));

    if (!iObservers)
        {
        iObservers = new(ELeave) CArrayPtrFlat<MPbkContactDbObserver>(4);
        }

    PBK_DEBUG_ONLY(const TInt old_Count=iObservers->Count());

    iObservers->AppendL(aObserver);

    PBK_DEBUG_PRINT(
        PBK_DEBUG_STRING("CPbkContactEngine(%x)::AddObserverL(%x), count = %d"),
        this, aObserver, iObservers->Count());

    // PostCond
    __ASSERT_DEBUG(iObservers->Count()==old_Count+1 &&
                   (*iObservers)[iObservers->Count()-1]==aObserver,
                   Panic(EPanicPostCond_AddObserverL));
    }

void CPbkContactEngine::RemoveObserver(MPbkContactDbObserver* aObserver)
    {
    // PreCond
    __ASSERT_DEBUG(iObservers && aObserver,
        Panic(EPanicPreCond_RemoveObserver));

    const TInt count = iObservers->Count();
    for (TInt i = 0; i < count; ++i)
        {
        if ((*iObservers)[i] == aObserver)
            {
            PBK_DEBUG_ONLY(const TInt old_Count=iObservers->Count());

            iObservers->Delete(i);


            PBK_DEBUG_PRINT(
                PBK_DEBUG_STRING(
                "CPbkContactEngine(%x)::RemoveObserver(%x), count = %d"),
                this, aObserver, iObservers->Count());

            // PostCond
            __ASSERT_DEBUG(iObservers->Count()==old_Count-1,
                   Panic(EPanicPostCond_RemoveObserver));
            return;
            }
        }

    // Illegal state
    __ASSERT_DEBUG(EFalse, Panic(EPanicObserverNotFoundInRemove));
    }

void CPbkContactEngine::doDeleteContactL(TContactItemId aContactId)
    {
    iDbConnection->CancelCompress();
    CContactDatabase& db = iDbConnection->Database();
    const TInt sizeBefore = db.FileSize();

    iSharedDataClient->RequestFreeDiskSpaceLC(iFreeSpaceRequiredToDelete);
    db.DeleteContactL(aContactId);
    CleanupStack::PopAndDestroy();  // RequestFreeDiskSpaceLC

    // Calculate how much database grew rounded to next kB
    const TInt KB = 1024;
    TInt sizeDiff = db.FileSize() - sizeBefore + KB;
    sizeDiff -= sizeDiff % KB;
    if (sizeDiff > iFreeSpaceRequiredToDelete)
        {
        // Update maximum size required
        iFreeSpaceRequiredToDelete = sizeDiff;
        }
    }

EXPORT_C void CPbkContactEngine::SetNameDisplayOrderL
		(TPbkNameOrder aNameOrder)
	{
	// change sort order
	iDbConnection->SetNameDisplayOrderL(aNameOrder);
	}

EXPORT_C CPbkContactEngine::TPbkNameOrder
        CPbkContactEngine::NameDisplayOrderL()
	{
	// retrieve sort order from sort order manager
	return iDbConnection->NameDisplayOrder();
	}

EXPORT_C CPbkConstants* CPbkContactEngine::Constants()
    {
    return iPbkConstants;
    }

const CPbkSortOrderManager& CPbkContactEngine::SortOrderManager() const
    {
    return iDbConnection->SortOrderManager();
    }

EXPORT_C void CPbkContactEngine::SetNameSeparatorL(TChar aSeparator)
    {
    iDbConnection->SetNameSeparatorL(aSeparator);
    }

EXPORT_C TChar CPbkContactEngine::NameSeparator() const
    {
    return iDbConnection->NameSeparator();
    }

// ==================== LOCAL FUNCTIONS ====================

namespace {

TBool IsFatalDbOpenError(TInt aError)
    {
    switch (aError)
        {
        case KErrNone:              //FALLTHROUGH
        case KErrCancel:            //FALLTHROUGH
        case KErrNoMemory:          //FALLTHROUGH
        case KErrDied:              //FALLTHROUGH
        case KErrInUse:             //FALLTHROUGH
        case KErrServerTerminated:  //FALLTHROUGH
        case KErrServerBusy:        //FALLTHROUGH
        case KErrNotReady:          //FALLTHROUGH
        case KErrAccessDenied:      //FALLTHROUGH
        case KErrLocked:            //FALLTHROUGH
        case KErrWrite:             //FALLTHROUGH
        case KErrDisMounted:        //FALLTHROUGH
        case KErrDiskFull:          //FALLTHROUGH
        case KErrAbort:             //FALLTHROUGH
        case KErrBadPower:          //FALLTHROUGH
        case KErrDirFull:           //FALLTHROUGH
        case KErrHardwareNotAvailable:
            {
            return EFalse;
            }

        // All error codes not handled above end up here, especially
        // KErrCorrupt.
        default:
            {
            return ETrue;
            }
        }
    }

TBool IsNonDuplicableFieldType(TFieldType aFieldType)
    {
    switch (aFieldType.iUid)
        {
        case KUidContactsVoiceDialFieldValue:	//FALLTHROUGH
        case KUidSpeedDialOneValue:				//FALLTHROUGH
        case KUidSpeedDialTwoValue:				//FALLTHROUGH
        case KUidSpeedDialThreeValue:			//FALLTHROUGH
        case KUidSpeedDialFourValue:			//FALLTHROUGH
        case KUidSpeedDialFiveValue:			//FALLTHROUGH
        case KUidSpeedDialSixValue:				//FALLTHROUGH
        case KUidSpeedDialSevenValue:			//FALLTHROUGH
        case KUidSpeedDialEightValue:			//FALLTHROUGH
        case KUidSpeedDialNineValue:
            {
            return ETrue;
            }

        default:
            {
            return EFalse;
            }
        }
    }

void RemoveNonDuplicableContactInformation(CContactItem& aContactItem)
    {
    // Remove all non duplicable field types from all fields
    const TInt fieldCount = aContactItem.CardFields().Count();
    for (TInt fieldIndex = 0; fieldIndex < fieldCount; ++fieldIndex)
        {
        CContactItemField& field = aContactItem.CardFields()[fieldIndex];
        const CContentType& contentType = field.ContentType();
        TBool typeRemoved;
        do
            {
            typeRemoved = EFalse;
		    const TInt typeCount = contentType.FieldTypeCount();
            for (TInt typeIndex = 0; typeIndex < typeCount; ++typeIndex)
                {
                const TFieldType type = contentType.FieldType(typeIndex);
                if (IsNonDuplicableFieldType(type))
                    {
                    field.RemoveFieldType(type);
                    typeRemoved = ETrue;
                    break;
                    }
                }
            }
        while (typeRemoved);
        }
    }

}  // namespace

//  End of File
