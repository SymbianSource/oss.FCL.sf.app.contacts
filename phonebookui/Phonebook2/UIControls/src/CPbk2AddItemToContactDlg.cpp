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
* Description:  Phonebook 2 add new contact field to a contact dialog.
*
*/


#include <CPbk2AddItemToContactDlg.h>

// Phonebook 2
#include <TPbk2AddItemDialogParams.h>
#include <TPbk2AddItemWrapper.h>
#include <pbk2uicontrols.rsg>
#include <CPbk2IconArray.h>
#include <TPbk2AppIconId.h>
#include <CPbk2UIExtensionManager.h>
#include <MPbk2UIExtensionIconSupport.h>
#include <MPbk2AppUi.h>
#include <CPbk2ServiceManager.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2ApplicationServices2.h>
#include "CPbk2IconInfo.h"
#include <aknlayoutscalable_avkon.cdl.h>
#include <MPbk2ApplicationServices.h>

// System includes
#include <aknPopup.h>
#include <aknlists.h>
#include <barsread.h>


/// Unnamed namespace for local definitions
namespace {

const TInt KMaxListBoxBufferSize = 128;
_LIT(KListBoxIconFormat, "%d\t");

#ifdef _DEBUG

enum TPanicCode
    {
    EPanicPreCond_ResetWhenDestroyed = 1
    };

void Panic( TInt aReason )
    {
    _LIT( KPanicText, "CPbk2AddItemToContactDlg" );
    User::Panic( KPanicText, aReason );
    }

#endif // _DEBUG


/**
 * Phonebook 2 list box model for add item wrappers.
 */
NONSHARABLE_CLASS(CModel) : public CBase,
                            public MDesCArray
    {
    public: // Construction

        /**
         * Constructor.
         *
         * @param aArray        An array of wrappers.
         * @param aIconArray    Icon array.
         * @return  A new instance of this class.
         */
        CModel(
                RArray<TPbk2AddItemWrapper>& aArray,
                CPbk2IconArray& aIconArray );

    public: // From MDesCArray
        TInt MdcaCount() const;
        TPtrC16 MdcaPoint(
                TInt aIndex ) const;

    private: // Implementation
        static TInt Compare(
                const TPbk2AddItemWrapper& aLhs,
                const TPbk2AddItemWrapper& aRhs );

    private: // Data
        /// Ref: Array of TPbk2AddItemWrapper objects to show
        RArray<TPbk2AddItemWrapper>& iArray;
        /// Own: Buffer for formatting a list box row text
        mutable TBuf16<KMaxListBoxBufferSize> iBuffer;
        /// Ref: Icons for field types
        CPbk2IconArray& iIconArray;
    };

// --------------------------------------------------------------------------
// CModel::CModel
// --------------------------------------------------------------------------
//
CModel::CModel(RArray<TPbk2AddItemWrapper>& aArray,
    CPbk2IconArray& aIconArray) :
        iArray(aArray),
        iIconArray(aIconArray)
    {
    iArray.Sort(TLinearOrder<TPbk2AddItemWrapper>(CModel::Compare));
    }

// --------------------------------------------------------------------------
// CModel::MdcaCount
// --------------------------------------------------------------------------
//
TInt CModel::MdcaCount() const
    {
    return iArray.Count();
    }

// --------------------------------------------------------------------------
// CModel::MdcaPoint
// --------------------------------------------------------------------------
//
TPtrC16 CModel::MdcaPoint( TInt aIndex ) const
    {
    // Insert icon index
    TInt iconIndex = iIconArray.FindIcon(iArray[aIndex].IconId());
    if (iconIndex < 0)
        {
        // If icon is not found by Id fall back to empty icon
        iconIndex = iIconArray.FindIcon(
            TPbk2AppIconId(EPbk2qgn_prop_nrtyp_empty));
        }

    iBuffer.Format(KListBoxIconFormat, iconIndex);
    const TDesC& fieldName = iArray[aIndex].Label();
    const TInt chars = Min(iBuffer.MaxLength() - iBuffer.Length(),
        fieldName.Length());
    iBuffer.Append(fieldName.Left(chars));
    return iBuffer;
    }

// --------------------------------------------------------------------------
// CModel::Compare
// --------------------------------------------------------------------------
//
TInt CModel::Compare(   const TPbk2AddItemWrapper& aLhs,
        const TPbk2AddItemWrapper& aRhs )
    {
    return aLhs.AddItemOrdering() - aRhs.AddItemOrdering();
    }

} /// namespace

