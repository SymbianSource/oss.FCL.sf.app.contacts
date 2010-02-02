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
* Description:     A utility that provides services to both the Speeddial and
*                PhoneBook applications for getting and setting speeddial
*                number configuration.
*
*/


// INCLUDE FILES
#include <bldvariant.hrh>
#include <gulicon.h>
#include <akncontext.h>
#include <akntitle.h>
#include <AknQueryDialog.h>
#include <aknlists.h>
#include <bautils.h>
#include <aknnotedialog.h>
#include <aknnotecontrol.h>
#include <aknnotewrappers.h>
#include <sysutil.h>
#include <ErrorUI.h>
#include <vmnumber.h>
#include <centralrepository.h>
#include <SettingsInternalCRKeys.h>
#include <SpeeddialPrivateCRKeys.h>
#include <telvmbxsettingscrkeys.h>
#include <voicemailboxdomaincrkeys.h>
#include <CPbkContactEngine.h>      // Phonebook Engine
#include <CPbkContactItem.h>        // Phonebook Contact
#include <CPbkFieldInfo.h>          // Phonebook Field
#include <CPbkSingleItemFetchDlg.h>    // Phonebook Single Entry Fetch API
#include <CPbkContactChangeNotifier.h>
#include <RPbkViewResourceFile.h>  // Phonebook view dll resource file loader
#include <StringLoader.h>
#include <Spdctrl.rsg>
#include <spdctrl.mbg>
#include <avkon.mbg>
#include <gdi.h>
#include <AknIconArray.h>
#include <CPbkThumbnailManager.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include <AknsBasicBackgroundControlContext.h>
#include <AknsDrawUtils.h>
#include <AknsUtils.h>
#include <AknIconUtils.h>
#include <applayout.cdl.h>
#include <data_caging_path_literals.hrh>
#include <f32file.h>
#include <featmgr.h>
#include "SpdiaGrid.h"
#include "SpdiaGridDlg.h"
#include "SpdiaIndexData.h"
#include "SpdiaControl.h"
#include "SpdiaControl.hrh"
#include "Speeddial.laf"
#include "SpdiaPanic.h"
#include <SettingsInternalCRKeys.h>
#include "speeddial.hrh"

#include <aknViewAppUi.h>
#include <aknnavi.h> 
#include <aknnavide.h> 

// const TUint KShortcutKey0('0');  // not referenced
const TInt KDialBmpCount = 9;
const TInt KDialHindiBmpCount = 9;

const TUint KDialBmp[KDialBmpCount] =
    {
    //EMbmSpdctrlQgn_graf_quick_one,
#ifdef __SCALABLE_ICONS
    EMbmSpdctrlQgn_menu_smsvo,
#else
    EMbmSpdctrlQgn_menu_smsvo_lst,
#endif
    EMbmSpdctrlQgn_graf_quick_two,
    EMbmSpdctrlQgn_graf_quick_three,
    EMbmSpdctrlQgn_graf_quick_four,
    EMbmSpdctrlQgn_graf_quick_five,
    EMbmSpdctrlQgn_graf_quick_six,
    EMbmSpdctrlQgn_graf_quick_seven,
    EMbmSpdctrlQgn_graf_quick_eight,
    EMbmSpdctrlQgn_graf_quick_nine
    };

const TUint KDialBmpMask[KDialBmpCount] =
    {
    //EMbmSpdctrlQgn_graf_quick_one_mask_soft,
#ifdef __SCALABLE_ICONS
    EMbmSpdctrlQgn_menu_smsvo_mask,
#else
    EMbmSpdctrlQgn_menu_smsvo_lst_mask,
#endif
    EMbmSpdctrlQgn_graf_quick_two_mask,
    EMbmSpdctrlQgn_graf_quick_three_mask,
    EMbmSpdctrlQgn_graf_quick_four_mask,
    EMbmSpdctrlQgn_graf_quick_five_mask,
    EMbmSpdctrlQgn_graf_quick_six_mask,
    EMbmSpdctrlQgn_graf_quick_seven_mask,
    EMbmSpdctrlQgn_graf_quick_eight_mask,
    EMbmSpdctrlQgn_graf_quick_nine_mask
    };
    
const TUint KDialHindiBmp[KDialHindiBmpCount] =
	{
   //EMbmSpdctrlQgn_graf_quick_one,
#ifdef __SCALABLE_ICONS
    EMbmSpdctrlQgn_menu_smsvo,
#else
    EMbmSpdctrlQgn_menu_smsvo_lst,
#endif
    EMbmSpdctrlQgn_graf_deva_quick_two,
    EMbmSpdctrlQgn_graf_deva_quick_three,
    EMbmSpdctrlQgn_graf_deva_quick_four,
    EMbmSpdctrlQgn_graf_deva_quick_five,
    EMbmSpdctrlQgn_graf_deva_quick_six,
    EMbmSpdctrlQgn_graf_deva_quick_seven,
    EMbmSpdctrlQgn_graf_deva_quick_eight,
    EMbmSpdctrlQgn_graf_deva_quick_nine
  	};

const TUint KDialHindiBmpMask[KDialHindiBmpCount] =
	{
   //EMbmSpdctrlQgn_graf_quick_one,
#ifdef __SCALABLE_ICONS
    EMbmSpdctrlQgn_menu_smsvo_mask,
#else
    EMbmSpdctrlQgn_menu_smsvo_lst_mask,
#endif
    EMbmSpdctrlQgn_graf_deva_quick_two_mask,
    EMbmSpdctrlQgn_graf_deva_quick_three_mask,
    EMbmSpdctrlQgn_graf_deva_quick_four_mask,
    EMbmSpdctrlQgn_graf_deva_quick_five_mask,
    EMbmSpdctrlQgn_graf_deva_quick_six_mask,
    EMbmSpdctrlQgn_graf_deva_quick_seven_mask,
    EMbmSpdctrlQgn_graf_deva_quick_eight_mask,
    EMbmSpdctrlQgn_graf_deva_quick_nine_mask
  	}; 	
  	 
const TInt KCellRowCount(3);
const TInt KCellColCount(3);
const TInt KVoiceMail(0);
const TInt KLayoutCount(5);
const TInt KNullIndexData(-1);
const TInt32 KVmbxUid(0x100058F5) ;
const TInt KOperatorNameLength(100);
const TInt KIconsCount(13);
const TInt KIconsIdCount(11);
const TInt KIconsMaskCount(11);

_LIT(KNullCell, "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t%d\t");
_LIT(KDesTab, "\t");
_LIT(KDesTab2, "\t\t");

/// Unnamed namespace for local definitions
namespace {

CPbkContactItem* OpenContactL( 
        CPbkContactEngine* aEngine, 
        TContactItemId aId )
    {
    CPbkContactItem* item = NULL;
    TRAPD(err, item = aEngine->OpenContactL(aId));
    if (err != KErrNone)    
        {
        CCoeEnv::Static()->HandleError(err);
        }
    User::LeaveIfError(err);
    
    return item;
    }

} /// namespace


// ---------------------------------------------------------
// E32Dll(TDllReason)
// Entry point function for Symbian dll
// Returns: KErrNone: No error
// ---------------------------------------------------------
//



// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CSpdiaControl::NewL
//
// ---------------------------------------------------------
//
EXPORT_C CSpdiaControl* CSpdiaControl::NewL()
    {
    CSpdiaControl* self = new(ELeave) CSpdiaControl();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();  // self
    return self;
    }

// ---------------------------------------------------------
// CSpdiaControl::NewL
//
// ---------------------------------------------------------
//
EXPORT_C CSpdiaControl* CSpdiaControl::NewL(CPbkContactEngine& aPbkEngine)
    {
    CSpdiaControl* self = new(ELeave) CSpdiaControl(&aPbkEngine);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();  // self
    return self;
    }

// ---------------------------------------------------------
// CSpdiaControl::AssignDialNumberL
// Function called when assigning a speed dial
// from the Idle state.
// ---------------------------------------------------------
//
EXPORT_C TBool CSpdiaControl::AssignDialNumberL(
             TInt aLocation, CEikStatusPane* /*aStatusPane*/)
    {
    TBool result(EFalse);
    TInt index(Index(aLocation));

    TInt vmbxSupport = 0;
    CRepository* vmbxSupported = CRepository::NewL( KCRUidVideoMailbox );
    vmbxSupported->Get( KVideoMbxSupport, vmbxSupport );
    delete vmbxSupported;
    
    TInt vmbxPos;
    CRepository*  vmbxKey2 = CRepository::NewL( KCRUidTelVideoMailbox );
    vmbxKey2->Get( KTelVideoMbxKey, vmbxPos );
    delete vmbxKey2;
    TInt vdoIndex( Index( vmbxPos ) );
    
    if ( index != VMBoxPosition() && 
        ( !vmbxSupport || index != vdoIndex ) && 
        ( aLocation > 0 && aLocation <= iSdmCount) )
        {
        result = ETrue;
        if (ContactId(index) == KNullContactId)
            {
            // Assign it now?
            iQueryDialog = CAknQueryDialog::NewL(CAknQueryDialog::ENoTone);
            TBool resDialog(EFalse);
            TRAPD(error,
                resDialog = iQueryDialog->ExecuteLD(R_SPDIA_QUERY_ASSIGN) );
            iQueryDialog = NULL;
            User::LeaveIfError(error);
            if (!resDialog)
                {
                result = EFalse;
                }
            }

        if (result)
            {
            iPopupDialog = ETrue;
            
            result = AssignDialIndexL(index);
            
            
            }
           iPopupDialog = EFalse;
        }
    return result;
    }

