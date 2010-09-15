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
*     Declares UI class for application.
*
*/






// INCLUDE FILES
#include <avkon.hrh>

#include <bldvariant.hrh>
#include <hlplch.h>
#include <featmgr.h>
#include <aknview.h>

#include <speeddial.rsg>
#include <AknGlobalNote.h>

#include "speeddial.hrh"
#include "SpdiaAppUi.h"
#include "SpdiaView.h"
#include "SpdiaContainer.h"
#include "SpdiaCallingVmbxView_voip.h"
#include "speeddialprivate.h"//for migration


#include <e32property.h>
#include <aknnotewrappers.h> 
#include <StringLoader.h>



#include <BTSapInternalPSKeys.h>
#include <centralrepository.h>

#include <AknNotify.h>
#include <AknNotifyStd.h>

#include <CVPbkContactStoreUriArray.h>
#include <TVPbkContactStoreUriPtr.h>
#include <CPbk2StoreConfiguration.h>
#include <VPbkContactStoreUris.h>

//  local constant
const TUint32 KSettingAppID = 0x100058EC;

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CSpdiaAppUi::ConstructL()
// ?implementation_description
// ----------------------------------------------------------
void CSpdiaAppUi::ConstructL()
    {
    BaseConstructL(EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible ); 
    CEikonEnv::Static()->AppUiFactory()->StatusPane()->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_EMPTY );
    iSettingType = iEikonEnv->StartedAsServerApp();
    FeatureManager::InitializeLibL();

	
    CPbk2StoreConfiguration* configuration = CPbk2StoreConfiguration::NewL();
    CleanupStack::PushL(configuration);
    CVPbkContactStoreUriArray* uriArray = configuration->CurrentConfigurationL();
    if (EFalse == uriArray->IsIncluded(VPbkContactStoreUris::DefaultCntDbUri()))
    {
    	uriArray->AppendL(VPbkContactStoreUris::DefaultCntDbUri());	
    }
    CleanupStack::PushL(uriArray);
   	iContactManager = NULL;
    iContactManager = CVPbkContactManager::NewL(*uriArray);
    CleanupStack::PopAndDestroy(2); // uriArray, configuration
   
    iSpeedPrivate = NULL;
	iSpeedPrivate = CSpeedDialPrivate::NewL(iContactManager);
	TInt error = iSpeedPrivate->GetSpdCtrlLastError();
	
	
    CAknView* view1 = CSpdiaView::NewLC(iSpeedPrivate);
    AddViewL( view1 );       // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();    // view1

    CSpdiaCallingVmbxView* view2 = CSpdiaCallingVmbxView::NewLC();
    AddViewL( view2 );
    CleanupStack::Pop();    // view2

	
	if (error != KErrNone)
      {
        LaunchInfoNoteL();
      }
    SetDefaultViewL( *view1 );
#ifdef __BT_SAP
	if ( FeatureManager::FeatureSupported( KFeatureIdBtSap ) ) 	
	{

	        TInt sapState(0); 
	        if ( RProperty::Get( 
	            KPSUidBluetoothSapConnectionState, 
	            KBTSapConnectionState, 
	            sapState ) == KErrNone ) 
	            { 
	           if ( sapState == EBTSapConnected ) 
	                { 

	HBufC* informationNoteTxt = StringLoader::LoadLC(R_OFFLINE_NOT_POSSIBLE_SAP);
	TPtrC note = informationNoteTxt->Des();				    
				    CAknGlobalNote* globalNote = CAknGlobalNote::NewLC();
				    globalNote->SetGraphic(EMbmAvkonQgn_note_info,EMbmAvkonQgn_note_info_mask);
	globalNote->ShowNoteL(EAknGlobalErrorNote,note); 
				    CleanupStack::PopAndDestroy(globalNote);
					Exit();
				   				    		                         
	                } 
	           } 

		
	}
#endif
    }

// ----------------------------------------------------
// CSpdiaAppUi::~CSpdiaAppUi()
// Destructor
// Frees reserved resources
// ----------------------------------------------------
CSpdiaAppUi::~CSpdiaAppUi()
    {
    FeatureManager::UnInitializeLib();
    
	 CSpdiaView* view = dynamic_cast<CSpdiaView*>(View( KViewId ));
	 if ( view )
	     {
	     view->SetSpeedPrivate( NULL );
	     }
    delete iSpeedPrivate;
    delete iContactManager;
    }

// ----------------------------------------------------
// CSpdiaAppUi::HandleCommandL(TInt aCommand)
// ?implementation_description
// ----------------------------------------------------
void CSpdiaAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EEikCmdExit:
            {
            if ( iSettingType )
                {
            RWsSession& ws = iCoeEnv->WsSession();
            TApaTaskList taskList(ws);
            TUid appUid = { KSettingAppID };
            TApaTask task = taskList.FindApp( appUid );
            if ( task.Exists() )
                {
                task.SendToBackground();
                task.EndTask();
                    }
                }
            Exit();
            break;
            }
        case EAknCmdHelp:
            {
            if (FeatureManager::FeatureSupported( KFeatureIdHelp ))
                {
                CArrayFix<TCoeHelpContext>* buf = AppHelpContextL();
                HlpLauncher::LaunchHelpApplicationL(iEikonEnv->WsSession(), buf);
                }
            break;
            }
        default:
            break;      
        }
    }

// ----------------------------------------------------
// CSpdiaAppUi::HandleResourceChangeL(TInt aType)
// Handle skin change event.
// ----------------------------------------------------

void CSpdiaAppUi::HandleResourceChangeL(TInt aType)
	{
	CAknViewAppUi::HandleResourceChangeL( aType );
	if(aType == KAknsMessageSkinChange)
		{
		iSpeedPrivate->DeleteIconArray();//for miration
		
		iSpeedPrivate->ReloadIconArray();//for miration
		iSkinChange=ETrue;
		}
	}

 // ----------------------------------------------------
// CSpdiaAppUi::IsSkinChanged()
// Returns the skin change status.
// ----------------------------------------------------
 TBool CSpdiaAppUi::IsSkinChanged()
	 {
		 return iSkinChange;
	 }
	
// ----------------------------------------------------
// CSpdiaAppUi::LaunchInfoNoteL()
// Launchs inforamtion note and exits from application
// ----------------------------------------------------
void CSpdiaAppUi::LaunchInfoNoteL()
   {
   	CAknNoteWrapper* note = new(ELeave) CAknNoteWrapper;
	note->ExecuteLD(R_PBK_NOTE_DATABASE_CORRUPTED);
	Exit();
   }
    
// ----------------------------------------------------
// CSpdiaAppUi::IsSettingType()
// ----------------------------------------------------   
TBool CSpdiaAppUi::IsSettingType()
    {
    return iSettingType;
    }
    
// End of File  
