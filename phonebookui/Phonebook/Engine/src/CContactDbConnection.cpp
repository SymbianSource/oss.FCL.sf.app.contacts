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
*        Implementation helper class for class CPbkContactEngine.
*
*/


// INCLUDE FILES
#include "CPbkContactEngine.h"
#include "CContactDbConnection.h" 

#include <cntdb.h>
#include <cntview.h>
#include <cntitem.h>
#include <sysutil.h>
#include <featmgr.h>
#include <shareddataclient.h>

#include <PbkDebug.h>
#include "PbkProfiling.h"

#include "PbkCompressConfig.h"
#include "CPbkDefaultCompressionStrategy.h"

#include "PbkUID.h"
#include "CPbkFieldsInfo.h"
#include "TPbkMatchPriorityLevel.h"
#include "CPbkEngineExtension.h"
#include "CPbkSortOrderManager.h"
#include "PbkNameFormatterFactory.h"
#include <MPbkContactNameFormat.h>

// Unnamed namespace for local definitions

namespace {

// LOCAL CONSTANTS AND MACROS
/// Name of the shared contacts view
_LIT(KPbkAllContactsViewName, "AllContacts");

/// Default contact database name
_LIT(KDefaultDbName, "C:Contacts.cdb");

/// Default contact view preferences
const TContactViewPreferences KPbkDefaultContactViewPrefs =
    static_cast<TContactViewPreferences>
        (EContactsOnly | EUnSortedAtEnd | ESingleWhiteSpaceIsEmptyField);

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicUninitializedData = 1
    };

// LOCAL FUNCTIONS
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CContactDbConnection");
    User::Panic(KPanicText, aReason);
    }
#endif


// ==================== LOCAL FUNCTIONS ====================

/**
 * Returns true if aLhs and aRhs differ.
 */
inline TBool operator!=
        (const RContactViewSortOrder& aLhs, const RContactViewSortOrder& aRhs)
    {
    return !(aLhs == aRhs);
    }

/**
 * Wrapper class for calling default file name or specified filename versions
 * of CContactDatabase::ReplaceL, OpenL and CreateL.
 */
NONSHARABLE_CLASS(TDbOpen)
    {
    public:
        /**
         * Constructor.
		 * @param aFileName file name
		 * @param aReplace open or replace the database
		 * @param aSharedDataClient shared data client
         */ 
        TDbOpen(const TDesC* aFileName, TBool aReplace, 
            const RSharedDataClient& aSharedDataClient);

        /**
         * Opens or replaces database.
         */ 
        CContactDatabase* OpenOrReplaceL() const;

        /**
         * Creates a new database.
         */
        CContactDatabase* CreateL() const;

        /**
         * Returns the drive the database is stored on.
         * @precond OpenOrReplaceL() or CreateL() has been called.
         */
        TDriveNumber DbDrive(RFs& aFs) const;

    private: // Implementation
        CContactDatabase* ReplaceL() const;
        CContactDatabase* OpenL() const;

    private:
		/// Own: file name
        TFileName iFileName;
		/// Own: open or replace flag
        const TBool iReplace;
		/// Ref: contact database
        mutable CContactDatabase* iDb;
        /// Ref: Shared data client
        const RSharedDataClient& iSharedDataClient;
    };

/**
 * Updates system template to reflect aFieldsInfo.
 */
TBool UpdateSystemTemplateFieldsL
        (CContactItem& aSysTemplate, 
        const CPbkFieldsInfo& aFieldsInfo);

}  // namespace


// ================= MEMBER FUNCTIONS =======================

inline CPbkContactEngine::CContactDbConnection::CContactDbConnection()
    {
    }