// ---------------------------------------------------------
// CSpdiaControl::ExecuteLD
//
// ---------------------------------------------------------
//
EXPORT_C TInt CSpdiaControl::ExecuteLD(
             TContactItemId aId, TInt aFieldIdx)
    {
    // Live up to the "D"
    CleanupStack::PushL(this);

    // Check if the item is available now, otherwise leave happens
    // Open current contact now, in order to get the lock.
    CPbkContactItem* item = OpenContactL( iEngine, aId );
    CleanupStack::PushL(item);

    // Create a CPbkFetchEntryDlg dialog
    TInt dial;
    iGridDlg = CSpdiaGridDlg::NewL(dial, *this);

    CEikAppUi* pAppUi = NULL;
    CAknNavigationDecorator *pNaviDecorator = NULL;
    CAknTitlePane* titlePane = NULL;
    HBufC* title = NULL;
    pAppUi = CEikonEnv::Static()->EikAppUi();
    if ( pAppUi )
        {
        CEikStatusPane* StatusPane = ((CAknAppUi*) pAppUi)->StatusPane();
        TUid naviPaneUid;
        naviPaneUid.iUid = EEikStatusPaneUidNavi;
        CAknNavigationControlContainer*naviPane = (CAknNavigationControlContainer *) StatusPane->ControlL(naviPaneUid);
        pNaviDecorator = naviPane->Top();
        if ( pNaviDecorator )
            {
            pNaviDecorator->MakeVisible( EFalse );
            pNaviDecorator->DrawNow();
            }
        
        titlePane =  STATIC_CAST(CAknTitlePane*, StatusPane->ControlL( TUid::Uid(EEikStatusPaneUidTitle) ) );
        title = titlePane->Text()->AllocLC();
        }
        
    // Execute the CPbkFetchEntryDlg dialog
    TInt result(ETrue);
    TRAPD(error, result = iGridDlg->ExecuteLD(R_SPDIA_GRID_DIALOG));
    iGridDlg = NULL;
    iIconArray = NULL;

    if ( pNaviDecorator )
        {
        pNaviDecorator->MakeVisible( ETrue );
        pNaviDecorator->DrawNow();
        }
    if ( title )
        {
        titlePane->SetText( title );
        CleanupStack::Pop();  // title
        titlePane->DrawNow();
        }
        
    // Check errors
    User::LeaveIfError(error);

    // unlock the item( contact ) so that it can be removed or assigend after back from speedial grid
    // delete item;
    iEngine->CloseContactL(aId);
    CleanupStack::PopAndDestroy();  // item
    
    if (result && !CheckSpaceBelowCriticalLevelL())
        {
        AssignDialL(aId, aFieldIdx, dial);
        }
    
    // Cleanup and return
    CleanupStack::PopAndDestroy();  // this
    return (result);
    }

// ---------------------------------------------------------
// CSpdiaControl::Cancel
//
// ---------------------------------------------------------
//
EXPORT_C void CSpdiaControl::Cancel()
    {
    if ( iPbkSingleItem )
        {
        delete iPbkSingleItem;
        iPbkSingleItem = NULL;
        }
    
    if ( iQueryDialog )
        {
        delete iQueryDialog;
        iQueryDialog = NULL;	
        }
    
        iSaveDlg = ETrue;
    }

// ---------------------------------------------------------
// CSpdiaControl::DialNumberL
//
// ---------------------------------------------------------
//
EXPORT_C TBool CSpdiaControl::DialNumberL(TInt aLocation,
                TDes& aPhoneNumber, CEikStatusPane* /*aStatusPane*/)
    {
    TBool result(ETrue);
    TInt index(Index(aLocation));
    if (index == VMBoxPosition())
        {
        result = VoiceMailL(aPhoneNumber);
        }
    else
        {
        if (ContactId(index) == KNullContactId)
            {
            result = AssignDialNumberL(aLocation);
            }
        if (result)
            {
            aPhoneNumber = (*iSdmArray)[index].PhoneNumber();
            }
        }
    return result;
    }

// ---------------------------------------------------------
// CSpdiaControl::CSpdiaControl
//
// ---------------------------------------------------------
//
CSpdiaControl::CSpdiaControl()
  : iIconsCount( KIconsCount ), iIconsIdCount( KIconsIdCount  ), iIconsMaskCount( KIconsMaskCount )    
    {
    if ( !( FeatureManager::FeatureSupported( KFeatureIdCommonVoip ) ) ) 
        { 
        iIconsCount -= 1; 
        iIconsIdCount -= 1; 
        iIconsMaskCount  -= 1; 
        } 
    }
// ---------------------------------------------------------
// CSpdiaControl::InitializeArray
//
// ---------------------------------------------------------
void CSpdiaControl::InitializeArray()
    {
    iDialSkinBmp->Append( KAknsIIDQgnMenuSmsvoLst );
    iDialSkinBmp->Append( KAknsIIDQgnGrafQuickTwo );
    iDialSkinBmp->Append( KAknsIIDQgnGrafQuickThree );
    iDialSkinBmp->Append( KAknsIIDQgnGrafQuickFour );
    iDialSkinBmp->Append( KAknsIIDQgnGrafQuickFive );
    iDialSkinBmp->Append( KAknsIIDQgnGrafQuickSix );
    iDialSkinBmp->Append( KAknsIIDQgnGrafQuickSeven );
    iDialSkinBmp->Append( KAknsIIDQgnGrafQuickEight );
    iDialSkinBmp->Append( KAknsIIDQgnGrafQuickNine );

    iSkinIcons->Append( KAknsIIDQgnPropNrtypPhone );
    iSkinIcons->Append( KAknsIIDQgnPropNrtypHome );
    iSkinIcons->Append( KAknsIIDQgnPropNrtypWork );
    iSkinIcons->Append( KAknsIIDQgnPropNrtypMobile );
    iSkinIcons->Append( KAknsIIDQgnPropNrtypVideo );
    iSkinIcons->Append( KAknsIIDQgnPropNrtypFax );
    iSkinIcons->Append( KAknsIIDQgnPropNrtypPager );
    iSkinIcons->Append( KAknsIIDQgnPropNrtypCar );
    if ( FeatureManager::FeatureSupported(KFeatureIdCommonVoip) )
    	{
        iSkinIcons->Append( KAknsIIDQgnPropNrtypVoip );  
        }
    iSkinIcons->Append( KAknsIIDQgnPropNrtypAssistant );
    iSkinIcons->Append(KAknsIIDQgnPropEmpty);
    iSkinIcons->Append( KAknsIIDQgnPropNrtypEmail );
    iSkinIcons->Append( KAknsIIDQgnPropNrtypAddress );
    
    // LOCAL CONSTANTS AND MACROS
    //This order is based on 'Standard field ids' (PbkFields.hrh)
    iIcons->Append( EMbmAvkonQgn_prop_nrtyp_phone );
    iIcons->Append( EMbmAvkonQgn_prop_nrtyp_home );
    iIcons->Append( EMbmAvkonQgn_prop_nrtyp_work );
    iIcons->Append( EMbmAvkonQgn_prop_nrtyp_mobile  );
    iIcons->Append( EMbmAvkonQgn_prop_nrtyp_video  );
    iIcons->Append( EMbmAvkonQgn_prop_nrtyp_fax  );
    iIcons->Append( EMbmAvkonQgn_prop_nrtyp_pager  );
    iIcons->Append( EMbmAvkonQgn_prop_nrtyp_car  );
    if ( FeatureManager::FeatureSupported(KFeatureIdCommonVoip) )
    	{
        iIcons->Append( EMbmAvkonQgn_prop_nrtyp_voip   );
    	}
    iIcons->Append( EMbmAvkonQgn_prop_nrtyp_assistant  );
    iIcons->Append( EMbmAvkonQgn_prop_empty);
    iIcons->Append( EMbmAvkonQgn_prop_nrtyp_email  );
    iIcons->Append( EMbmAvkonQgn_prop_nrtyp_address );
    
    iIconsId->Append( EPbkqgn_prop_nrtyp_phone  );
    iIconsId->Append( EPbkqgn_prop_nrtyp_home );
    iIconsId->Append( EPbkqgn_prop_nrtyp_work );
    iIconsId->Append( EPbkqgn_prop_nrtyp_mobile  );
    iIconsId->Append( EPbkqgn_prop_nrtyp_video  );
    iIconsId->Append( EPbkqgn_prop_nrtyp_fax  );
    iIconsId->Append( EPbkqgn_prop_nrtyp_pager  );
    iIconsId->Append( EPbkqgn_prop_nrtyp_car  );
    if ( FeatureManager::FeatureSupported(KFeatureIdCommonVoip) )
    	{
    	iIconsId->Append( EPbkqgn_prop_nrtyp_voip   );
    	}
    iIconsId->Append( EPbkqgn_prop_nrtyp_assistant  );
    iIconsId->Append( EPbkqgn_prop_nrtyp_empty);
    
    iIconsMask->Append( EMbmAvkonQgn_prop_nrtyp_phone_mask );
    iIconsMask->Append( EMbmAvkonQgn_prop_nrtyp_home_mask );
    iIconsMask->Append( EMbmAvkonQgn_prop_nrtyp_work_mask );
    iIconsMask->Append( EMbmAvkonQgn_prop_nrtyp_mobile_mask  );
    iIconsMask->Append( EMbmAvkonQgn_prop_nrtyp_video_mask  );
    iIconsMask->Append( EMbmAvkonQgn_prop_nrtyp_fax_mask  );
    iIconsMask->Append( EMbmAvkonQgn_prop_nrtyp_pager_mask  );
    iIconsMask->Append( EMbmAvkonQgn_prop_nrtyp_car_mask  );
    if ( FeatureManager::FeatureSupported(KFeatureIdCommonVoip) )
    	{
    	iIconsMask->Append( EMbmAvkonQgn_prop_nrtyp_voip_mask   );
    	}
    iIconsMask->Append( EMbmAvkonQgn_prop_nrtyp_assistant_mask  );
    iIconsMask->Append( EMbmAvkonQgn_prop_empty_mask);
    }
// ---------------------------------------------------------
// CSpdiaControl::CSpdiaControl
//
// ---------------------------------------------------------
//
CSpdiaControl::CSpdiaControl( CPbkContactEngine* aPbkEngine )
    : iEngine( aPbkEngine ), iExEngine( ETrue ), iFocusedGrid( KNullIndexData ), 
    iIconsCount( KIconsCount ), iIconsIdCount( KIconsIdCount  ), iIconsMaskCount( KIconsMaskCount )  
    {
    }

