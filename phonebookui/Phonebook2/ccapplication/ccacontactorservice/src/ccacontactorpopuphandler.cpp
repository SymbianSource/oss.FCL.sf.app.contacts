/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for handling popup query.
 *
*/


#include <e32std.h>
#include <s32mem.h>
#include <aknappui.h>
#include <coemain.h> 

#include "ccacontactorserviceheaders.h"

// ================= MEMBER FUNCTIONS =======================
//

// --------------------------------------------------------------------------
// CCCAContactorPopupHandler::NewL
// --------------------------------------------------------------------------
//
CCCAContactorPopupHandler* CCCAContactorPopupHandler::NewL()
    {
    CCCAContactorPopupHandler* self = new (ELeave) CCCAContactorPopupHandler();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CCCAContactorPopupHandler::~CCCAContactorPopupHandler
// --------------------------------------------------------------------------
//
CCCAContactorPopupHandler::~CCCAContactorPopupHandler()
    {
    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorPopupHandler::~CCCAContactorPopupHandler"));
    delete iServiceHandler;
    delete iSchedulerWait;
    iCoeEnv.DeleteResourceFile (iResourceOffset);
    delete iQueryResults;
    }

// --------------------------------------------------------------------------
// CCCAContactorPopupHandler::LaunchPopupL
// --------------------------------------------------------------------------
//
TDesC& CCCAContactorPopupHandler::LaunchPopupL(
    const TDesC8& aContactLinkArray,
    TAiwCommAddressSelectType aCommAddressSelectType,
    TBool aUseDefaults )
    {
    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorPopupHandler::LaunchPopupL"));

    __ASSERT_ALWAYS (NULL != &aContactLinkArray, User::Leave (KErrArgument));
    __ASSERT_ALWAYS (0 < aContactLinkArray.Size(), User::Leave (KErrArgument));

    TUint fetchFlags = 0;

    // Stop scheduler just in case this is called while operation is still ongoing.
    AsyncStop();
    
    // Set service data
    TAiwSingleItemSelectionDataV3 data;

    if (!aUseDefaults)
        {
        fetchFlags |= ::EDoNotUseDefaultField;
        }

    data.SetFlags(fetchFlags);
    data.SetCommAddressSelectType(aCommAddressSelectType);
    CAiwGenericParamList& inParamList = iServiceHandler->InParamListL();
    inParamList.AppendL(TAiwGenericParam(EGenericParamContactSelectionData,
        TAiwVariant(TAiwSingleItemSelectionDataV3Pckg(data))));

    // Set contactlink
    inParamList.AppendL(TAiwGenericParam(EGenericParamContactLinkArray,
        TAiwVariant(aContactLinkArray)));
    
    //Append the current status pane id
    //this will also be used by Pbk2ServerApplication
    //This is kind of a new implementation which will pave way
    //for the applications to use the required statuspane that they
    //prefer. 
    //since its not feasible to add any new AIW functionality
    //we will follow this approach.
    CEikStatusPane* statusPane =
            CEikonEnv::Static()->AppUiFactory()->StatusPane();
    
    if ( statusPane )
        {
        TInt currentstatuspane = statusPane->CurrentLayoutResId();        
        TBuf<KCCAMAXBUFFSIZE> numBuf;
        numBuf.AppendNum( currentstatuspane );
        
        inParamList.AppendL(TAiwGenericParam(EGenericParamUnspecified,
                    TAiwVariant( numBuf )));
        }
        

    iServiceHandler->ExecuteServiceCmdL(KAiwCmdSelect, inParamList,
        iServiceHandler->OutParamListL(), 0, this);

    CAknInputBlock* inputBlock = CAknInputBlock::NewCancelHandlerLC( this );
    iCloseApp = EFalse;
    
    // Waiting for popup selection.
    // Make this whole operation look synchronous
    iSchedulerWait->Start();
    
    CleanupStack::PopAndDestroy( inputBlock );
    
    // If error case occurred, leave
    if (KErrNone != iError)
        {
        if (iCloseApp)
            {
            // Send an exit command in order to close CCA and Phonebook2
            // once the end(red) key is pressed or Exit command 
            CAknAppUi* appUi = static_cast<CAknAppUi*>( iCoeEnv.AppUi() );

            if ( appUi )
                {
                appUi->ProcessCommandL( EAknCmdExit );
                }
            }
        User::Leave(iError);
        }

    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorPopupHandler::LaunchPopupL: Done."));

    return *iQueryResults;
    }

// --------------------------------------------------------------------------
// CCCAContactorPopupHandler::HandleNotifyL
// --------------------------------------------------------------------------
//
TInt CCCAContactorPopupHandler::HandleNotifyL(TInt aCmdId, TInt aEventId,
        CAiwGenericParamList& aEventParamList,
        const CAiwGenericParamList& /*aInParamList*/)
    {
    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorPopupHandler::HandleNotifyL"));
    TInt returnValue = KErrNone;

    if (aCmdId == KAiwCmdSelect)
        {
        switch(aEventId)
            {
            case KAiwEventCompleted:
                {
                CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorPopupHandler::HandleNotifyL: KAiwEventCompleted"));
                // Selection done.
                // Read and store results
                TRAP(iError, ReadFieldDataL(aEventParamList));
                }
                break;

            case KAiwEventCanceled:
                CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorPopupHandler::HandleNotifyL: KAiwEventCanceled"));
                // Selection canceled.
                iError = KErrCancel;
                break;

            case KAiwEventError:
            case KAiwEventStopped:
                CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorPopupHandler::HandleNotifyL: KAiwEventError or KAiwEventStopped"));
                // Error occurred.
                iError = KErrGeneral;
                break;

            case KAiwEventQueryExit:
                // TODO: how should this work?
                iError = KErrAbort;
                iCloseApp = ETrue;
                returnValue = ETrue;
                break;

            default:
                CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorPopupHandler::HandleNotifyL: Unknown"));
                iError = KErrGeneral;
                break;
            }
        // if notify received, stop scheduler
        AsyncStop();
        }

    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorPopupHandler::HandleNotifyL: Done."));
    return returnValue;
    }

// --------------------------------------------------------------------------
// CCCAContactorPopupHandler::CCCAContactorPopupHandler
// --------------------------------------------------------------------------
//
CCCAContactorPopupHandler::CCCAContactorPopupHandler() :
    iCoeEnv(*CCoeEnv::Static()),
    iError(KErrNone)
    {
    }

// --------------------------------------------------------------------------
// CCCAContactorPopupHandler::ConstructL
// --------------------------------------------------------------------------
//
void CCCAContactorPopupHandler::ConstructL()
    {
    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorPopupHandler::ConstructL"));

    PrepareResourcesL();

    iServiceHandler = CAiwServiceHandler::NewL();
    iServiceHandler->AttachL(R_CCACONTACTORSERVICE_CONTACT_SELECTION_INTEREST);

    iSchedulerWait = new (ELeave) CActiveSchedulerWait();

    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorPopupHandler::ConstructL: Done."));
    }

// --------------------------------------------------------------------------
// CCCAContactorPopupHandler::PrepareResourcesL
// --------------------------------------------------------------------------
//
void CCCAContactorPopupHandler::PrepareResourcesL()
    {
    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorPopupHandler::PrepareResourcesL"));

    // preparing resources for use
    TFileName fileName;
    fileName.Append (KCCAContactorServiceResourceFileName);

    BaflUtils::NearestLanguageFile (iCoeEnv.FsSession (), fileName);
    if (ConeUtils::FileExists (fileName))
        {
        iResourceOffset = iCoeEnv.AddResourceFileL (fileName);
        }
    else // not found from user drive, leave badly.
        {
        CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorPopupHandler::PrepareResourcesL: Resource file not exists."));
        User::Leave (KErrGeneral);
        }

    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorPopupHandler::PrepareResourcesL: Done."));
    }

// --------------------------------------------------------------------------
// CCCAContactorPopupHandler::ReadFieldDataL
// --------------------------------------------------------------------------
//
void CCCAContactorPopupHandler::ReadFieldDataL(
    CAiwGenericParamList& aEventParamList)
    {
    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorPopupHandler::ReadFieldDataL"));

    if (iQueryResults)
        {
        delete iQueryResults;
        iQueryResults = NULL;
        }

    TInt index = 1; // Get field data from index 1
    const TAiwGenericParam* param =
         aEventParamList.FindFirst(index, EGenericParamContactFieldData);
    if (KErrNotFound != index)
        {
        TInt length = param->Value().AsDes().Length();
        iQueryResults = HBufC::NewL(length);
        iQueryResults->Des().Append(param->Value().AsDes());

        // Data must be found!
        if (0 >= iQueryResults->Length())
            {
            CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorPopupHandler::ReadFieldDataL: Failed to get field data!"));
            User::Leave(KErrGeneral);
            }
        }
    else
        {
        CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorPopupHandler::ReadFieldDataL: Not found!"));
        User::Leave(KErrNotFound);
        }

    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorPopupHandler::ReadFieldDataL: Done."));
    }

// --------------------------------------------------------------------------
// CCCAContactorPopupHandler::AsyncStop
// --------------------------------------------------------------------------
//
void CCCAContactorPopupHandler::AsyncStop()
    {
    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorPopupHandler::AsyncStop"));

    if(iSchedulerWait->IsStarted())
        {
        iSchedulerWait->AsyncStop();
        }

    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorPopupHandler::AsyncStop: Done."));
    }

// --------------------------------------------------------------------------
// CCCAContactorPopupHandler::AknInputBlockCancel
// --------------------------------------------------------------------------
//
void CCCAContactorPopupHandler::AknInputBlockCancel()
    {
    if ( iSchedulerWait->IsStarted() )
        {
        TRAPD( error, iServiceHandler->ExecuteServiceCmdL( KAiwCmdSelect, 
                                                           iServiceHandler->InParamListL(), 
                                                           iServiceHandler->OutParamListL(),
                                                           KAiwOptCancel,
                                                           this ) );
        if ( error == KErrNone )
            {
            iCloseApp = ETrue;
            }
        }
    }

// End of file