inline TBool CPbkContactEngine::CContactDbConnection::CheckSystemTemplateL
        (const CPbkFieldsInfo& aFieldsInfo)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING(
        "CContactDbConnection::CheckSystemTemplateL(0x%x)"),this);

    // Open the system template
    TRAPD(err, iSysTemplate = iContactDb->OpenContactL(
        iContactDb->TemplateId()));
    switch (err)
        {
        case KErrNone:
            {
            // All is well
            break;
            }
        case KErrInUse:
            {
            // Assume some other CPbkContactEngine instance is currently 
            // performing this check -> skip the test. The test could be
            // reattempted after a delay but that would slow down the
            // initialisation of this class even more.
            return EFalse;
            }
        default:
            {
            // Propagate other errors to caller
            User::Leave(err);
            return EFalse;
            }
        }

	TBool result = EFalse;
    if (UpdateSystemTemplateFieldsL(*iSysTemplate,aFieldsInfo))
        {
        PBK_DEBUG_PRINT(PBK_DEBUG_STRING(
            "CContactDbConnection::CheckSystemTemplateL(0x%x): updating system template...."),
            this);
        iContactDb->CommitContactL(*iSysTemplate);

		result = ETrue;
        PBK_DEBUG_PRINT(PBK_DEBUG_STRING(
            "CContactDbConnection::CheckSystemTemplateL(0x%x): system template updated."), this);
        }

    CloseSystemTemplate();

    PBK_DEBUG_PRINT(PBK_DEBUG_STRING(
        "CContactDbConnection::CheckSystemTemplateL(0x%x) succesful"),this);
	return result;
    }

inline void CPbkContactEngine::CContactDbConnection::CreateAllContactsViewL
        (const RContactViewSortOrder& aSortOrder)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING(
        "CContactDbConnection::CreateAllContactsViewL(0x%x)"),this);

    __PBK_PROFILE_RESET(PbkProfiling::EAllContactsViewOpen);
    __PBK_PROFILE_START(PbkProfiling::EAllContactsViewOpen);
    iAllContactsView = CContactNamedRemoteView::NewL
        (*this, 
        KPbkAllContactsViewName, 
        Database(), 
        aSortOrder, 
        KPbkDefaultContactViewPrefs);

	InitViewL(*iAllContactsView);

    PBK_DEBUG_PRINT(PBK_DEBUG_STRING(
        "CContactDbConnection::CreateAllContactsViewL(0x%x) succesful"),this);
    }

inline void CPbkContactEngine::CContactDbConnection::ConstructL
        (const TParams& aParams, TBool aSettingsVisible)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING(
        "CContactDbConnection::ConstructL(0x%x)"),this);
    iFs = aParams.iFs;

    __PBK_PROFILE_START(PbkProfiling::EContactDbOpen);

    // Shared data client is created and connected in CPbkContactEngine
    __ASSERT_DEBUG(aParams.iEngine && aParams.iEngine->iSharedDataClient,
        Panic(EPanicUninitializedData));
    
    TDbOpen dbOpen(aParams.iDbFileName, aParams.iReplaceExistingDb, 
        *aParams.iEngine->iSharedDataClient);
    TRAPD(error, iContactDb = dbOpen.OpenOrReplaceL());
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING(
        "CContactDbConnection::ConstructL(0x%x): CContactDatabase open result=%d."),this, error);
    if (error == KErrNotFound)
        {
	    // Db not found, create it
	    TRAP(error, iContactDb = dbOpen.CreateL());
        PBK_DEBUG_PRINT(PBK_DEBUG_STRING(
            "CContactDbConnection::ConstructL(0x%x): CContactDatabase create result=%d."),this, error);
        }

    __PBK_PROFILE_END(PbkProfiling::EContactDbOpen);

    *aParams.iDbOpenError = error;
    if (error != KErrNone)
        {
        User::Leave(error);
        }

    // Store database drive
    iDbDrive = dbOpen.DbDrive(iFs);

    // Call engine extension plugins
    aParams.iExtension->AddFieldTypesL(*aParams.iFieldsInfo);
    aParams.iExtension->ModifyFieldTypesL(*aParams.iFieldsInfo);

    // Check and update system template if needed
	TBool updated = EFalse;
    TRAP(error, updated = CheckSystemTemplateL(*aParams.iFieldsInfo));
    *aParams.iDbOpenError = error;
    if (error != KErrNone)
        {
        User::Leave(error);
        }

	if (updated)
		{
		// close and reopen contact database if system template has been updated
		delete iContactDb;
		iContactDb = NULL;
		iContactDb = dbOpen.OpenOrReplaceL();
		}

    // Create contact DB change notifier and connect it to aObserver
    iContactChangeNotifier = CContactChangeNotifier::NewL
        (*iContactDb, aParams.iContactDbObserver);

    // Create DB compressor
    iCompressionStrategy = CPbkDefaultCompressionStrategy::NewL
        (*iContactDb, iDbDrive, iFs);

	// Create contact Sort order managing object
    iSortOrderManager = CPbkSortOrderManager::NewL(iFs, aSettingsVisible);

    // Create the shared all contacts view
    CreateAllContactsViewL(iSortOrderManager->SortOrder());

	// Set view to sort order manager
	iSortOrderManager->SetContactView(*iAllContactsView);

    // create name formatting object
    iContactNameFormatter = PbkNameFormatterFactory::CreateL
        (*aParams.iUnnamedTitle, *iSortOrderManager, aSettingsVisible);


    PBK_DEBUG_PRINT(PBK_DEBUG_STRING(
        "CContactDbConnection::ConstructL(0x%x) succesful."),this);
    }