// ---------------------------------------------------------
// CSpdiaControl::~CSpdiaControl
//
// ---------------------------------------------------------
//
EXPORT_C CSpdiaControl::~CSpdiaControl()
    {
    //delete iThumbFactory;
    delete iManager;
    delete iShadowLayout;

    delete iPbkNotifier;
    if ( iGridUsed == EGridNoUse )
        {
        if ( iIconArray )
            {
            iIconArray->ResetAndDestroy();
            delete iIconArray;
            }
        }
    if (!iExEngine)
        {
        delete iEngine;
        }

    ResetArray();
    delete iSdmArray;

    delete iPbkSingleItem;
    delete iQueryDialog;

    if (iPbkResourceFile != NULL)
        {
        iPbkResourceFile->Close();
        }
    delete iPbkResourceFile;

    if (iCoeEnv != NULL)
        {
        iCoeEnv->DeleteResourceFile(iFileOffset);
        }
    delete iBgContext;

    if ( iDialSkinBmp != NULL )
        {
        iDialSkinBmp->Close();
        }
    delete iDialSkinBmp;

    if ( iSkinIcons !=  NULL )
        {
        iSkinIcons->Close();
        }
    delete iSkinIcons;
    if ( iIcons !=  NULL ) 
        { 
        iIcons->Close(); 
        } 
    delete iIcons; 
       
    if ( iIconsId !=  NULL ) 
        { 
        iIconsId->Close(); 
        } 
    delete iIconsId; 
       
    if ( iIconsMask !=  NULL ) 
        { 
        iIconsMask->Close(); 
        } 
    delete iIconsMask; 
    }

// ---------------------------------------------------------
// CSpdiaControl::VoiceMailL
//
// ---------------------------------------------------------
//
EXPORT_C TBool CSpdiaControl::VoiceMailL(TDes& aNumber)
    {
    TBool result(ETrue);

    RVmbxNumber vmbx;
    CleanupClosePushL(vmbx); // So that Close() is called if a leave happens.
    if ( vmbx.Open() == KErrNone )
        {
        TInt err1 = vmbx.GetVmbxNumber( aNumber );
        TInt err2 = vmbx.GetVideoMbxNumber( aNumber );
                    
        if ( ( err1 == KErrNotFound || err1 == KErrNotSupported ) &&
             ( err2 == KErrNotFound || err2 == KErrNotSupported ))
            {
            iType = vmbx.SelectTypeL( EVmbxDefine );
            }
        else 
            {
            iType = vmbx.SelectTypeL( EVmbxCall );
            }   
        
        if( iType == EVmbxIP && !vmbx.IsVoIPProfilesFound() )
            {
            TInt err( vmbx.GetVmbxAddressL( aNumber ) );
            if ( err != KErrNone )
                {
                aNumber.Zero();
                if (err != KErrNotFound || !vmbx.QueryAddressL( EVmbxNotDefinedQuery, aNumber ) )
                    {
                    result = EFalse;
                    }
                }
            }
        else if( iType == EVmbxVideo )
            {
            TInt err( vmbx.GetVideoMbxNumber( aNumber ) );
            if ( err != KErrNone  )
                {
                aNumber.Zero();
                if ( err != KErrNotFound ||!vmbx.QueryVideoMbxNumberL( EVmbxNotDefinedQuery, aNumber) )
                    {
                    result = EFalse;
                    }
                }            
            }
        else if ( iType ==EVmbx )
            {
            TInt err( vmbx.GetVmbxNumber( aNumber ) );
            if ( err != KErrNone )
                {
                aNumber.Zero();
                if (err != KErrNotFound || !vmbx.QueryNumberL( EVmbxNotDefinedQuery, aNumber ) )
                    {
                    result = EFalse;
                    }
                }
            }
        else
            {
            result = EFalse;
            }
        }
    CleanupStack::PopAndDestroy();  // It will call vmbx.Close()
    return result;
    }

// ---------------------------------------------------------
// CSpdiaControl::PbkEngine
// phone book engine pointer
// ---------------------------------------------------------
//
EXPORT_C CPbkContactEngine* CSpdiaControl::PbkEngine() const
    {
    return iEngine;
    }

// ---------------------------------------------------------
// CSpdiaControl::Index
//
// ---------------------------------------------------------
//
EXPORT_C TInt CSpdiaControl::Index(TInt aDial) const
    {
    return ( aDial - 1 );
    }

// ---------------------------------------------------------
// CSpdiaControl::Number
// An interface for accessing index data array
// ---------------------------------------------------------
//
EXPORT_C TInt CSpdiaControl::Number(TInt aIndex) const
    {
    return (*iSdmArray)[aIndex].Number();
    }

// ---------------------------------------------------------
// CSpdiaControl::ContactId
// An interface for accessing index data array
// ---------------------------------------------------------
//
EXPORT_C TContactItemId CSpdiaControl::ContactId(TInt aIndex) const
    {
    return (*iSdmArray)[aIndex].ContactId();
    }

// ---------------------------------------------------------
// CSpdiaControl::PhoneDialNumber
// An interface for accessing index data array.
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CSpdiaControl::PhoneDialNumber(TInt aDial) const
    {
    return (*iSdmArray)[Index(aDial)].TelNumber();
    }

// ---------------------------------------------------------
// CSpdiaControl::PhoneNumber
// An interface for accessing index data array.
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CSpdiaControl::PhoneNumber(TInt aIndex) const
    {
    return (*iSdmArray)[aIndex].PhoneNumber();
    }

// ---------------------------------------------------------
// CSpdiaControl::ContactTitleL
// phone book access
// ---------------------------------------------------------
EXPORT_C HBufC* CSpdiaControl::ContactTitleL(TContactItemId aCid, TBool aUnnamed)
    {
    HBufC* title = NULL;
    if (aCid != KNullContactId)
        {
        CPbkContactItem* item =
                iEngine->ReadContactLC(aCid);
        if ((title = item->GetContactTitleOrNullL()) == NULL && aUnnamed)
            {
            title = iEngine->UnnamedTitle().AllocL();
            }
        CleanupStack::PopAndDestroy(); // item
        }

    return title;
    }

// ---------------------------------------------------------
// CSpdiaControl::ThumbIndex
// An interface for accessing index data array.
// ---------------------------------------------------------
//
EXPORT_C TInt CSpdiaControl::ThumbIndex(TInt aIndex) const
    {
    return (*iSdmArray)[aIndex].ThumbIndex();
    }

// ---------------------------------------------------------
// CSpdiaControl::IconIndex
// An interface for accessing index data array.
// ---------------------------------------------------------
//
EXPORT_C TInt CSpdiaControl::IconIndex(TInt aIndex) const
    {
    TInt index = (*iSdmArray)[aIndex].IconIndex();
    if (index >= 0)
        {
        return ( *iIconsMask )[index - iSdmCount];
        }
    return index;
    }

// ---------------------------------------------------------
// CSpdiaControl::NumberType
// An interface for accessing phone number type.
// Number types are defined in PbkFields.hrh.
// ---------------------------------------------------------
//
EXPORT_C TInt CSpdiaControl::NumberType(TInt aIndex) const
    {
    return (*iSdmArray)[aIndex].FieldId();
    }

// ---------------------------------------------------------
// CSpdiaControl::IconArray
// An interface for accessing index data array.
// ---------------------------------------------------------
//
EXPORT_C CArrayPtr<CGulIcon>* CSpdiaControl::IconArray() const
    {
    return iIconArray;
    }

// ---------------------------------------------------------
// CSpdiaControl::CreateGridDataL
//
// ---------------------------------------------------------
//
EXPORT_C TBool CSpdiaControl::CreateGridDataL(
                CAknGrid* aGrid, TBool aIndex)
    {
    iGrid = aGrid;
    if (iGridUsed == EGridNoUse)
        {
        iGridUsed = EGridUse;
        }

    // The specified index is updated.
    if (aIndex >= 0)
        {
        return UpdateIndexDataL(aIndex);
        }

    // All items are updated.
    CDesCArray* array =
        STATIC_CAST(CDesCArray*, aGrid->Model()->ItemTextArray());
    array->Reset();
    InitIndexDataL();
    if (SetIconArrayL(iIconArray) > 0)
        {
        SetOperationsL();
        }

    TKeyArrayFix key(TSpdiaIndexData::OffsetValue(
                    TSpdiaIndexData::EIndex), ECmpTInt);
    iSdmArray->Sort(key);

    TRAPD(err, SetItemDataL(*array));
    if (err)
        {
        array->Reset();
        User::Leave(err);
        }

    CFormattedCellListBoxData* list = aGrid->ItemDrawer()->FormattedCellData();
    if (list->IconArray() == NULL)
        {
        aGrid->ItemDrawer()->FormattedCellData()->SetIconArrayL(iIconArray);
        }

    return ETrue;
    }

// ---------------------------------------------------------
// CSpdiaControl::CheckingIfPopUpNeeded
// Internal rutine to check if pop up has to be shown
// to enable the One-Key-Dialling setting.
// ---------------------------------------------------------
//

EXPORT_C TBool CSpdiaControl::CheckingIfPopUpNeeded()
    {
		TBool result(ETrue);
		TRAP_IGNORE(result = CheckingIfPopUpNeededL());
		
		return result;
    }

// ---------------------------------------------------------
// CSpdiaControl::CheckingIfPopUpNeededL
// Internal rutine to check if pop up has to be shown
// to enable the One-Key-Dialling setting.
// ---------------------------------------------------------
//

TBool CSpdiaControl::CheckingIfPopUpNeededL()
    {
    TBool gridEmpty( ETrue );
    TInt  gridIndex( 1 );
    TInt value;
    TBool result( ETrue );

    CRepository* repository = CRepository::NewL( KCRUidTelephonySettings );
    repository->Get( KSettingsOneKeyDialling, value );

    if ( value == 0 )
        {
        while ( ( gridIndex < 9 ) && gridEmpty )
            {
            if ( CSpdiaControl::ContactId( gridIndex ) != KNullContactId )
                {
                gridEmpty=EFalse;
                }
            ++gridIndex;
            }
        if ( gridEmpty )
            {
            iSaveDlg = EFalse;     
            if( iSaveDlg )
                {
                delete repository;
                return EFalse;
                }

            repository->Set( KSettingsOneKeyDialling, 1 );
            }
        }
    delete repository;
    return result;
    }