// --------------------------------------------------------------------------
// CPbk2AddItemToContactDlg::CPbk2AddItemToContactDlg
// --------------------------------------------------------------------------
//
inline CPbk2AddItemToContactDlg::CPbk2AddItemToContactDlg( MPbk2ApplicationServices* aAppServices ) :
	iAppServices( aAppServices )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2AddItemToContactDlg::~CPbk2AddItemToContactDlg
// --------------------------------------------------------------------------
//
CPbk2AddItemToContactDlg::~CPbk2AddItemToContactDlg()
    {
    // Tells ExecuteLD this object has been destroyed
    if (iDestroyedPtr)
        {
        *iDestroyedPtr = ETrue;
        }

    if ( iSelfPtr )
        {
        *iSelfPtr = NULL;
        }

    if ( iPopup )
        {
        iPopup->CancelPopup();
        }

    delete iListBox;
    }

// --------------------------------------------------------------------------
// CPbk2AddItemToContactDlg::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2AddItemToContactDlg* CPbk2AddItemToContactDlg::NewL()
    {
    CPbk2AddItemToContactDlg* self = new(ELeave) CPbk2AddItemToContactDlg();
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AddItemToContactDlg::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2AddItemToContactDlg* CPbk2AddItemToContactDlg::NewL(
	MPbk2ApplicationServices* aAppServices )
    {
    CPbk2AddItemToContactDlg* self = new(ELeave) CPbk2AddItemToContactDlg( aAppServices );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AddItemToContactDlg::ExecuteLD
// --------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2AddItemToContactDlg::ExecuteLD
        ( RArray<TPbk2AddItemWrapper>& aAddItems,
          const TPbk2AddItemDialogParams* aParams /*= NULL*/ )
    {
    CleanupStack::PushL( this );

    TBool thisDestroyed = EFalse;
    iDestroyedPtr = &thisDestroyed;

    PreparePopupL( aAddItems, aParams );
    TInt result = ExecutePopupL( aParams );

    if ( thisDestroyed )
        {
        // This object has been destroyed
        CleanupStack::Pop( this );
        }
    else
        {
        CleanupStack::PopAndDestroy( this );
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2AddItemToContactDlg::RequestExitL
// --------------------------------------------------------------------------
//
void CPbk2AddItemToContactDlg::RequestExitL( TInt /*aCommandId*/ )
    {
    if ( iPopup )
        {
        iPopup->CancelPopup();
        }
    }

// --------------------------------------------------------------------------
// CPbk2AddItemToContactDlg::ForceExit
// --------------------------------------------------------------------------
//
void CPbk2AddItemToContactDlg::ForceExit()
    {
    delete this;
    }

// --------------------------------------------------------------------------
// CPbk2AddItemToContactDlg::ResetWhenDestroyed
// --------------------------------------------------------------------------
//
void CPbk2AddItemToContactDlg::ResetWhenDestroyed
        ( MPbk2DialogEliminator** aSelfPtr )
    {
    __ASSERT_DEBUG(!aSelfPtr || *aSelfPtr == this,
        Panic( EPanicPreCond_ResetWhenDestroyed ) );

    iSelfPtr = aSelfPtr;
    }

// --------------------------------------------------------------------------
// CPbk2AddItemToContactDlg::PreparePopupL
// --------------------------------------------------------------------------
//
void CPbk2AddItemToContactDlg::PreparePopupL
        ( RArray<TPbk2AddItemWrapper>& aAddItems,
          const TPbk2AddItemDialogParams* aParams )
    {
    // Create a list box
    delete iListBox;
    iListBox = NULL;
    iListBox = static_cast<CEikFormattedCellListBox*>
        ( EikControlFactory::CreateByTypeL
                ( EAknCtSingleGraphicPopupMenuListBox ).iControl );

    TInt cbaResource = R_AVKON_SOFTKEYS_OK_CANCEL__OK;
    if (aParams && aParams->iCbaResource)
        {
        cbaResource = aParams->iCbaResource;
        }

    // Create a popup list
    if ( iPopup )
        {
        iPopup->CancelPopup();
        iPopup = NULL;
        }
    iPopup = CAknPopupList::NewL( iListBox,
        cbaResource, AknPopupLayouts::EMenuGraphicWindow );

    if ( aParams && aParams->iTitle && aParams->iTitle->Length() > 0 )
        {
        iPopup->SetTitleL( *aParams->iTitle );
        }

    // Init list box
    iListBox->ConstructL( iPopup, CEikListBox::ELeftDownInViewRect );
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL
        ( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    // Create icon array
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC
        ( reader, R_PBK2_FIELDTYPE_ICONS );
    CPbk2IconArray* iconArray = CPbk2IconArray::NewL(reader);
    CleanupStack::PopAndDestroy(); // reader
    CleanupStack::PushL( iconArray );
    
    //Calculate preferred size for xsp service icons 
    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect(
        AknLayoutUtils::EMainPane, mainPane );
    TAknLayoutRect listLayoutRect;
    listLayoutRect.LayoutRect(
        mainPane,
        AknLayoutScalable_Avkon::list_single_graphic_pane_g2(0).LayoutLine() );
    TSize size(listLayoutRect.Rect().Size());
    
    MPbk2ApplicationServices2* servicesExtension = NULL ;
    
    /*
	 * Use iAppServices if provided. This is to enable editor use outside from pbk2 context
	 */
	if( iAppServices )
		{
		servicesExtension = reinterpret_cast<MPbk2ApplicationServices2*>
			( iAppServices->MPbk2ApplicationServicesExtension( KMPbk2ApplicationServicesExtension2Uid ) );
		}
	else
		{
		// Add xsp service icons
		servicesExtension =
			reinterpret_cast<MPbk2ApplicationServices2*>
				( Phonebook2::Pbk2AppUi()->ApplicationServices().
					MPbk2ApplicationServicesExtension(
						KMPbk2ApplicationServicesExtension2Uid ) );               
		}
	
    CPbk2ServiceManager& servMan = servicesExtension->ServiceManager();
    const CPbk2ServiceManager::RServicesArray& services = servMan.Services();
    TUid uid;
    uid.iUid = KPbk2ServManId;
    for ( TInt i = 0; i < services.Count(); i++ )
        {
        const CPbk2ServiceManager::TService& service = services[i];
        if ( service.iBitmapId && service.iBitmap )
            {
        	AknIconUtils::SetSize(
        	           service.iBitmap,
        	           size );
        	AknIconUtils::SetSize(
        	           service.iMask,
        	           size );            
            TPbk2IconId id = TPbk2IconId( uid, services[i].iBitmapId );
            CPbk2IconInfo* info = CPbk2IconInfo::NewLC(
            	id, services[i].iBitmap, services[i].iMask, size );
            iconArray->AppendIconL(info);
            CleanupStack::Pop(info);
            }
        }

    // Add icons from UI extensions
    CPbk2UIExtensionManager* extManager =
        CPbk2UIExtensionManager::InstanceL();
    extManager->PushL();
    extManager->IconSupportL().AppendExtensionIconsL( *iconArray );
    CleanupStack::PopAndDestroy(); // extManager
    CleanupStack::Pop( iconArray );

    // Set icon array
    iListBox->ItemDrawer()->ColumnData()->SetIconArray( iconArray );

    // Create listbox model
    CModel* listBoxModel = new(ELeave) CModel( aAddItems, *iconArray );
    iListBox->Model()->SetItemTextArray(listBoxModel);
    iListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
    
    }

// --------------------------------------------------------------------------
// CPbk2AddItemToContactDlg::ExecutePopupL
// --------------------------------------------------------------------------
//
TInt CPbk2AddItemToContactDlg::ExecutePopupL
        ( const TPbk2AddItemDialogParams* aParams )
    {
    TInt result = KErrCancel;

    if ( aParams && aParams->iSelectionIndex >= 0 )
        {
        // Silent mode, the selection has already been made
        // Result is the same index as was given in parameters,
        // the aAddItems array got sorted when creating the list box model
        // so the returned index value is valid now
        result = aParams->iSelectionIndex;
        }
    else
        {
        // iDestroyedPtr points to ExecuteLD's thisDestroyed local variable,
        // and we need another destruction check here. We cannot copy the
        // solution from ExecuteLD since making iDestroyedPtr point to a
        // local variable defined in this function rather than the
        // variable of ExecuteLD causes the destructor change the value of
        // a variable that is out of scope and it will lead to a crash
        // eventually. So thisDestroyed now points to the
        // ExecuteLD::thisDestroyed and it is still in scope in destructor.
        TBool* thisDestroyed = iDestroyedPtr;

        TBool popupAccepted = iPopup->ExecuteLD();
        iPopup = NULL;

        if ( popupAccepted && *thisDestroyed == EFalse )
            {
            // Retrieve and return selection
            TInt index = iListBox->CurrentItemIndex();
            if ( index >= KErrNone )
                {
                result = index;
                }
            }
        }

    return result;
    }

// End of File