CPbkContactEngine::CContactDbConnection* CPbkContactEngine::CContactDbConnection::NewL
        (const TParams& aParams, TBool aSettingsVisible)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CContactDbConnection::NewL()"));

    CContactDbConnection* self = new(ELeave) CContactDbConnection;
    CleanupStack::PushL(self);
    self->ConstructL(aParams, aSettingsVisible);
    CleanupStack::Pop(self);

    PBK_DEBUG_PRINT(PBK_DEBUG_STRING(
        "CContactDbConnection::NewL() succesful, result=0x%x"), self);
    return self;
    }

CPbkContactEngine::CContactDbConnection::~CContactDbConnection()
    {
    for (TInt i = iFilteredViews.Count()-1; i >= 0; --i)
        {
        iFilteredViews[i].iFilteredView->Close(*this);
        }
    iFilteredViews.Close();
    if (iGroupsLocalView)
        {
        iGroupsLocalView->Close(*this);
        }

    if (iAllContactsView)
        {
        iAllContactsView->Close(*this);
        }
    
    if (iCompressionStrategy)
        {
        iCompressionStrategy->Release();
        }
    delete iContactChangeNotifier;
	delete iContactNameFormatter;
	delete iSortOrderManager;
    CloseSystemTemplate();
    delete iContactDb;
    }

CContactViewBase& CPbkContactEngine::CContactDbConnection::AllGroupsViewL()
    {
    // start loading the all the groups
    if (!iGroupsLocalView)
        {
        // Create a CContactLocalView that contains only groups.
        RContactViewSortOrder groupViewSortOrder;
        CleanupClosePushL(groupViewSortOrder);
        // KUidContactFieldTemplateLabel contains the group label
        groupViewSortOrder.AppendL(KUidContactFieldTemplateLabel);

        iGroupsLocalView = CContactLocalView::NewL(
            *this, *iContactDb, groupViewSortOrder, EGroupsOnly);

		InitViewL(*iGroupsLocalView);

        CleanupStack::PopAndDestroy();  // groupViewSortOrder
        }
    
    return *iGroupsLocalView;
    }