// ---------------------------------------------------------
// CSpdiaControl::AssignDialIndexL
// Function called when the access is done through
// the speed dial application.
// ---------------------------------------------------------
//
EXPORT_C TBool CSpdiaControl::AssignDialIndexL(TInt aIndex)
    {
    TBool result(EFalse);
    HBufC* prompt = NULL;
    TBuf<KOperatorNameLength> operatorName;
    CAknTitlePane* title = NULL;

    if (iBooleanVarForAddingDialogVariation)
        {
        if(iPopupDialog)
            {
            if(!CSpdiaControl::CheckingIfPopUpNeeded())
                return result;
            }
        }
    else
        {   // do nothing
        }

    if(iPopupDialog)
        {
    		CEikStatusPane *sp = CEikonEnv::Static()->AppUiFactory()->StatusPane();
        title =  STATIC_CAST(CAknTitlePane*, sp->ControlL( TUid::Uid(EEikStatusPaneUidTitle) ) );
        prompt = StringLoader::LoadLC(R_QTN_SPDIA_TITLE, iCoeEnv);
        operatorName.Copy(*title->Text());
        title->SetTextL(*prompt);
        title->MakeVisible(ETrue);
        CleanupStack::PopAndDestroy(prompt); 
        }
    // Contact id before selection is saved.
    TContactItemId id(ContactId(aIndex));
    CContactDatabase::TContactViewFilter filter( CContactDatabase::EPhonable );
    
    // No filtering if VoIP is supported.
    TInt VoIPSupported( 0 );
    if ( FeatureManager::FeatureSupported( KFeatureIdCommonVoip ) )
        {
        CRepository* voiprepository = CRepository::NewL( KCRUidTelephonySettings );
        voiprepository->Get( KDynamicVoIP, VoIPSupported );
        delete voiprepository;
    
        if ( 0 < VoIPSupported )
            {
            filter = CContactDatabase::EUnfiltered; 
            }
        }
    
    CContactViewBase& filteredView = iEngine->FilteredContactsViewL( filter );        

    CPbkSingleItemFetchDlg::TParams params;
    // The array of fieldId to display is made.
    CPbkFieldIdArray* fieldIds = new (ELeave) CPbkFieldIdArray();
    CleanupStack::PushL(fieldIds);
    fieldIds->AppendL(EPbkFieldIdPhoneNumberStandard);
    fieldIds->AppendL(EPbkFieldIdPhoneNumberHome);
    fieldIds->AppendL(EPbkFieldIdPhoneNumberWork);
    fieldIds->AppendL(EPbkFieldIdPhoneNumberMobile);
		fieldIds->AppendL(EPbkFieldIdAssistantNumber);//changed 
		fieldIds->AppendL(EPbkFieldIdCarNumber);//changed
    fieldIds->AppendL(EPbkFieldIdPhoneNumberVideo);
    fieldIds->AppendL(EPbkFieldIdFaxNumber);
    fieldIds->AppendL(EPbkFieldIdPagerNumber);
    if ( VoIPSupported )
        {
        fieldIds->AppendL(EPbkFieldIdVOIP);  
        }
    params.iFieldIdArray = fieldIds;
    params.iContactView = &filteredView;
    iPbkSingleItem = CPbkSingleItemFetchDlg::NewL(params, *iEngine);

    TRAPD(error, result = iPbkSingleItem->ExecuteLD());
    iPbkSingleItem = NULL;
    User::LeaveIfError(error);

    CleanupStack::PushL(params.iContactItem);
    if (result && params.iContactItem != NULL)
        {
        result = ETrue;
        // It is already assigned from other applications.
        if (id == KNullContactId && ContactId(aIndex) != KNullContactId)
            {
            id = ContactId(aIndex);
            }

        if (result && !CheckSpaceBelowCriticalLevelL())
            {
            if (id != KNullContactId)
                {
                RemoveDialIndexL(aIndex, EFalse);
                (*iSdmArray)[aIndex].SetContactId(KNullContactId);
                }

            CPbkContactItem* item =
            iEngine->OpenContactL(params.iContactItem->Id());
            CleanupStack::PushL(item);

            TPhCltTelephoneNumber phonenumber;
            params.iContactItemField->GetTextL(phonenumber);
            CPbkFieldInfo& info = params.iContactItemField->FieldInfo();

            CPbkFieldArray& array = item->CardFields();
            for (TInt index(0); index < array.Count(); ++index)
                {
                if (array[index].ItemField().Id() ==
                        params.iContactItemField->ItemField().Id())
                    {
                    iEngine->SetFieldAsSpeedDialL(*item,
                                        index, (*iSdmArray)[aIndex].Number());
                    (*iSdmArray)[aIndex].SetContactId(params.iContactItem->Id());
                    (*iSdmArray)[aIndex].SetPhoneNumber(phonenumber);
                    (*iSdmArray)[aIndex].SetIconIndex(FindIconIndex(info.IconId()));
                    (*iSdmArray)[aIndex].SetThumbIndex(aIndex, NULL);
                    index = array.Count();
                    }
                }
            iEngine->CloseContactL(params.iContactItem->Id());
            CleanupStack::PopAndDestroy();  // item
            }
        }
    CleanupStack::PopAndDestroy(2); // params.iContactItem, fieldIds
    if(iPopupDialog)
        {
        title->SetTextL(operatorName);
        }
    return result;
    }

// ---------------------------------------------------------
// CSpdiaControl::RemoveDialIndexL
// phone book access
// ---------------------------------------------------------
//
EXPORT_C TBool CSpdiaControl::RemoveDialIndexL(TInt aIndex, TBool aConfirmation)
    {
    TBool release(ETrue);

    if (release)
        {
        TContactItemId cid((*iSdmArray)[aIndex].ContactId());
        TInt pos((*iSdmArray)[aIndex].Number());
        release = EFalse;
        if (cid != KNullContactId && (aIndex >= 0 && aIndex < iSdmCount))
            {
            iEngine->RemoveSpeedDialFieldL(cid, pos);
            (*iSdmArray)[aIndex].SetContactId(KNullContactId);
            release = ETrue;
            }
        }
    return release;
    }

// ---------------------------------------------------------
// deprecated
//
// ---------------------------------------------------------
//
EXPORT_C TBool CSpdiaControl::AssignNumberL(TInt /*aIndex*/,
                                    CEikStatusPane* /*aStatusPane*/,
                                    CCoeControl* /*aContainer*/,
                                    TBool /*aConfirmation*/)
    {
    __ASSERT_DEBUG(0, Panic(ESdmPanicNotSupported));
    return EFalse;
    }

EXPORT_C void CSpdiaControl::ApplyCellSpdiaPaneStyleL(
                        CAknGrid& /*aListBox*/, TSize& /*aSize*/)
    {
    __ASSERT_DEBUG(0, Panic(ESdmPanicNotSupported));
    }

EXPORT_C void CSpdiaControl::DrawShadow(
    CWindowGc& /*aGc*/,
    const TSize& /*aSize*/ )
    {
    __ASSERT_DEBUG(0, Panic(ESdmPanicNotSupported));
    }

// ---------------------------------------------------------
// CSpdiaContainer::SetLayout
//
// ---------------------------------------------------------
//
EXPORT_C void CSpdiaControl::SetLayout(const TRect& aRect)
    {
    //Scalable UI.
    if ( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
        {
        TRect			   	 mainPaneRect ;
        TRect 			   	 statusPaneRect;
        TAknLayoutRect     	 mainQdialRect;
        TAknLayoutRect     	 gridQdialPaneRect;
        TAknLayoutRect     	 cellQdialPaneRect;
        TAknWindowLineLayout cellQdialPane;
        TInt 				 varient =0;
			
        if ( Layout_Meta_Data::IsLandscapeOrientation() )
            {
            AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,mainPaneRect);
            AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EStatusPane,statusPaneRect);
            mainPaneRect.iTl = statusPaneRect.iTl;
            mainPaneRect.iBr.iY = mainPaneRect.iBr.iY - statusPaneRect.iTl.iY;
            varient =1;
            if ( AknLayoutUtils::PenEnabled() )
                {
                mainPaneRect.iTl -= statusPaneRect.iTl;
                mainPaneRect.iBr.iY +=statusPaneRect.iTl.iY;
                }
            }
        else
            {
            AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,mainPaneRect);
            AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EStatusPane,statusPaneRect);
            mainPaneRect.iTl= statusPaneRect.iTl;
            if ( AknLayoutUtils::PenEnabled() )
                {
                mainPaneRect.iTl -= statusPaneRect.iTl;
                }
            }
		
        SetRect( mainPaneRect );
		
        mainQdialRect.LayoutRect(mainPaneRect,AknLayoutScalable_Apps::main_qdial_pane().LayoutLine());
        gridQdialPaneRect.LayoutRect(mainQdialRect.Rect(),AknLayoutScalable_Apps::grid_qdial_pane().LayoutLine());
        cellQdialPaneRect.LayoutRect(gridQdialPaneRect.Rect(),AknLayoutScalable_Apps::cell_qdial_pane(0,0).LayoutLine());
        cellQdialPane = AknLayoutScalable_Apps::cell_qdial_pane_g4(varient).LayoutLine();
			
        TInt Cnt = 0;
			
        for(TInt rowSize=0;rowSize < 3; rowSize++)
            {
            for(TInt column=0;column < 3; column++)
                {
                icellLayout[Cnt].LayoutRect(gridQdialPaneRect.Rect(),AknLayoutScalable_Apps::cell_qdial_pane(column,rowSize).LayoutLine());	
                Cnt ++ ;	
                }
            }
				

        (*iShadowLayout)[CSpdiaGrid::EtnCIF].LayoutRect(cellQdialPaneRect.Rect(),cellQdialPane
	            /*sdm_tn_shadow_cif_c,
	            sdm_tn_shadow_cif_l,
	            sdm_tn_shadow_cif_t,
	            sdm_tn_shadow_cif_r,
	            sdm_tn_shadow_cif_b,
	            sdm_tn_shadow_cif_w,
	            sdm_tn_shadow_cif_h*/);

        (*iShadowLayout)[CSpdiaGrid::EtnVGA90].LayoutRect(cellQdialPaneRect.Rect(),cellQdialPane
	            /*sdm_tn_shadow_vga90_c,
	            sdm_tn_shadow_vga90_l,
	            sdm_tn_shadow_vga90_t,
	            sdm_tn_shadow_vga90_r,
	            sdm_tn_shadow_vga90_b,
	            sdm_tn_shadow_vga90_w,
	            sdm_tn_shadow_vga90_h*/);

        (*iShadowLayout)[CSpdiaGrid::EtnVGA].LayoutRect(cellQdialPaneRect.Rect(),cellQdialPane
	            /*sdm_tn_shadow_vga_c,
	            sdm_tn_shadow_vga_l,
	            sdm_tn_shadow_vga_t,
	            sdm_tn_shadow_vga_r,
	            sdm_tn_shadow_vga_b,
	            sdm_tn_shadow_vga_w,
	            sdm_tn_shadow_vga_h*/);

        (*iShadowLayout)[CSpdiaGrid::EtnCOM].LayoutRect(cellQdialPaneRect.Rect(),cellQdialPane
	            /*sdm_tn_shadow_com_c,
	            sdm_tn_shadow_com_l,
	            sdm_tn_shadow_com_t,
	            sdm_tn_shadow_com_r,
	            sdm_tn_shadow_com_b,
	            sdm_tn_shadow_com_w,
	            sdm_tn_shadow_com_h*/);         
        }
    else
        {
        TAknWindowLineLayout lScr = AknLayout::screen();
        TAknWindowLineLayout lCtrlPane = AknLayout::control_pane(TRect( TPoint(lScr.il, lScr.it), TSize(lScr.iW, lScr.iH) ));
        SetRect(TRect( TPoint(lScr.il, lScr.it), TSize(lScr.iW, lScr.iH - lCtrlPane.iH) ) );

        (*iShadowLayout)[CSpdiaGrid::EtnCIF90].LayoutRect(aRect,AppLayout::Cell_pane_elements__qdial__Line_1()
            /*sdm_tn_shadow_cif90_c,
            sdm_tn_shadow_cif90_l,
            sdm_tn_shadow_cif90_t,
            sdm_tn_shadow_cif90_r,
            sdm_tn_shadow_cif90_b,
            sdm_tn_shadow_cif90_w,
            sdm_tn_shadow_cif90_h*/);
        (*iShadowLayout)[CSpdiaGrid::EtnCIF].LayoutRect(aRect,AppLayout::Cell_pane_elements__qdial__Line_4()
            /*sdm_tn_shadow_cif_c,
            sdm_tn_shadow_cif_l,
            sdm_tn_shadow_cif_t,
            sdm_tn_shadow_cif_r,
            sdm_tn_shadow_cif_b,
            sdm_tn_shadow_cif_w,
            sdm_tn_shadow_cif_h*/);
        (*iShadowLayout)[CSpdiaGrid::EtnVGA90].LayoutRect(aRect,AppLayout::Cell_pane_elements__qdial__Line_7()
            /*sdm_tn_shadow_vga90_c,
            sdm_tn_shadow_vga90_l,
            sdm_tn_shadow_vga90_t,
            sdm_tn_shadow_vga90_r,
            sdm_tn_shadow_vga90_b,
            sdm_tn_shadow_vga90_w,
            sdm_tn_shadow_vga90_h*/);
        (*iShadowLayout)[CSpdiaGrid::EtnVGA].LayoutRect(aRect,AppLayout::Cell_pane_elements__qdial__Line_10()
            /*sdm_tn_shadow_vga_c,
            sdm_tn_shadow_vga_l,
            sdm_tn_shadow_vga_t,
            sdm_tn_shadow_vga_r,
            sdm_tn_shadow_vga_b,
            sdm_tn_shadow_vga_w,
            sdm_tn_shadow_vga_h*/);
        (*iShadowLayout)[CSpdiaGrid::EtnCOM].LayoutRect(aRect,AppLayout::Cell_pane_elements__qdial__Line_13()
            /*sdm_tn_shadow_com_c,
            sdm_tn_shadow_com_l,
            sdm_tn_shadow_com_t,
            sdm_tn_shadow_com_r,
            sdm_tn_shadow_com_b,
            sdm_tn_shadow_com_w,
            sdm_tn_shadow_com_h*/);
		    }
    }

