/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CNewContactLauncherAppUi implementation
*
*/

// INCLUDE FILES
#include <avkon.hrh>
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h>
#include <StringLoader.h>
#include <f32file.h>
#include <s32file.h>
#include <hlplch.h>

#include "NewContactLauncher.hrh"
#include "NewContactLauncher.pan"
#include "NewContactLauncherApplication.h"
#include "NewContactLauncherAppUi.h"
#include "NewContactLauncherAppView.h"
#include <e32std.h>  
#include <apgtask.h>
#include <apgcli.h> 
#include <bautils.h>
#include <AiwCommon.h>
#include <AiwContactSelectionDataTypes.h>
#include <AiwContactAssignDataTypes.h>
#include <AiwVariant.h>
#include <NewContactLauncher.rsg>
#include <coemain.h>
#include <coeutils.h>
#include <sysutil.h>


_LIT( KNewContactLauncherResourceFileName, "\\resource\\apps\\newcontactlauncher.rsc");


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CNewContactLauncherAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNewContactLauncherAppUi::ConstructL()
    {
    // Initialise app UI with standard value.
    BaseConstructL(CAknAppUi::EAknEnableSkin);

    // Create view object
    iAppView = CNewContactLauncherAppView::NewL(ClientRect());

    TFileName resfileName;
    resfileName.Append (KNewContactLauncherResourceFileName);
    
    BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(), resfileName );
  
    if ( !ConeUtils::FileExists ( resfileName) )
        {
        User::Leave( KErrGeneral );
        }
    
    iResourceOffset = CCoeEnv::Static()->AddResourceFileL( resfileName );
   
    iServiceHandler = CAiwServiceHandler::NewL();
       
    iServiceHandler->AttachL( R_AIWASSIGNMENT_INTEREST );
    
    TUint assignFlags = 0;
       assignFlags |= AiwContactAssign::ECreateNewContact;
           
    iAiwParamList = CAiwGenericParamList::NewL();
   
    iAiwParamList->AppendL(
            TAiwGenericParam(
                    EGenericParamContactAssignData,
                    TAiwVariant(AiwContactAssign::TAiwSingleContactAssignDataV1Pckg(
                            AiwContactAssign::TAiwSingleContactAssignDataV1().SetFlags( assignFlags )))));
    iServiceHandler->ExecuteServiceCmdL(
            KAiwCmdAssign,
            *iAiwParamList,
            iServiceHandler->OutParamListL(),
            0,
            this);
    
    }
// -----------------------------------------------------------------------------
// CNewContactLauncherAppUi::CNewContactLauncherAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CNewContactLauncherAppUi::CNewContactLauncherAppUi()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CNewContactLauncherAppUi::~CNewContactLauncherAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CNewContactLauncherAppUi::~CNewContactLauncherAppUi()
    {
    delete iAppView;

    if (iServiceHandler)
        {
        iServiceHandler->Reset();
        }
    delete iServiceHandler;
    
    CCoeEnv::Static()->DeleteResourceFile( iResourceOffset );
    
    delete iAiwParamList;
    }

// -----------------------------------------------------------------------------
//  Called by the framework when the application status pane
//  size is changed.  Passes the new client rectangle to the
//  AppView
// -----------------------------------------------------------------------------
//
void CNewContactLauncherAppUi::HandleStatusPaneSizeChange()
    {
    iAppView->SetRect(ClientRect());
    }

TInt CNewContactLauncherAppUi::HandleNotifyL( TInt /*aCmdId*/, TInt /*aEventId*/,
        CAiwGenericParamList& aEventParamList,
        const CAiwGenericParamList& /*aInParamList*/ )
    {
    if ( aEventParamList.Count() > 0 )
        {
        const TAiwGenericParam& aiwGenericParam = aEventParamList[0];
        TInt err = aiwGenericParam.Value().AsTInt32();
        if ( err == KErrDiskFull )
            {
            // Show not enough memory note
            CCoeEnv::Static()->HandleError( err );
            }
        }
    Exit();
    return KErrNone;
    }

void CNewContactLauncherAppUi::ProcessMessageL( TUid /*aUid*/, const TDesC8& /*aParamData*/)
    {
     // no implementation needed now
    }


 
// End of File