CContactViewBase& CPbkContactEngine::CContactDbConnection::FilteredContactsViewL
        (TInt aFilter)
    {
    const TInt count = iFilteredViews.Count();
    for (TInt i=0; i < count; ++i)
        {
        if (iFilteredViews[i].iFilter == aFilter)
            {
            PBK_DEBUG_PRINT(PBK_DEBUG_STRING(
                "Returned filtered view filter=%x from the cache"), aFilter);
            return *iFilteredViews[i].iFilteredView;
            }
        }

    // Ending this profile is in HandleContactViewEvent
    __PBK_PROFILE_RESET(PbkProfiling::EFilteredViewOpen);
    __PBK_PROFILE_START(PbkProfiling::EFilteredViewOpen);

    TFilteredView filter;
    filter.iFilter = aFilter;
    filter.iFilteredView = CContactFilteredView::NewL
        (*this, *iContactDb, *iAllContactsView, aFilter);
	InitViewL(*filter.iFilteredView);
    const TInt err = iFilteredViews.Append(filter);
    if (err != KErrNone)
        {
        filter.iFilteredView->Close(*this);
        User::Leave(err);
        }
    
    return *filter.iFilteredView;
    }

void CPbkContactEngine::CContactDbConnection::SetCompressUi
        (MPbkCompressUi* aCompressUi)
    {
    iCompressionStrategy->SetCompressUi(aCompressUi);
    }

TBool CPbkContactEngine::CContactDbConnection::IsCompressionEnabled() const
    {
    return iCompressionStrategy->IsCompressionEnabled();
    }

TBool CPbkContactEngine::CContactDbConnection::CheckCompress()
    {
    return iCompressionStrategy->CheckCompress();
    }

void CPbkContactEngine::CContactDbConnection::CompressL()
    {
    iCompressionStrategy->CompressL();
    }

void CPbkContactEngine::CContactDbConnection::CancelCompress() const
    {
    iCompressionStrategy->CancelCompress();
    }

void CPbkContactEngine::CContactDbConnection::CheckFileSystemSpaceAndCompressL()
    {
    PBK_COMPRESS_LOG
        (PBK_COMPRESS_STRING(
        "CContactDbConnection::CheckFileSystemSpaceAndCompressL(0x%x), DB size = %d bytes"), 
        this, Database().FileSize());

    // Cancel any async compress in progress
    CancelCompress();

    // Compress synchronously always if compression is required by the DB 
    // or file system space is below critical level
    if (Database().CompressRequired() || FileSpaceLowOnDbDrive())
        {
        TRAPD(result, Database().CompactL());
        PBK_COMPRESS_LOG(PBK_COMPRESS_STRING("    Compacted database, result=%d"), result);
        }

    // If file system space is still under critical level leave with 
    // error code KErrDiskFull.
    if (FileSpaceLowOnDbDrive())
        {
        PBK_COMPRESS_LOG
            (PBK_COMPRESS_STRING("    FFS space is under critical level -> leaving with KErrDiskFull(%d)"), 
            KErrDiskFull);
        User::Leave(KErrDiskFull);
        }
    }

TBool CPbkContactEngine::CContactDbConnection::FileSpaceLowOnDbDrive() const
    {
    TBool result = EFalse;
    TRAP_IGNORE(result = SysUtil::DiskSpaceBelowCriticalLevelL(&iFs,0,iDbDrive));
    if (result)
        {
        PBK_COMPRESS_LOG(PBK_COMPRESS_STRING("CContactDbConnection::FileSpaceLowOnDbDrive() returning ETrue"));
        }
    return result;
    }

void CPbkContactEngine::CContactDbConnection::HandleContactViewEvent
#ifdef PBK_ENABLE_PROFILE
        (const CContactViewBase& aView, const TContactViewEvent& aEvent)
#else
        (const CContactViewBase& /*aView*/, const TContactViewEvent& /*aEvent*/)