// ---------------------------------------------------------
// deprecated
//
// ---------------------------------------------------------
//
EXPORT_C TInt CSpdiaControl::Size() const
    {
    __ASSERT_DEBUG(0, Panic(ESdmPanicNotSupported));
    return KCellRowCount;
    }

// ---------------------------------------------------------
// CSpdiaControl::VMBoxPosition
//
// ---------------------------------------------------------
//
EXPORT_C TInt CSpdiaControl::VMBoxPosition() const
    {
    return KVoiceMail;
    }

// ---------------------------------------------------------
// CSpdiaControl::SpdIconIndex
// An interface for accessing index data array.
// ---------------------------------------------------------
//
EXPORT_C TInt CSpdiaControl::SpdIconIndex(TInt aIndex) const
    {
    return (*iSdmArray)[aIndex].IconIndex();
    }

// ---------------------------------------------------------
// CSpdiaControl::CreateDataL
// Set grid used flag and calls CreateDataL.
// ---------------------------------------------------------
//
TBool CSpdiaControl::CreateDataL(const CAknGrid& aGrid)
    {
    iGridUsed = EGridUseAndUpdate;

    return CreateGridDataL(CONST_CAST(CAknGrid*, &aGrid), KNullIndexData);
    }

// ---------------------------------------------------------
// CSpdiaControl::PbkThumbnailGetComplete
//
// ---------------------------------------------------------
//
void CSpdiaControl::PbkThumbnailGetComplete
        (MPbkThumbnailOperation& aOperation, CFbsBitmap* aBitmap)
   {
   TRAP_IGNORE(PbkThumbnailGetCompleteL(aOperation, aBitmap )); 	
   }
   