#endif
    {
#ifdef PBK_ENABLE_PROFILE
    if (aEvent.iEventType != TContactViewEvent::EReady)
        {
        return;
        }
    if (&aView == iAllContactsView)
        {
        __PBK_PROFILE_END(PbkProfiling::EAllContactsViewOpen);
        __PBK_PROFILE_DISPLAY(PbkProfiling::EAllContactsViewOpen);
        __PBK_PROFILE_RESET(PbkProfiling::EAllContactsViewOpen);
        }
    else if (iFilteredViews.Count() > 0)
        {
        for (TInt i=0; i < iFilteredViews.Count(); ++i)
            {
            if (iFilteredViews[i].iFilteredView == &aView)
                {
                __PBK_PROFILE_END(PbkProfiling::EFilteredViewOpen);
                __PBK_PROFILE_DISPLAY(PbkProfiling::EFilteredViewOpen);
                __PBK_PROFILE_RESET(PbkProfiling::EFilteredViewOpen);
                }
            }

        }
#endif // PBK_ENABLE_PROFILE
    }

void CPbkContactEngine::CContactDbConnection::CloseSystemTemplate()
    {
    if (iSysTemplate)
        {
        // Contact Model documentation says CloseContactL cannot leave
        iContactDb->CloseContactL(iSysTemplate->Id());
        delete iSysTemplate;
        iSysTemplate = NULL;
        }
    }

void CPbkContactEngine::CContactDbConnection::ChangeSortOrderL
		(const RContactViewSortOrder& aSortOrder) const
	{
	// Compare is the new sort order same than the previous one
	if (iAllContactsView->SortOrderL() != aSortOrder)
		{
		// It wasn't, change the sort order
		iAllContactsView->ChangeSortOrderL(aSortOrder);
		}
	}

MPbkContactNameFormat& CPbkContactEngine::CContactDbConnection::ContactNameFormatter() const
	{
	return *iContactNameFormatter;
	}

void CPbkContactEngine::CContactDbConnection::SetNameDisplayOrderL
		(TPbkNameOrder aNameOrder)
	{
	iSortOrderManager->SetNameDisplayOrderL(aNameOrder);
	}

CPbkContactEngine::TPbkNameOrder CPbkContactEngine::CContactDbConnection::NameDisplayOrder()
	{
	return iSortOrderManager->NameDisplayOrder();
	}
	
	
void CPbkContactEngine::CContactDbConnection::SetNameSeparatorL(TChar aNameSeparator)
    {
    iSortOrderManager->SetNameSeparatorL(aNameSeparator);
    }
    
TChar CPbkContactEngine::CContactDbConnection::NameSeparator()
    {
    return iSortOrderManager->NameSeparator();
    }

/**
 * Call this method for all views created in this class.
 */
void CPbkContactEngine::CContactDbConnection::InitViewL(CContactViewBase& aView)
	{
	// Set find plugin
	aView.SetViewFindConfigPlugin(TUid::Uid(KFindPluginUID));
	}


namespace {

const TInt KDiskSpaceForDbOpening = 140*1024; // Space for db opening

// ==================== LOCAL FUNCTIONS ====================

/**
 * Returns the default database name using CContactDatabase::GetDefaultNameL().
 * If GetDefaultNameL() fails returns silently KDefaultDbName.
 */ 
void GetDefaultDatabaseName(TDes& aName)
    {
    TRAPD(err, CContactDatabase::GetDefaultNameL(aName));
    if (err != KErrNone)
        {
        PBK_DEBUG_PRINT(PBK_DEBUG_STRING(
            "CContactDatabase::GetDefaultNameL() failed with error code %d"),
            err);
        aName.Copy(KDefaultDbName);
        }
    }
TDbOpen::TDbOpen(const TDesC* aFileName, TBool aReplace, 
    const RSharedDataClient& aSharedDataClient):
    iReplace(aReplace), iSharedDataClient(aSharedDataClient)
    {
    if (aFileName)
        {
        iFileName.Copy(aFileName->Left(iFileName.MaxLength()));
        }
    else
        {
        GetDefaultDatabaseName(iFileName);
        }
    }

inline CContactDatabase* TDbOpen::ReplaceL() const
    {
    iDb = CContactDatabase::ReplaceL(iFileName);
    return iDb;
    }


inline CContactDatabase* TDbOpen::OpenL() const
    {   
     
    TRAPD(err,iDb = CContactDatabase::OpenL(iFileName));

    //Handle KErrDiskFull case separately
    if(KErrDiskFull==err)
        {
        PBK_DEBUG_PRINT(PBK_DEBUG_STRING(
            "CContactDbConnection::TDbOpen::OpenL(0x%x) KErrDiskFull"),this);
        // Try to release space for contact db opening
        iSharedDataClient.RequestFreeDiskSpaceLC(KDiskSpaceForDbOpening);
        iDb = CContactDatabase::OpenL(iFileName);        
        CleanupStack::PopAndDestroy();  // RequestFreeDiskSpaceLC
        PBK_DEBUG_PRINT(PBK_DEBUG_STRING(
            "CContactDbConnection::TDbOpen::OpenL(0x%x) ContactDb open"),this);
        }
    else
        {
        User::LeaveIfError(err);
        }        
    return iDb;
    }


CContactDatabase* TDbOpen::OpenOrReplaceL() const
    {
    return (iReplace ? ReplaceL() : OpenL());
    }

CContactDatabase* TDbOpen::CreateL() const
    {
    iDb = CContactDatabase::CreateL(iFileName);
    return iDb;
    }

TDriveNumber TDbOpen::DbDrive(RFs& aFs) const
    {
    TInt result = EDriveC;
    TParsePtrC fileParser(iFileName);
    if (fileParser.DrivePresent())
        {
        const TChar driveLetter(fileParser.Drive()[0]);
        TInt drive;
        if (aFs.CharToDrive(driveLetter,drive) == KErrNone)
            {
            result = drive;
            }
        }
    else
        {
        TDriveUnit driveUnit;
        //Note: this function can leave
        TRAPD(err, iDb->DatabaseDrive(driveUnit));
        if (err == KErrNone)
            {
            result = driveUnit;
            }
        }
    return static_cast<TDriveNumber>(result);
    }

/**
 * Updates a single field to the contact database system template. If the field
 * does not exist in the system template it is added. If the field exists but 
 * its label differs from the field type label the system template label is 
 * updated.
 *
 * @param aSysTemplate  the system template.
 * @param aFieldInfo    the field type to update.
 * @return true if the system template was updated.
 */
TBool UpdateSystemTemplateFieldL
        (CContactItem& aSysTemplate, const CPbkFieldInfo& aFieldInfo)
    {
    TBool updated = EFalse;
    // Scan system template field set for the field
    CContactItemFieldSet& fieldSet = aSysTemplate.CardFields();
    TInt i;
    const TInt fieldCount = fieldSet.Count();
    for (i = 0; i < fieldCount; ++i)
        {
        CContactItemField& sysTemplateField = fieldSet[i];
        if (aFieldInfo.IsEqualType(sysTemplateField))
            {
            // Field was found, just check the label
            if (!aFieldInfo.IsEqualLabel(sysTemplateField))
                {
                sysTemplateField.SetLabelL(aFieldInfo.FieldName());
                updated = ETrue;
                }
            break;
            }
        }
    
    if (i == fieldCount)
        {
        // Field was not found, add it
        CContactItemField* field = aFieldInfo.CreateFieldL();
        CleanupStack::PushL(field);
        fieldSet.AddL(*field);
        CleanupStack::Pop(field);
        updated = ETrue;
        }    
    return updated;
    }

TBool UpdateSystemTemplateFieldsL
        (CContactItem& aSysTemplate, 
        const CPbkFieldsInfo& aFieldsInfo)
    {
    TBool updated = EFalse;
    const TInt fieldInfoCount = aFieldsInfo.Count();

    // Update fields
    for (TInt i=0; i < fieldInfoCount; ++i)
        {
        if (UpdateSystemTemplateFieldL(aSysTemplate, *aFieldsInfo[i]))
            {
            updated = ETrue;
            }
        }

    return updated;
    }

}  // namespace

//  End of File  