// ---------------------------------------------------------
// CSpdiaControl::PbkThumbnailGetCompleteL
//
// ---------------------------------------------------------
//
void CSpdiaControl::PbkThumbnailGetCompleteL
        (MPbkThumbnailOperation& aOperation, CFbsBitmap* aBitmap)
    {
    TInt index(HasOperation(aOperation));
    if ( index != KNullIndexData )
        {
        if ( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
            {
            CAknIcon* icon = CAknIcon::NewL();
            icon->SetBitmap(aBitmap);

            // create mask
            CFbsBitmap* mask = new (ELeave) CFbsBitmap();

            TSize size = aBitmap->SizeInPixels();
            mask->Create(size,EGray2);

            CFbsBitGc* fbsBitGc = CFbsBitGc::NewL();
            CleanupStack::PushL( fbsBitGc );
            CFbsBitmapDevice* bmpDevice = CFbsBitmapDevice::NewL( mask );
            CleanupStack::PushL( bmpDevice );
            bmpDevice->Resize( size );
            fbsBitGc->Activate( bmpDevice );

            fbsBitGc->SetPenStyle(CGraphicsContext::ENullPen);
            fbsBitGc->SetBrushStyle(CGraphicsContext::ESolidBrush);

            fbsBitGc->SetBrushColor(KRgbBlack); 
            fbsBitGc->DrawRect( TRect( size ) );

            CleanupStack::PopAndDestroy( 2, fbsBitGc ); // bmpDevice and fbsBitGc

            icon->SetMask(mask);

            // this creates both scalable icon and mask
            CAknIcon* scaledIcon = AknIconUtils::CreateIconL(icon);

            TSize thumbnailSize = GetThumbnailSize();

            AknIconUtils::SetSize(scaledIcon->Bitmap(),thumbnailSize,EAspectRatioPreservedAndUnusedSpaceRemoved);
            AknIconUtils::SetSize(scaledIcon->Mask(),thumbnailSize,EAspectRatioPreservedAndUnusedSpaceRemoved);

            CFbsBitmap* bmp = new (ELeave) CFbsBitmap();
            CFbsBitmap* msk = new (ELeave) CFbsBitmap();
            TInt err = msk->Duplicate( scaledIcon->Mask()->Handle() ); 
            TInt err1 = bmp->Duplicate( scaledIcon->Bitmap()->Handle() );			

			
            delete (*iIconArray)[index];
            (*iIconArray)[index] = NULL;
            (*iIconArray)[index] = CGulIcon::NewL(bmp,msk);
            // set the text array of grid
            (*iSdmArray)[index].SetThumbIndex(index,bmp);
            ChangeIndexDataL(index, (*iSdmArray)[index]);
            if ((*iSdmArray)[index].LastThumb())
                {
                DrawNow();
                iGrid->DrawNow(); 
                }			
            delete scaledIcon;
            }       
        else
            {
            // set the scaled bitmap to the array of icons
            delete (*iIconArray)[index];
            (*iIconArray)[index] = NULL;
            (*iIconArray)[index] = CGulIcon::NewL(aBitmap);
            // set the text array of grid
            (*iSdmArray)[index].SetThumbIndex(index, aBitmap);
            ChangeIndexDataL(index, (*iSdmArray)[index]);
            if ((*iSdmArray)[index].LastThumb())
                {
                DrawNow();
                iGrid->DrawNow();
                }
            }
        delete &aOperation;
        (*iSdmArray)[index].SetOperation(NULL);
        delete (*iSdmArray)[index].ContactItem();
        (*iSdmArray)[index].SetContactItem(NULL);
        }
    }

// ---------------------------------------------------------
// CSpdiaControl::PbkThumbnailGetFailed
//
// ---------------------------------------------------------
//
void CSpdiaControl::PbkThumbnailGetFailed
        (MPbkThumbnailOperation& aOperation,
        TInt /*aError*/)
    {
    TInt index(HasOperation(aOperation));
    if (index != KNullIndexData)
        {
        delete &aOperation;
        (*iSdmArray)[index].SetOperation(NULL);
        delete (*iSdmArray)[index].ContactItem();
        (*iSdmArray)[index].SetContactItem(NULL);
        }
    }

// ---------------------------------------------------------
// CSpdiaContainer::Draw
//
// ---------------------------------------------------------
//
void CSpdiaControl::Draw(const TRect& /*aRect*/) const
    {
    CWindowGc& gc = SystemGc();
//    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
//    gc.Clear();

   AknsDrawUtils::Background(
       AknsUtils::SkinInstance(),
       AknsDrawUtils::ControlContext(this),
       this,
       gc,
       Rect() );

    DrawShadow(gc);
    }

// ---------------------------------------------------------
// CSpdiaContainer::HandleDatabaseEventL
// Handles an database event of type aEventType.
// ---------------------------------------------------------
//
void CSpdiaControl::HandleDatabaseEventL(
         TContactDbObserverEvent /*aEvent*/)
    {
    // An update flag will be updated if the grid is used.
    // Other than this, index data is updated.
    if (iGridUsed != EGridUse)
        {
        InitIndexDataL();
        if ( iGridDlg )
            {
            CreateGridDataL( ( CAknGrid* )iGrid , KNullIndexData );
            iGrid->DrawNow();
            
            // The grid from 2 to 9 has the possibility to show the dialog,
            // and the related index is the value from 1 to 8.
            if ( iFocusedGrid != KNullIndexData 
                && ( iFocusedGrid >= 1 && iFocusedGrid < 9 ) 
                && iQueryDialog  )
                {
                TContactItemId id( ( *iSdmArray )[iFocusedGrid].ContactId() );          	
                if ( id == KNullContactId )
                    {
                    delete iQueryDialog;
                    iQueryDialog = NULL;
                    }
                }
            }
        }
    }

// ---------------------------------------------------------
// CSpdiaControl::ConstructL
//
// ---------------------------------------------------------
//
void CSpdiaControl::ConstructL()
    {
    TInt tmp = 1;

   	_LIT(KMbmFileName, "Z:spdctrl.mbm");
    _LIT(KRscFileName, "Z:spdctrl.rsc" );

    TParse* fpMbm = new(ELeave) TParse ();
    fpMbm->Set (KMbmFileName, &KDC_APP_BITMAP_DIR, NULL);
    iBmpPath.Copy(fpMbm ->FullName());
    delete fpMbm;
    fpMbm=NULL;

    TParse* fpRsc = new(ELeave) TParse ();
    fpRsc->Set (KRscFileName, &KDC_RESOURCE_FILES_DIR, NULL);
    iRscPath.Copy(fpRsc ->FullName());
    delete fpRsc;
    fpRsc=NULL;

    CRepository* repository = CRepository::NewLC(KCRUidSpeedDialLV);
    repository->Get( KSpeedDialLVFlags, tmp );

    //variable that will be used to variate the code.
    // if 0 release is 2.1 and code shouldnt be used
    // if 1 release is 2.x (where x:5, 6, ... )and the dialog should be used.

    iBooleanVarForAddingDialogVariation  = ( tmp == 1);
	
    CleanupStack::PopAndDestroy();

    if (iEngine == NULL)
        {
        TRAPD(error,iEngine = CPbkContactEngine::NewL());
        iError = error;
        if(error !=KErrNone)   
            {
            delete iEngine ;	
            iEngine = NULL;
            return ;	
            }
        }
      
    if ( iEngine )    
        {
        iPbkNotifier = iEngine->CreateContactChangeNotifierL(this);
        }
    iPbkResourceFile = new (ELeave) RPbkViewResourceFile(*iCoeEnv);
    if (!iPbkResourceFile->IsOpen())
        {
        iPbkResourceFile->OpenL();
        }

    TFileName fileName(iRscPath);
    BaflUtils::NearestLanguageFile(iCoeEnv->FsSession(), fileName);
    iFileOffset = iCoeEnv->AddResourceFileL(fileName);

    iSdmCount = KCellRowCount * KCellColCount;
    iGridUsed = EGridNoUse;

    iIconArray = new (ELeave)CAknIconArray(iSdmCount);
    iSdmArray = new (ELeave) CArrayFixFlat<TSpdiaIndexData> (iSdmCount);

    iDialSkinBmp = new (ELeave)RArray<TAknsItemID>(KArraySize);
    iSkinIcons = new (ELeave)RArray<TAknsItemID>(KArraySize);
    iIcons = new (ELeave)RArray<TInt>(iIconsCount);
    iIconsId = new (ELeave)RArray<TInt>(iIconsIdCount);
    iIconsMask = new (ELeave)RArray<TInt>(iIconsMaskCount);
    iShadowLayout = new (ELeave) CArrayFixFlat<TAknLayoutRect> (KLayoutCount);
    for (TInt i(0); i < KLayoutCount; ++i)
        {
        TAknLayoutRect rect;
        iShadowLayout->AppendL(rect);
        }

    InitIndexDataL();
    InitializeArray();

    // create skin context
    // Size is set in SizeChanged()
    iBgContext = CAknsBasicBackgroundControlContext::NewL(
        KAknsIIDQsnBgAreaMainQdial,
        TRect(0,0,0,0),
        EFalse );
        iPopupDialog = EFalse;

    iType = EVmbx;
    }

// ---------------------------------------------------------
// CSpdiaControl::InitIndexDataL
// The SDM control data is initialized.
// ---------------------------------------------------------
//
void CSpdiaControl::InitIndexDataL()
    {
    // The dialog of grid is not closed.
    if (iIconArray != NULL)
        {
        ResetArray();
        iSdmArray->Reset();
        for (TInt i(0); i < iSdmCount; ++i)
            {
            TSpdiaIndexData data;
            SetIndexDataL(i, data);
            iSdmArray->AppendL(data);
            }
        }
    }

// ---------------------------------------------------------
// CSpdiaControl::ThumbType
// An interface for accessing index data array.
// ---------------------------------------------------------
//
TInt CSpdiaControl::ThumbType(TInt aIndex) const
    {
    return ConvThumbType((*iSdmArray)[aIndex].ThumbSize());
    }

// ---------------------------------------------------------
// CSpdiaControl::ThumbType
// An interface for accessing index data array.
// ---------------------------------------------------------
//
TSize CSpdiaControl::ThumbSize(TInt aIndex) const
    {
    return (*iSdmArray)[aIndex].ThumbSize();
    }


// ---------------------------------------------------------
// CSpdiaControl::IconPositionL
// phone book access
// ---------------------------------------------------------
//
TInt CSpdiaControl::IconPositionL(const CPbkContactItem& aItem, TInt aDial, TInt& aFieldId)
    {
    TInt index(KNullIndexData);
    CPbkFieldArray& fields = aItem.CardFields();
    for (TInt fieldIdx(0); index == KNullIndexData &&
                    fieldIdx < fields.Count(); ++fieldIdx)
        {
        TPbkContactItemField& field = fields[fieldIdx];
        CArrayFix<TInt>* array = field.SpeedDialIndexesL();
        if (array)
            {
            for (TInt i(0); index < 0 && i < array->Count(); ++i)
                {
                if (aDial == (*array)[i])
                    {
                    index = FindIconIndex(field.FieldInfo().IconId());
                    aFieldId = field.FieldInfo().FieldId();
                    }
                }
            delete array;
            }
        }
    return index;
    }

// ---------------------------------------------------------
// CSpdiaControl::FindIconIndex
//
// ---------------------------------------------------------
//
TInt CSpdiaControl::FindIconIndex(TInt aId) const
    {
    TInt index(KNullIndexData);
    for (TInt n(0); index == KNullIndexData && n < iIconsIdCount; ++n)
        {
        if ( ( *iIconsId )[n] == ( TUint )aId )
            {
            index = n + iSdmCount;
            }
        }
    return index;
    }

// ---------------------------------------------------------
// CSpdiaControl::DeleteIconArray
// Deletes the old icons.
// ---------------------------------------------------------
EXPORT_C void CSpdiaControl::DeleteIconArray()
    {
    if( iIconArray )
        {
        iIconArray->ResetAndDestroy();
        delete iIconArray;
        iIconArray = NULL;
        }
    }

// ---------------------------------------------------------
// CSpdiaControl::ReloadIconArray
// Reloads the new skin icons.
// ---------------------------------------------------------
EXPORT_C void CSpdiaControl::ReloadIconArray()
    {
		TRAP_IGNORE(ReloadIconArrayL());
    }
// ---------------------------------------------------------
// CSpdiaControl::ReloadIconArrayL
// Reloads the new skin icons.
// ---------------------------------------------------------
void CSpdiaControl::ReloadIconArrayL()
    {
    if ( NULL == iIconArray )
        {
        iIconArray = new (ELeave)CAknIconArray(iSdmCount);
        }
    SetIconArrayL(iIconArray);
    }
	
// ---------------------------------------------------------
// CSpdiaControl::SetIconArrayL
//
// ---------------------------------------------------------
//
TInt CSpdiaControl::SetIconArrayL(CArrayPtrFlat<CGulIcon>* aArray)
    {
    CFbsBitmap* bitmap,*mask;
    bitmap = NULL;
    mask= NULL;
    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
    if (!iManager)
        {
        iManager = CPbkThumbnailManager::NewL(*iEngine);
        }

    if ( aArray != NULL )
        {
        aArray->ResetAndDestroy();
        }
    else
        {
        return 0;
        }
	
    TInt vmbxSupport = 0;
    CRepository* vmbxSupported = CRepository::NewL( KCRUidVideoMailbox );
    vmbxSupported->Get( KVideoMbxSupport, vmbxSupport );
    delete vmbxSupported;
    
    TInt vmbxPos;
    CRepository*  vmbxKey2 = CRepository::NewL( KCRUidTelVideoMailbox );
    vmbxKey2->Get( KTelVideoMbxKey, vmbxPos );
    delete vmbxKey2;
    TInt vdoIndex( Index( vmbxPos ) );    
    
    TInt existThumb(0);
    for (TInt index(0); index < iSdmArray->Count(); index++)
        {
        if ( index == 0 )
            {
            TUid uidVmbx;
            uidVmbx.iUid =KVmbxUid;
            AknsUtils::CreateAppIconLC( skinInstance,uidVmbx, EAknsAppIconTypeContext,bitmap ,mask);		
            CleanupStack::Pop(2);
            }
        else if ( index == 1 && ( vmbxSupport && vdoIndex == 1 ) )
            {
            AknsUtils::CreateIconL( skinInstance, (*iDialSkinBmp)[index], bitmap, mask, 
                iBmpPath, EMbmSpdctrlQgn_prop_video_mb, EMbmSpdctrlQgn_prop_video_mb_mask );
            }
        else
            {
            if( User::Language()== ELangHindi)
                {
                AknsUtils::CreateIconL(skinInstance, (*iDialSkinBmp)[index],bitmap,mask,iBmpPath,KDialHindiBmp[index],  KDialHindiBmpMask[index] );	
                }
            else
                {
                AknsUtils::CreateIconL(skinInstance, (*iDialSkinBmp)[index],bitmap,mask,iBmpPath,KDialBmp[index],  KDialBmpMask[index] );	
                }
            }
        CGulIcon* icon = CGulIcon::NewL(bitmap,mask);
        CleanupStack::PushL(icon);
        aArray->AppendL(icon);

        if (CreateIndexIconL(index,
            (*iSdmArray)[index], existThumb > 0? EFalse: ETrue))
            {
            ++existThumb;
            }
        CleanupStack::Pop(); // icon
        }

    // Add icons
    for (TInt i(0); i < iIconsIdCount; ++i)
        {
        AknsUtils::CreateIconL(skinInstance, (*iSkinIcons)[i],bitmap,mask,AknIconUtils::AvkonIconFileName(),( *iIcons )[i], ( *iIconsMask )[i] );
        aArray->AppendL(CGulIcon::NewL(bitmap,mask));
        }

    return existThumb;
    }

// ---------------------------------------------------------
// CSpdiaContainer::ItemDescriptorL
//
// ---------------------------------------------------------
//
HBufC* CSpdiaControl::ItemDescriptorLC(TInt aIndex,
                            const TSpdiaIndexData& aSdmData)
    {
    HBufC* des = NULL;
    if (aSdmData.ContactId() != KNullContactId)
        {
        des = CreateItemDescriptorLC(aSdmData);
        }
    else
        {
        des = HBufC::NewLC(KNullCell.iTypeLength);
        des->Des().Format(KNullCell, aIndex);
        }
    return des;
    }

// ---------------------------------------------------------
// CSpdiaContainer::CreateItemDescriptorL
//
// ---------------------------------------------------------
//
HBufC* CSpdiaControl::CreateItemDescriptorLC(const TSpdiaIndexData& aSdmData)
    {
    HBufC* des = HBufC::NewLC(KApaMaxAppCaption);
    TPtr ptr(des->Des());

    // thumb type
    TInt type(CSpdiaGrid::EText);
    if (aSdmData.ThumbIndex() >= 0)
        {
        type = ConvThumbType(aSdmData.ThumbSize());
        }

    // Text
    if (type == CSpdiaGrid::EText)
        {
        ptr.Append(KDesTab);       // 1
        if (aSdmData.IconIndex() >= 0)
            {
            ptr.AppendNum(aSdmData.IconIndex());
            }
        AppendTextL(aSdmData, ptr); //  2,3,4
        }
    else
        {
        // icon + text + text + text
        ptr.Append(KDesTab2);       // 1, 2
        ptr.Append(KDesTab2);       // 2
        }

    // thumbnail 5-14
    for (TInt i(CSpdiaGrid::EtnCIF90); i <= CSpdiaGrid::EtnCOM; ++i)
        {
        AppendThumbnail(aSdmData, ptr, (type == i? ETrue: EFalse));
        }

    // number
    ptr.Append(KDesTab);        // 15
    // check
    ptr.Append(KDesTab);        // 16

    return des;
    }

// ---------------------------------------------------------
// CSpdiaContainer::AppendTextL
// output Text1\tText2\tText3\t
// ---------------------------------------------------------
//
void CSpdiaControl::AppendTextL(const TSpdiaIndexData& aSdmData, TPtr& aText)
    {
    CPbkContactItem* item = iEngine->ReadContactLC(aSdmData.ContactId());
    if (item == NULL)
        {
        aText.Append(KDesTab2);
        aText.Append(KDesTab2);
        }
    else
        {
        TPbkContactItemField* lastNameField =
            FindFieldL(*item, EPbkFieldIdLastName);     // Last Name
        TPbkContactItemField* firstNameField  =
            FindFieldL(*item, EPbkFieldIdFirstName);  // First Name
        TPbkContactItemField* companyNameField =
            FindFieldL(*item, EPbkFieldIdCompanyName);

        aText.Append(KDesTab);             // 2
        if (!lastNameField && !firstNameField && !companyNameField)
            {
            aText.Append(aSdmData.PhoneNumber());
            aText.Append(KDesTab);         // 3
            }
        else if (!lastNameField && !firstNameField)
            {
            aText.Append(companyNameField->Text());
            aText.Append(KDesTab);         // 3
            }
        else
            {
            if (lastNameField)
                {
                aText.Append(lastNameField->Text());
                aText.Append(KDesTab);     // 3
                }
            if (firstNameField)
                {
                aText.Append(firstNameField->Text());
                }
            if (!lastNameField)
                {
                aText.Append(KDesTab);     // 3
                }
            }
        aText.Append(KDesTab);             // 4
        }
    CleanupStack::PopAndDestroy(); // item
    }

// ---------------------------------------------------------
// CSpdiaContainer::FindFieldL
//
// ---------------------------------------------------------
//
TPbkContactItemField* CSpdiaControl::FindFieldL(
              CPbkContactItem& aItem, TPbkFieldId aFieldId)
    {
    TPbkContactItemField* field = aItem.FindField(aFieldId);
    if (field)
        {
        HBufC* buf = field->Text().AllocL();
        buf->Des().Trim();
        if (buf->Des().Length() == 0)
            {
            field = NULL;
            }
        delete buf;
        }
    return field;
    }
// ---------------------------------------------------------
// CSpdiaContainer::AppendThumbnail
// output \tThumbnail\ticon
// ---------------------------------------------------------
//
void CSpdiaControl::AppendThumbnail(
    const TSpdiaIndexData& aSdmData,
    TPtr& aText,
    TBool aFixedLocation )
    {
    if (!aFixedLocation)
        {
        aText.Append(KDesTab2);
        return;
        }

    aText.Append(KDesTab);
    if (aSdmData.ThumbIndex() >= 0)
        {
        aText.AppendNum(aSdmData.ThumbIndex());
        }

    aText.Append(KDesTab);
    if (aSdmData.IconIndex() >= 0)
        {
        aText.AppendNum(aSdmData.IconIndex());
        }
    }

// ---------------------------------------------------------
// CSpdiaContainer::ConvThumbType
// Returns
//      EtnVGA  : 640 x 480 40 x 30 VGA
//      EtnVGA90: 480 x 640 30 x 40 VGA turned
//      EtnCIF  : 352 x 288 44 x 36 CIF
//      EtnCIF90: 288 x 352 36 x 44 CIF turned
//      EtnCOM  : othre
// ---------------------------------------------------------
//
TInt CSpdiaControl::ConvThumbType(const TSize& aSize) const
    {
    // Scalable UI
    TAknWindowLineLayout  vga;
    TAknWindowLineLayout  cif;
    if( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
        {
        vga = AknLayoutScalable_Apps::popup_phob_thumbnail_window_g2().LayoutLine();
        cif = AknLayoutScalable_Apps::popup_phob_thumbnail_window_g1().LayoutLine();//image
        }
    else
        {
        vga = AppLayout::Thumbnail_pop_up_window_elements_Line_1();
        cif = AppLayout::Thumbnail_pop_up_window_elements_Line_3();
        }
    TInt type(CSpdiaGrid::EtnCOM);

    if (aSize.iWidth == cif.iH//SDM_TN_CIF_HEIGHT
            && aSize.iHeight == cif.iW)// SDM_TN_CIF_WIDTH)
        {
        type = CSpdiaGrid::EtnCIF90;
        }
    else if (aSize.iWidth == cif.iW &&//SDM_TN_CIF_WIDTH &&
                    aSize.iHeight ==  cif.iH)//SDM_TN_CIF_HEIGHT)
        {
        type = CSpdiaGrid::EtnCIF;
        }
    else if (aSize.iWidth == vga.iH &&//SDM_TN_VGA_HEIGHT &&
                    aSize.iHeight == vga.iW )//SDM_TN_VGA_WIDTH)
        {
        type = CSpdiaGrid::EtnVGA90;
        }
    else if (aSize.iWidth == vga.iW &&//SDM_TN_VGA_WIDTH &&
                    aSize.iHeight == vga.iH)//SDM_TN_VGA_HEIGHT)
        {
        type = CSpdiaGrid::EtnVGA;
        }

    return type;
    }


// ---------------------------------------------------------
// CSpdiaContainer::SetOperationsL
//
// ---------------------------------------------------------
//
void CSpdiaControl::SetOperationsL()
    {
    for (TInt index(iSdmArray->Count() -1); index > 0; --index)
        {
        TSpdiaIndexData& data = (*iSdmArray)[index];
        if (data.ContactItem() != NULL)
            {
            data.SetOperation(
                    iManager->GetThumbnailAsyncL(*this, *data.ContactItem()));
            }
        }
    }

// ---------------------------------------------------------
// CSpdiaContainer::SetItemDataL
//
// ---------------------------------------------------------
//
void CSpdiaControl::SetItemDataL(CDesCArray& aArray)
    {
    for (TInt loop(0); loop < iSdmCount; loop++)
        {
        TSpdiaIndexData& data = (*iSdmArray)[loop];

        aArray.AppendL(ItemDescriptorLC(loop, data)->Des());
        CleanupStack::PopAndDestroy(); // des
        }
    }


// ---------------------------------------------------------
// CSpdiaContainer::AssignDialL
// Function called when assigning speed dial from phone book
// application
// ---------------------------------------------------------
//
void CSpdiaControl::AssignDialL(TContactItemId aId, TInt aFieldIdx, TInt aDial)
    {
    if (iBooleanVarForAddingDialogVariation)
        {
        if(!CSpdiaControl::CheckingIfPopUpNeeded())
            return;
        }
    else
        {   // do nothing
        }

    TInt index(Index(aDial));
    TPhCltTelephoneNumber phonenumber;
    TContactItemId id(ContactId(index));
    
    if (id != KNullContactId)
        {
        RemoveDialIndexL(index, EFalse);
        }
    
    CPbkContactItem* item = OpenContactL( iEngine, aId );
    CleanupStack::PushL(item);

    CPbkFieldArray& fields = item->CardFields();
    TInt fieldCount = fields.Count();   
    
    if ( aFieldIdx < fieldCount )
        {
        TPbkContactItemField& field = fields[aFieldIdx];
        field.GetTextL(phonenumber); 
        phonenumber.Trim();
        iEngine->SetFieldAsSpeedDialL(*item, aFieldIdx, aDial); 
		
        iEngine->CloseContactL(aId);
        CleanupStack::PopAndDestroy();  // item
	
        HBufC* prompt =
            StringLoader::LoadLC(R_SPDIA_TEXT_KEY_ASSIGNED, aDial, iCoeEnv);
        CAknConfirmationNote* note = new (ELeave)CAknConfirmationNote();
        note->ExecuteLD(*prompt);
        CleanupStack::PopAndDestroy(); // prompt
        }
    else 
        {
        iEngine->CloseContactL(aId);
        CleanupStack::PopAndDestroy();  // item
        }
    }

// ---------------------------------------------------------
// CCalenNoteForm::CheckSpaceBelowCriticalLevelL
// Checks if the Flash File System storage will fall below
// Critical Level. Warning will be displayed if sotrage
// is below Critical Level.
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CSpdiaControl::CheckSpaceBelowCriticalLevelL()
    {
    TBool retcode(EFalse);

    if (SysUtil::FFSSpaceBelowCriticalLevelL(&(iCoeEnv->FsSession())))
        {
        CErrorUI* errorUi = CErrorUI::NewLC();
        errorUi->ShowGlobalErrorNoteL(KErrDiskFull);
        CleanupStack::PopAndDestroy(); // errorUi
        retcode = ETrue;
        }
    return retcode;
    }

// ---------------------------------------------------------
// CSpdiaContainer::HasOperation
//
// ---------------------------------------------------------
//
TInt CSpdiaControl::HasOperation(const MPbkThumbnailOperation& aOperation)
    {
    for (TInt index(0); index < iSdmArray->Count(); index++)
        {
        if ((*iSdmArray)[index].Operation() == &aOperation)
            {
            return index;
            }
        }
    return KNullIndexData;
    }

// ---------------------------------------------------------
// CSpdiaContainer::ResetArray
//
// ---------------------------------------------------------
//
void CSpdiaControl::ResetArray()
    {
    if(iSdmArray!= NULL)
        {
        for (TInt index(0); index < iSdmArray->Count(); index++)
            {
            delete (*iSdmArray)[index].ContactItem();
            delete (*iSdmArray)[index].Operation();
            }
        }
    }

// ---------------------------------------------------------
// CSpdiaContainer::UpdateIndexDataL
//
// ---------------------------------------------------------
//
TBool CSpdiaControl::UpdateIndexDataL(TInt aIndex)
    {
    CFbsBitmap* bitmap,*mask;
    bitmap = NULL;
    mask= NULL;
    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
	

    TSpdiaIndexData& data = (*iSdmArray)[aIndex];
    delete (*iIconArray)[aIndex];
    (*iIconArray)[aIndex] = NULL;
    TFileName bmpPath(iBmpPath);
    
    if(User::Language() == ELangHindi)
        {
        AknsUtils::CreateIconL(skinInstance, (*iDialSkinBmp)[aIndex],bitmap,mask,bmpPath,KDialHindiBmp[aIndex],  KDialHindiBmpMask[aIndex] );	
        }
    else 
        {
        AknsUtils::CreateIconL(skinInstance, (*iDialSkinBmp)[aIndex],bitmap,mask,bmpPath,KDialBmp[aIndex],  KDialBmpMask[aIndex] );
        }
    (*iIconArray)[aIndex] = CGulIcon::NewL(bitmap,mask);
	
    data.ResetThumbIndex();
    if (CreateIndexIconL(aIndex, data, ETrue))
        {
        data.SetOperation(
            iManager->GetThumbnailAsyncL(*this, *data.ContactItem()));
        }
    else
        {
        ChangeIndexDataL(aIndex, data);
        iGrid->DrawNow();
        }
    return ETrue;
    }

// ---------------------------------------------------------
// CSpdiaContainer::SetIndexDataL
// phone book access
// ---------------------------------------------------------
//
void CSpdiaControl::SetIndexDataL(TInt aIndex, TSpdiaIndexData& aSdmData)
    {
    TPhCltTelephoneNumber phonenumber;

    aSdmData.SetIndex(aIndex);
    aSdmData.SetPhoneNumber(KNullDesC);
    aSdmData.SetNumber(aIndex + 1);
    aSdmData.SetContactId(iEngine->GetSpeedDialFieldL(
                             aIndex + 1, phonenumber));
    if (aSdmData.ContactId() != KNullContactId)
        {
        phonenumber.Trim();
        if (phonenumber.Length() > 0)
            {
            aSdmData.SetPhoneNumber(phonenumber);
            }
        else
            {
            iEngine->RemoveSpeedDialFieldL(aSdmData.ContactId(), aIndex + 1);
            aSdmData.SetContactId(KNullContactId);
            }
        }
    }

// ---------------------------------------------------------
// CSpdiaContainer::ChangeIndexDataL
//
// ---------------------------------------------------------
//
void CSpdiaControl::ChangeIndexDataL(
    TInt aIndex,
    const TSpdiaIndexData& aSdmData )
    {
    CDesCArray* array = STATIC_CAST(CDesCArray*,
                        iGrid->Model()->ItemTextArray());
    HBufC* des = ItemDescriptorLC(aIndex, aSdmData);
    array->Delete(aIndex);
    array->InsertL(aIndex, *des);
    CleanupStack::PopAndDestroy(); // des
    }

// ---------------------------------------------------------
// CSpdiaContainer::CreateIndexIconL
//
// ---------------------------------------------------------
//
TBool CSpdiaControl::CreateIndexIconL(TInt aIndex,
                 TSpdiaIndexData& aSdmData, TBool aSetUpdateThumb)
    {
    TBool existThumb(EFalse);

    if (aSdmData.ContactId() != KNullContactId)
        {
        CPbkContactItem* item = iEngine->ReadContactL(aSdmData.ContactId());
        TInt fieldId(EPbkFieldIdNone);
        aSdmData.SetIconIndex(IconPositionL(*item, aIndex + 1, fieldId));
        aSdmData.SetFieldId( fieldId );  // set number type

        if (iManager->HasThumbnail(*item))
            {
            existThumb = ETrue;
            aSdmData.SetContactItem(item);

            if (aSetUpdateThumb)
                {
                aSdmData.SetLastThumb(ETrue);
                }
            aSdmData.SetThumbIndex(aIndex, NULL);
            }
        else
            {
            delete item;
            }
        }
    return existThumb;
    }

// ---------------------------------------------------------
// CSpdiaContainer::DrawShadow
//
// ---------------------------------------------------------
//
void CSpdiaControl::DrawShadow(CWindowGc& aGc) const
    {
    //TSize size(cell_spdia_item_w, cell_spdia_item_h);
    //Scalable UI.
    TAknWindowLineLayout area;
    if( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
        {
        area = AknLayoutScalable_Apps::cell_qdial_pane(0,0).LayoutLine();           
        }
    else
        {
        area = AppLayout::cell_qdial_pane(0,0);
        }
	
    TSize size(area.iW, area.iH);
    aGc.SetPenColor(AKN_LAF_COLOR(SDM_TN_SHADOW_COROL));
    TInt count(iSdmArray->Count());
    //TRect rect(TPoint(spdia_main_pane_grid_l, spdia_main_pane_grid_t), size);
    //Scalable UI
    if( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
        {
        area = AknLayoutScalable_Apps::grid_qdial_pane().LayoutLine();
        }
    else
        {
        area = AppLayout::Speed_Dial_descendants_Line_1();
        }
    TRect rect(TPoint(area.il, area.it), size);
    for (TInt i(0); i < count; ++i)
        {
        if (ThumbIndex(i) >= 0)
            {
            TInt type(ThumbType(i));
            TRect rc((*iShadowLayout)[type].Rect());
            if (type == CSpdiaGrid::EtnCOM)
                {
                TSize image(ThumbSize(i));
                image.iWidth = Min(image.iWidth, rc.Width());
                image.iHeight = Min(image.iHeight, rc.Height());
                rc = TRect(TPoint(0,0), image);
                TInt x(Max(0, (size.iWidth - image.iWidth) / 2) + 2);
                TInt y(Max(0, (size.iHeight - image.iHeight) / 2) + 2);
                rc.Move(x, y);
                }
            rc.Move(rect.iTl.iX, rect.iTl.iY);
            aGc.DrawRect(rc);
            }
        rect.Move(size.iWidth, 0);
        if (((i + 1)/ KCellColCount) * KCellRowCount == i + 1)
            {
            rect.Move(-(KCellColCount * size.iWidth), size.iHeight);
            }
        }
    }

// ---------------------------------------------------------
// CSpdiaControl::MopSupplyObject()
// Pass skin information if need.
// ---------------------------------------------------------
//
TTypeUid::Ptr CSpdiaControl::MopSupplyObject(TTypeUid aId)
    {
    if(aId.iUid == MAknsControlContext::ETypeId)
        {
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }
    return CCoeControl::MopSupplyObject( aId );
    }

// ---------------------------------------------------------
// CSpdiaControl::SizeChanged()
// ---------------------------------------------------------
//
void CSpdiaControl::SizeChanged()
    {
    iBgContext->SetRect( Rect() );
    }


// ---------------------------------------------------------
// CSpdiaControl::VoiceMailTypeL
//
// ---------------------------------------------------------
//
EXPORT_C TInt CSpdiaControl::VoiceMailType()
    {
    return iType;
    }
    
// ---------------------------------------------------------
// CSpdiaControl::GetThumbnailSize()
// Gets actual size of thumbnial to be drawn
// ---------------------------------------------------------
//
TSize CSpdiaControl::GetThumbnailSize()
    {
    TRect mainPaneRect ;
    TRect statusPaneRect;
    TInt  varient =0;

    if (Layout_Meta_Data::IsLandscapeOrientation())
        {
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,mainPaneRect);
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EStatusPane,statusPaneRect);
        mainPaneRect.iTl = statusPaneRect.iTl;
        mainPaneRect.iBr.iY = mainPaneRect.iBr.iY - statusPaneRect.iTl.iY;
        varient =1;
        }
    else
        {
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,mainPaneRect);
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EStatusPane,statusPaneRect);
        mainPaneRect.iTl= statusPaneRect.iTl;
        }
		
    TAknLayoutRect mainGridPaneRect;
    TAknLayoutRect gridPaneRect;
    TAknLayoutRect cellPaneRect;
    TAknLayoutRect thumbPaneRect;
	
    mainGridPaneRect.LayoutRect(mainPaneRect,AknLayoutScalable_Apps::main_qdial_pane());//main grid pane
    gridPaneRect.LayoutRect(mainGridPaneRect.Rect(),AknLayoutScalable_Apps::grid_qdial_pane());//main grid pane
    cellPaneRect.LayoutRect(gridPaneRect.Rect(),AknLayoutScalable_Apps::cell_qdial_pane(0,0));
    thumbPaneRect.LayoutRect(cellPaneRect.Rect(),AknLayoutScalable_Apps::thumbnail_qdial_pane(varient));

    return thumbPaneRect.Rect().Size();
    }
// ---------------------------------------------------------
// CSpdiaControl::GetSpdCtrlLastError()
// retuns last error 
// ---------------------------------------------------------
//	
EXPORT_C TInt CSpdiaControl::GetSpdCtrlLastError()
    {
    return iError;
    }
// End of File
