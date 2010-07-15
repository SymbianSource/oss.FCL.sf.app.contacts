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
* Description:  Execute Maps application.
*
*/

// INCLUDES
#include "cpmapcmd.h"

// Phonebook 2
#include <MPbk2ContactUiControl.h>
#include <MPbk2CommandObserver.h>
#include <MPbk2CommandHandler.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>
#include <CPbk2FieldPropertyArray.h>
#include <Pbk2MapUIRes.rsg>
#include <MPbk2ContactEditorControl.h>
#include <MPbk2ContactEditorField.h>
#include <Pbk2UIControls.hrh>
#include <Pbk2Commands.hrh>
#include <Pbk2UIControls.rsg>
#include <TPbk2AddressSelectParams.h>
#include <CPbk2AddressSelect.h>
#include <Pbk2AddressTools.h>
#include <Pbk2UIControls.rsg>
#include <CPbk2ApplicationServices.h>
#include <pbk2mapcommands.hrh>

// Virtual Phonebook
#include <MVPbkBaseContactField.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactAttributeManager.h>
#include <CVPbkVoiceTagAttribute.h>
#include <CVPbkContactManager.h>
#include <CVPbkFieldTypeSelector.h>
#include <CVPbkFieldFilter.h>
#include <MVPbkStoreContactField.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactFieldTextData.h>
#include <VPbkEng.rsg>
#include <MVPbkFieldType.h>
#include <TVPbkFieldVersitProperty.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <TVPbkFieldTypeParameters.h>
#include "MPbk2ContactEditorControlExtension.h"
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactStore.h>
#include <MVPbkBaseContact.h>
#include <MVPbkViewContact.h>
#include <MPbk2FieldProperty.h>

 

// System includes
#include <lbsposition.h>
#include <AiwServiceHandler.h>
#include <mnaiwservices.h>
#include <mnmapview.h>
#include <mnproviderfinder.h>
#include <mngeocoder.h>
#include <EPos_CPosLandmark.h>
#include <epos_poslandmarkserialization.h>
#include <eikclb.h>
#include <aknPopup.h>
#include <eiktxlbm.h>
#include <aknnotewrappers.h>
#include <coemain.h>
#include <barsread.h>
#include <avkon.rsg>
#include <StringLoader.h>

// Debugging headers
#include <Pbk2Debug.h>

_LIT( KGeoSeparator, ",");
const TInt KGeoSeparatorLength = 1;
const TReal32 KGeoHorizontalAccuracy = 0;
const TInt KGeoMaxSize = 60;
const TInt KGeoFormatWidth = 11;

// --------------------------------------------------------------------------
// CPmapCmd::CPmapCmd
// --------------------------------------------------------------------------
//
CPmapCmd::CPmapCmd( MPbk2ContactEditorControl& aEditorControl,
		MVPbkStoreContact& aContact, TInt aCommandId ) :
		CActive( CActive::EPriorityStandard ),
		        iEditorControl( &aEditorControl ),
		        iContact( &aContact ),
            iCommandId( aCommandId )            
    {
    CActiveScheduler::Add( this );
    iAddressUpdatePrompt = ETrue;
    iCurrentMapLaunchedByState = EMapNone;
    }

// --------------------------------------------------------------------------
// CPmapCmd::CPmapCmd
// --------------------------------------------------------------------------
//
CPmapCmd::CPmapCmd( MPbk2ContactUiControl& aUiControl, TInt aCommandId ) :
            CActive( CActive::EPriorityStandard ),
            iUiControl( &aUiControl ),
            iCommandId( aCommandId )
    {
    CActiveScheduler::Add( this );
    iAddressUpdatePrompt = ETrue;
    iCurrentMapLaunchedByState = EMapNone;
    }

// --------------------------------------------------------------------------
// CPmapCmd::~CPmapCmd
// --------------------------------------------------------------------------
//
CPmapCmd::~CPmapCmd()
    {
    Cancel();
    if(iMapView)
        {
        iMapView->ResetLandmarksToShow();
        delete iMapView;
        iMapView = NULL;    
        }
    ReleaseLandmarkResources();
    if( iUiControl )
	    {
	    iUiControl->RegisterCommand( NULL );
	    }
    delete iMapViewProvider;
    }

// --------------------------------------------------------------------------
// CPmapCmd::NewL
// --------------------------------------------------------------------------
//
CPmapCmd* CPmapCmd::NewL
        ( MPbk2ContactEditorControl& aEditorControl,
          MVPbkStoreContact& aContact, TInt aCommandId )
    {
    CPmapCmd* self = new ( ELeave ) CPmapCmd
        ( aEditorControl, aContact, aCommandId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPmapCmd::NewL
// --------------------------------------------------------------------------
//
CPmapCmd* CPmapCmd::NewL
        ( MPbk2ContactUiControl& aUiControl, TInt aCommandId )
    {
    CPmapCmd* self = new ( ELeave ) CPmapCmd
        ( aUiControl, aCommandId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPmapCmd::ConstructL
// --------------------------------------------------------------------------
//
void CPmapCmd::ConstructL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPmapCmd::ConstructL"));    
    
    RPointerArray<CMnProvider> providers;

    MnProviderFinder::FindProvidersL( providers, CMnProvider::EServiceMapView );
    CleanupClosePushL( providers );
    if (providers.Count() > 0)
        {
        iMapViewProvider = providers[0];
        providers.Remove(0);
        }

    providers.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &providers );
                
    if ( !iMapViewProvider )
        {
        User::Leave( KErrNotSupported );
        }
    
    if( iUiControl && ! iContact )
    	{
    	iContact = const_cast<MVPbkStoreContact*>(iUiControl->FocusedStoreContact());
    	if( !iContact )
    		{
	    	iUiControl->RegisterCommand( this );
	    	const MVPbkViewContact* viewContact = iUiControl->FocusedViewContactL();
	    	if( viewContact )
	    		{
	    		viewContact->ReadL(*this);
		        CActiveScheduler::Start();
		        }
    		}
    	}
    
    User::LeaveIfNull(iContact);
    
    
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPmapCmd::ConstructL end"));        
    }

// --------------------------------------------------------------------------
// CPmapCmd::CheckViewProviderL
// --------------------------------------------------------------------------
//
TBool CPmapCmd::CheckViewProviderL()
    {
    TBool ret = EFalse;
    RPointerArray<CMnProvider> providers;

    MnProviderFinder::FindProvidersL( providers, CMnProvider::EServiceMapView );
    
    CleanupClosePushL(providers);
    
    if ( providers.Count() > 0 )
        {
        ret = ETrue;
        }

    providers.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &providers );
    return ret;
    }

void CPmapCmd::ExecuteLD()
    {
    ExecuteL();
    }

// --------------------------------------------------------------------------
// CPmapCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPmapCmd::ExecuteL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPmapCmd::ExecuteLD"));

    iAddressUpdatePrompt = ETrue;
    TPbk2FieldGroupId groupId = EPbk2FieldGroupIdNone;
    if( !IsAddressInContact() )
    	{
    	if( iCommandId == EPbk2ExtensionShowOnMap )
    		{
    		HBufC* prompt = StringLoader::LoadLC( R_QTN_PHOB_NO_ADDRESS_DEFINE_NEW );
    		CAknQueryDialog* dlg = CAknQueryDialog::NewL();
			if( dlg->ExecuteLD( R_PBK2_GENERAL_CONFIRMATION_QUERY, *prompt ) )
				{
				CleanupStack::PopAndDestroy( prompt );
				groupId = EPbk2FieldGroupIdPostalAddress;
				iCommandId = EPbk2ExtensionAssignFromMap;
				}
			else
				{
				CleanupStack::PopAndDestroy( prompt );
				FinishProcess();				
				PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
					("CPmapCmd::ExecuteLD end"));
				
				return;
				}
			
    		}
    	else
    		{
    		groupId = EPbk2FieldGroupIdPostalAddress;
    		}
    	iAddressUpdatePrompt = EFalse;
    	}

     if( groupId == EPbk2FieldGroupIdNone )
    	{
    	groupId = GetAddressFromEditorView();
    	}
    
    if( groupId == EPbk2FieldGroupIdNone )
    	{
    	groupId = GetAddressFromUiControl();
    	}
    
    if( groupId == EPbk2FieldGroupIdNone )
		{
		groupId = GetAddressIfIsAlone();
		}
    
    if( groupId == EPbk2FieldGroupIdNone )
    	{
    	groupId = SelectAddressL();
    	}
    
    if( groupId != EPbk2FieldGroupIdNone )
    	{
	    TVPbkFieldTypeParameter addressType = Pbk2AddressTools::MapAddressToVersitParam( groupId );
	    if( iCommandId == EPbk2ExtensionShowOnMap )
    		{
    		EditorShowOnMapsL(addressType);
    		return; // as the asynchronous request is processed next part will be executed inside RunL of this class. 
    		}
    	else if( iCommandId == EPbk2ExtensionAssignFromMap )
    		{
    		EditorAssignFromMapsL(addressType);
    	    return; // as the asynchronous request is processed next part will be executed inside RunL of this class.
    		}
    	}
    
    FinishProcess();

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPmapCmd::ExecuteLD end"));
    }

// --------------------------------------------------------------------------
// CPmapCmd::GetAddressIfIsAlone
// --------------------------------------------------------------------------
//
TPbk2FieldGroupId CPmapCmd::GetAddressIfIsAlone()
    {
    TPbk2FieldGroupId grupId = EPbk2FieldGroupIdNone;
    MVPbkStoreContactFieldCollection& arrFields = iContact->Fields();
	TInt count = arrFields.FieldCount();
	for( TInt idx = 0; idx < count; idx++)
		{
		MVPbkStoreContactField& field = arrFields.FieldAt(idx);
		const MVPbkFieldType* type = field.BestMatchingFieldType();
		if ( type )
		    {
            TArray<TVPbkFieldVersitProperty> arrProp = type->VersitProperties();
            for (TInt idx2 = 0; idx2 < arrProp.Count(); idx2++ )
                {
                if ( arrProp[idx2].Name() == EVPbkVersitNameADR )
                    {
                    if ( arrProp[idx2].Parameters().Contains( EVPbkVersitParamHOME ) )
                        {
                        if ( grupId == EPbk2FieldGroupIdNone )
                            {
                            grupId = EPbk2FieldGroupIdHomeAddress;
                            }
                        else if ( grupId != EPbk2FieldGroupIdHomeAddress )
                            {
                            return EPbk2FieldGroupIdNone;
                            }
                        }
                    else if ( arrProp[idx2].Parameters().Contains( EVPbkVersitParamWORK ) )
                        {
                        if ( grupId == EPbk2FieldGroupIdNone )
                            {
                            grupId = EPbk2FieldGroupIdCompanyAddress;
                            }
                        else if ( grupId != EPbk2FieldGroupIdCompanyAddress )
                            {
                            return EPbk2FieldGroupIdNone;
                            }
                        }
                    else
                        {
                        if ( grupId == EPbk2FieldGroupIdNone )
                            {
                            grupId = EPbk2FieldGroupIdPostalAddress;
                            }
                        else if ( grupId != EPbk2FieldGroupIdPostalAddress )
                            {
                            return EPbk2FieldGroupIdNone;
                            }
                        }
                    }
                }
		    }
		}
    
	return grupId;
    }

// --------------------------------------------------------------------------
// CPmapCmd::GetAddressFromEditorView
// --------------------------------------------------------------------------
//
TPbk2FieldGroupId CPmapCmd::GetAddressFromEditorView()
    {
    TPbk2FieldGroupId grupId = EPbk2FieldGroupIdNone;
    if( ControlExtension() )
    	{
    	grupId = Pbk2AddressTools::MapCtrlTypeToAddress( ControlExtension()->FocusedControlType() );
    	}
    
    if( iEditorControl && grupId == EPbk2FieldGroupIdNone )
    	{
		TInt countControls = iEditorControl->NumberOfControls();
		for ( TInt idx = 0; idx < countControls; idx++ )
		    {
		    MPbk2ContactEditorField& field = iEditorControl->EditorField( idx );
		    
		    grupId = field.FieldProperty().GroupId();
		    if( grupId == EPbk2FieldGroupIdPostalAddress || 
				grupId == EPbk2FieldGroupIdHomeAddress || 
				grupId == EPbk2FieldGroupIdCompanyAddress )
		    	{
		    	iAddressView = ETrue;
		    	break;
		    	}
		    }
    	}
    
	return grupId;
    }

// --------------------------------------------------------------------------
// CPmapCmd::GetAddressFromUiControl
// --------------------------------------------------------------------------
//
TPbk2FieldGroupId CPmapCmd::GetAddressFromUiControl()
    {
    TPbk2FieldGroupId ret = EPbk2FieldGroupIdNone;
    const MVPbkBaseContactField* field = NULL;
    if( iUiControl )
    	{
    	field = iUiControl->FocusedField();
    	}
    if( field )
    	{
    	const MVPbkFieldType* type = field->BestMatchingFieldType();
    	if( type )
    		{
    		TArray<TVPbkFieldVersitProperty> arrProp = type->VersitProperties();
    		for( TInt idx = 0; idx < arrProp.Count(); idx++)
				{
				if( arrProp[idx].Name() != EVPbkVersitNameADR )
					{
					continue;
					}
				
				if( arrProp[idx].Parameters().Contains( 
					Pbk2AddressTools::MapAddressToVersitParam( EPbk2FieldGroupIdHomeAddress ) ) )
					{
					ret = EPbk2FieldGroupIdHomeAddress;
					break;
					}
				else if( arrProp[idx].Parameters().Contains( 
					Pbk2AddressTools::MapAddressToVersitParam( EPbk2FieldGroupIdCompanyAddress ) ) )
					{
					ret = EPbk2FieldGroupIdCompanyAddress;
					break;
					}
				else
					{
					ret = EPbk2FieldGroupIdPostalAddress;
					break;
					}
				}
    		}
    	}
    
	return ret;
    }

// --------------------------------------------------------------------------
// CPmapCmd::SelectAddressL
// --------------------------------------------------------------------------
//
TPbk2FieldGroupId CPmapCmd::SelectAddressL()
   {
    TPbk2FieldGroupId ret = EPbk2FieldGroupIdNone;

    TResourceReader resReader;
    CCoeEnv::Static()->CreateResourceReaderLC
        (resReader, R_PBK2_ADDRESS_SELECT);

    CPbk2ApplicationServices* appServices =
        CPbk2ApplicationServices::InstanceLC();

    TPbk2AddressSelectParams params(
        *iContact,
        appServices->ContactManager(),
        appServices->NameFormatter(),
        appServices->FieldProperties(),
        resReader );
    if ( iCommandId == EPbk2ExtensionShowOnMap )
    	{
    	params.iCommMethod = VPbkFieldTypeSelectorFactory::EFindOnMapSelector;
    	}
    else
    	{
        params.iCommMethod = VPbkFieldTypeSelectorFactory::EAssignFromMapSelector;
    	}

    CleanupStack::PopAndDestroy( appServices );
    CPbk2AddressSelect* addressSelect = CPbk2AddressSelect::NewL( params );
    MVPbkStoreContactField* selectedField = addressSelect->ExecuteLD();
    CleanupStack::PopAndDestroy(); // resReader

    if ( selectedField )
    	{
    	CleanupDeletePushL( selectedField );
    	ret = GetFieldGroupL( *selectedField );
        CleanupStack::PopAndDestroy( selectedField );
    	}

    return ret;    
   }

// --------------------------------------------------------------------------
// CPmapCmd::ControlExtension
// --------------------------------------------------------------------------
//
MPbk2ContactEditorControlExtension* CPmapCmd::ControlExtension()
    {
    if(iEditorControl && iEditorControl->ContactEditorControlExtension(TUid::Null()))
    	{
    	return static_cast<MPbk2ContactEditorControlExtension*>
    		(iEditorControl->ContactEditorControlExtension(TUid::Null()));
    	}
    return NULL;
    }

// --------------------------------------------------------------------------
// CPmapCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPmapCmd::AddObserver( MPbk2CommandObserver& aObserver )
    {
    iObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPmapCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPmapCmd::ResetUiControl( MPbk2ContactUiControl& aUiControl )
    {
    if (iUiControl == &aUiControl)
        {
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPmapCmd::FinishProcess
// --------------------------------------------------------------------------
//
void CPmapCmd::FinishProcess()
    {
    if (iObserver)
        {
        iObserver->CommandFinished( *this );
        }
    }
    
// --------------------------------------------------------------------------
// CPmapCmd::EditorShowMapL
// --------------------------------------------------------------------------
//
void CPmapCmd::EditorShowOnMapsL( TVPbkFieldTypeParameter aAddressType )
    {
    iAddressType = aAddressType;
    
    RPointerArray<CPosLandmark> landmarks;
    CleanupClosePushL( landmarks );

    CPosLandmark* landmark = CPosLandmark::NewL();
    CleanupStack::PushL( landmark );

    FillGeoLandmarkL( *landmark, aAddressType );
    FillLandmarkL( *landmark, aAddressType );
    SetLandmarkNameL(*landmark);
                
    TLocality locality;
    if ( landmark->NumOfAvailablePositionFields() > 0
        || landmark->GetPosition( locality ) == KErrNone )
        {
        landmarks.AppendL( landmark );
        }

    if ( landmark->GetPosition( locality ) == KErrNone )
        {
        if(iMapView)
            {
            delete iMapView;
            iMapView = NULL;
            }
        iMapView = CMnMapView::NewL( *iMapViewProvider );
        iMapView->AddLandmarksToShowL( landmarks );
        iMapView->ShowMapL();
        CleanupStack::PopAndDestroy( landmark );
        CleanupStack::PopAndDestroy( &landmarks );
        delete iMapView;
        iMapView = NULL;
        FinishProcess();
        }
    else
        {
        if(iMapView)
            {
            delete iMapView;
            iMapView = NULL;
            }
        iMapView = CMnMapView::NewChainedL( *iMapViewProvider );
        iMapView->AddLandmarksToShowL( landmarks );
        iMapView->SelectFromMapL( iStatus );
        iCurrentMapLaunchedByState = EMapShowOnMaps;
        SetActive();
        CleanupStack::PopAndDestroy( landmark );
        CleanupStack::PopAndDestroy( &landmarks );
        }
    }

// --------------------------------------------------------------------------
// CPmapCmd::FillLandmarkL
// --------------------------------------------------------------------------
//
void CPmapCmd::FillLandmarkL(CPosLandmark& aLandmark, TVPbkFieldTypeParameter aAddressType)
	{
	if( iAddressView && iEditorControl )
		{
		TInt count = iEditorControl->NumberOfControls();
		for( TInt idx1 = 0; idx1 < count; idx1++)
			{
			MPbk2ContactEditorField& field = iEditorControl->EditorField(idx1);
			HBufC* uiData = field.ControlTextL();
			if( !uiData || !uiData->Length() )
				continue;
			
			CleanupStack::PushL(uiData);
			TArray<TVPbkFieldVersitProperty> arrProp = field.FieldProperty().FieldType().VersitProperties();
			for( TInt idx2 = 0; idx2 < arrProp.Count(); idx2++)
				{
				if( arrProp[idx2].Name() == EVPbkVersitNameADR )
					{
					
					if( arrProp[idx2].SubField() == EVPbkVersitSubFieldCountry )
						{
						aLandmark.SetPositionFieldL( EPositionFieldCountry, uiData->Des() );
						continue;
						}
					else if( arrProp[idx2].SubField() == EVPbkVersitSubFieldLocality )
						{
						aLandmark.SetPositionFieldL( EPositionFieldCity, uiData->Des() );
						continue;
						}
					else if( arrProp[idx2].SubField() == EVPbkVersitSubFieldStreet )
						{
						aLandmark.SetPositionFieldL( EPositionFieldStreet, uiData->Des() );
						continue;
						}
					else if( arrProp[idx2].SubField() == EVPbkVersitSubFieldPostalCode )
						{
						aLandmark.SetPositionFieldL( EPositionFieldPostalCode, uiData->Des() );
						continue;
						}
					}
				}
			CleanupStack::PopAndDestroy(uiData);
			}
		}
	else
		{
		MVPbkStoreContactFieldCollection& arrFields = iContact->Fields();
		TInt count = arrFields.FieldCount();
		for( TInt idx = 0; idx < count; idx++)
			{
			MVPbkStoreContactField& field = arrFields.FieldAt(idx);
			const MVPbkContactFieldData& fieldData = field.FieldData();
			if( fieldData.DataType() != EVPbkFieldStorageTypeText )
				{
				continue;
				}
			const TDesC& dataText = MVPbkContactFieldTextData::Cast( fieldData ).Text();
			
			if( !dataText.Length() )
				{
				continue;
				}
			const MVPbkFieldType* type = field.BestMatchingFieldType();
			if ( type )
			    {
			    TArray<TVPbkFieldVersitProperty> arrProp = type->VersitProperties();
                for (TInt idx2 = 0; idx2 < arrProp.Count(); idx2++ )
                    {
                    if ( arrProp[idx2].Name() == EVPbkVersitNameADR
                        && (arrProp[idx2].Parameters().Contains( aAddressType )
                            || (aAddressType == EVPbkVersitParamPREF
                                && !arrProp[idx2].Parameters().Contains( EVPbkVersitParamHOME )
                                && !arrProp[idx2].Parameters().Contains( EVPbkVersitParamWORK ))) )
                        {
                        if ( arrProp[idx2].SubField() == EVPbkVersitSubFieldCountry )
                            {
                            aLandmark.SetPositionFieldL( EPositionFieldCountry, dataText );
                            continue;
                            }
                        else if ( arrProp[idx2].SubField() == EVPbkVersitSubFieldLocality )
                            {
                            aLandmark.SetPositionFieldL( EPositionFieldCity, dataText );
                            continue;
                            }
                        else if ( arrProp[idx2].SubField() == EVPbkVersitSubFieldStreet )
                            {
                            aLandmark.SetPositionFieldL( EPositionFieldStreet, dataText );
                            continue;
                            }
                        else if ( arrProp[idx2].SubField() == EVPbkVersitSubFieldPostalCode )
                            {
                            aLandmark.SetPositionFieldL( EPositionFieldPostalCode, dataText );
                            continue;
                            }
                        }
                    }
			    }
			}
		}
	}

void CPmapCmd::SetLandmarkNameL(CPosLandmark& aLandmark)
    {
    _LIT(KComma, ",");
    _LIT(KSpace, " ");
    
    TInt length = 0; 
    TPtrC street;
    if( aLandmark.IsPositionFieldAvailable( EPositionFieldStreet ) )
        {
        aLandmark.GetPositionField( EPositionFieldStreet, street );
        length += street.Length();
        }
    
    TPtrC city;
    if( aLandmark.IsPositionFieldAvailable( EPositionFieldCity ) )
        {
        aLandmark.GetPositionField( EPositionFieldCity, city );
        if( length )
            {
            length += KComma().Length() + KSpace().Length();
            }
        length += city.Length();
        }   
    
    TPtrC country;
    if( aLandmark.IsPositionFieldAvailable( EPositionFieldCountry ) )
        {
        aLandmark.GetPositionField( EPositionFieldCountry, country );
        if( length )
            {
            length += KComma().Length() + KSpace().Length();
            }
        length += country.Length();
        }   
    
    RBuf newAddr;
    newAddr.CreateL( length );
    CleanupClosePushL( newAddr );
    
    if( street.Length() )
        {
        newAddr += street;
        }
    
    if( city.Length() )
        {
        if( newAddr.Length() > 0 )
            {
            newAddr += KComma();
            newAddr += KSpace();
            }
        newAddr += city;
        }
    
    if( country.Length() )
        {
        if( newAddr.Length() > 0 )
            {
            newAddr += KComma();
            newAddr += KSpace();
            }
        newAddr += country;
        }
    if(length)
        aLandmark.SetLandmarkNameL(newAddr);
    
    CleanupStack::PopAndDestroy( &newAddr );
    }

// --------------------------------------------------------------------------
// CPmapCmd::FillGeoLandmarkL
// --------------------------------------------------------------------------
//
TBool CPmapCmd::FillGeoLandmarkL(
		CPosLandmark& aLandmark, TVPbkFieldTypeParameter aAddressType )
	{
	TBool result = EFalse;
	MVPbkStoreContactFieldCollection& arrFields = iContact->Fields();
	TInt count = arrFields.FieldCount();
	for( TInt idx = 0; idx < count; idx++ )
		{
		MVPbkStoreContactField& field = arrFields.FieldAt( idx );
		const MVPbkContactFieldData& fieldData = field.FieldData();
		if( fieldData.DataType() != EVPbkFieldStorageTypeText )
			{
			continue;
			}
		const TDesC& dataText =
		    MVPbkContactFieldTextData::Cast( fieldData ).Text();
		const MVPbkFieldType* type = field.BestMatchingFieldType();
		if ( type )
		    {
		    TInt countProps = type->VersitProperties().Count();
            TArray<TVPbkFieldVersitProperty> props = type->VersitProperties();
            for (TInt idx2 = 0; idx2 < countProps; idx2++ )
                {
                if ( (props[idx2].Parameters().Contains( EVPbkVersitParamHOME )
                    && aAddressType == EVPbkVersitParamHOME)
                    || (props[idx2].Parameters().Contains( EVPbkVersitParamWORK ) 
                        && aAddressType == EVPbkVersitParamWORK)
                    || (!props[idx2].Parameters().Contains( EVPbkVersitParamHOME )
                        && !props[idx2].Parameters().Contains( EVPbkVersitParamWORK ) 
                        && aAddressType == EVPbkVersitParamPREF) )
                    {
                    result = DoFillGeoLandmarkL( aLandmark, dataText );
                    break;
                    }
                }
		    }
		}
	return result;
	}

// --------------------------------------------------------------------------
// CPmapCmd::DoFillGeoLandmarkL
// --------------------------------------------------------------------------
//
TBool CPmapCmd::DoFillGeoLandmarkL(
		CPosLandmark& aLandmark, const TDesC& aDataText )
	{
	TBool result = EFalse;
	TInt separator = aDataText.Find( KGeoSeparator );
	if ( separator != KErrNotFound )
		{
		TReal64 latitude = 0;
		TReal64 logitude = 0;
        TLex lexLatitude( aDataText.Left( separator ) );
		TLex lexLogitude( aDataText.Right( aDataText.Length()
				- separator - KGeoSeparatorLength ) );
		if ( lexLatitude.Val( latitude ) == KErrNone
				&& lexLogitude.Val( logitude ) == KErrNone )
			{
			TLocality loc( TCoordinate( latitude, logitude ),
					KGeoHorizontalAccuracy );
			aLandmark.SetPositionL( loc );
            TInt separator2 = aDataText.Right( aDataText.Length() - separator - KGeoSeparatorLength ).Find( KGeoSeparator );
            if(separator2 != KErrNotFound)
                {
                separator += separator2;
                aLandmark.SetPositionFieldL(EPositionFieldCountryCode,aDataText.Right( aDataText.Length() - separator - (KGeoSeparatorLength * 2) ));
                }
            result = ETrue;
            }
		}
	return result;
	}

// --------------------------------------------------------------------------
// CPmapCmd::EditorAssignFromMapsL
// --------------------------------------------------------------------------
//
void CPmapCmd::EditorAssignFromMapsL(TVPbkFieldTypeParameter aAddressType)
	{
    if(iMapView)
        {
        delete iMapView;
        iMapView = NULL;
        }
    
    iMapView = CMnMapView::NewChainedL( *iMapViewProvider );
    iAddressType = aAddressType;
    
    RPointerArray<CPosLandmark> landmarks;
    CleanupClosePushL( landmarks );

    CPosLandmark* landmark = CPosLandmark::NewL();
	CleanupStack::PushL( landmark );
	
	if ( !FillGeoLandmarkL( *landmark, aAddressType ) )
		{
		FillLandmarkL( *landmark, aAddressType );
		}
	TLocality locality;
	iNoAddress = ETrue;
	if( landmark->NumOfAvailablePositionFields() > 0 || landmark->GetPosition( locality ) == KErrNone )
		{
		landmarks.AppendL( landmark );
		iMapView->AddLandmarksToShowL( landmarks );
		iNoAddress = EFalse;
		}
    else
        {
        landmark->SetPositionFieldL(EPositionFieldCity,KNullDesC16);
        TLocality tempLoc;
        tempLoc.SetCoordinate(-0.0,-0.0);
        landmark->SetPositionL(tempLoc);
        
        landmarks.AppendL( landmark );
        iMapView->AddLandmarksToShowL( landmarks );
        }

	iMapView->SelectFromMapL( iStatus );
	iCurrentMapLaunchedByState = EMapAssignFromMaps;
	SetActive();
	CleanupStack::PopAndDestroy( landmark );
    CleanupStack::PopAndDestroy( &landmarks );
	}

// --------------------------------------------------------------------------
// CPmapCmd::UpdateCoordsL
// --------------------------------------------------------------------------
//
void CPmapCmd::UpdateCoordsL( 
        const CPosLandmark& aLandmark,
		TVPbkFieldTypeParameter aAddressType )
	{
    TLocality locality;
    aLandmark.GetPosition(locality);   
    TReal64 latitude = locality.Latitude();
    TReal64 longitude = locality.Longitude();
	RBuf geoData;
	RBuf textNumber;
	CleanupClosePushL( geoData );
	CleanupClosePushL( textNumber );
	geoData.CreateL( KGeoMaxSize );
	textNumber.CreateL( KGeoMaxSize );
	TRealFormat format( KGeoFormatWidth );
	textNumber.Num( latitude, format );
	geoData = textNumber;
	geoData += KGeoSeparator();
	textNumber.Num( longitude, format );
	geoData += textNumber;

    if(aLandmark.IsPositionFieldAvailable( EPositionFieldCountryCode ))
        {
        TPtrC countryCode;
        aLandmark.GetPositionField(EPositionFieldCountryCode,countryCode);
        geoData += KGeoSeparator();
        geoData += countryCode;
        }
	CleanupStack::PopAndDestroy( &textNumber );
	
	TBool updated = EFalse;
	MVPbkStoreContactFieldCollection& arrFields = iContact->Fields();
	TInt count = arrFields.FieldCount();
	for( TInt idx = 0; idx < count; idx++)
		{
		MVPbkStoreContactField& field = arrFields.FieldAt(idx);
		const MVPbkFieldType* type = field.BestMatchingFieldType();
		if ( type )
		    {		    
		    TInt countProps = type->VersitProperties().Count();
            TArray<TVPbkFieldVersitProperty> props = type->VersitProperties();
            for (TInt idx2 = 0; idx2 < countProps; idx2++ )
                {
                if ( props[idx2].Name() == EVPbkVersitNameGEO
                    && ((props[idx2].Parameters().Contains( EVPbkVersitParamHOME ) 
                        && aAddressType == EVPbkVersitParamHOME)
                        || (props[idx2].Parameters().Contains( EVPbkVersitParamWORK ) 
                            && aAddressType == EVPbkVersitParamWORK)
                        || (!props[idx2].Parameters().Contains( EVPbkVersitParamHOME )
                            && !props[idx2].Parameters().Contains( EVPbkVersitParamWORK ) 
                            && aAddressType == EVPbkVersitParamPREF)) )
                    {
                    MVPbkContactFieldData& fieldData = field.FieldData();
                    MVPbkContactFieldTextData& data =
                        MVPbkContactFieldTextData::Cast( fieldData );
                    data.SetTextL( geoData );
                    updated = ETrue;
                    break;
                    }
                }
		    }
		}
	
	if( !updated )
		{
		MVPbkContactStore& storeContact = iContact->ContactStore();
		const MVPbkContactStoreProperties& storeProp = storeContact.StoreProperties();
		const MVPbkFieldTypeList& fieldList = storeProp.SupportedFields();
		TInt count = fieldList.FieldTypeCount();
		for( TInt idx = 0; idx < count; idx++)
			{
			const MVPbkFieldType& fieldType = fieldList.FieldTypeAt( idx );
			TInt countProps = fieldType.VersitProperties().Count();
			TArray<TVPbkFieldVersitProperty> props =
			    fieldType.VersitProperties();
			for ( TInt idx2 = 0; idx2 < countProps; idx2++ )
				{
				if ( props[ idx2 ].Name() == EVPbkVersitNameGEO &&
				     ( ( props[ idx2 ].Parameters().Contains( EVPbkVersitParamHOME ) &&
					   aAddressType == EVPbkVersitParamHOME ) ||
					   ( props[ idx2 ].Parameters().Contains( EVPbkVersitParamWORK ) &&
			             aAddressType == EVPbkVersitParamWORK ) ||
			             ( !props[ idx2 ].Parameters().Contains( EVPbkVersitParamHOME ) &&
			               !props[ idx2 ].Parameters().Contains( EVPbkVersitParamWORK ) &&
			               aAddressType == EVPbkVersitParamPREF ) ) )
					{
					MVPbkStoreContactField* contactField = iContact->CreateFieldLC( fieldType );
					if( contactField )
					    {
					    MVPbkContactFieldData& fieldData = contactField->FieldData();
					    if( fieldData.DataType() == EVPbkFieldStorageTypeText )
					    	{
					    	MVPbkContactFieldTextData& data = 
					    		MVPbkContactFieldTextData::Cast( fieldData );
					    	data.SetTextL( geoData );
					    	iContact->AddFieldL( contactField );
					    	}
					    CleanupStack::Pop( contactField );
					    }
					break;
					}
				}
			}	
		}
	CleanupStack::PopAndDestroy( &geoData );
	}

// --------------------------------------------------------------------------
// CPmapCmd::UpdateFieldL
// --------------------------------------------------------------------------
//
void CPmapCmd::UpdateFieldL( 
		const CPosLandmark& aLandmark, 
		_TPositionFieldId aPositionField, 
		TVPbkSubFieldType aVersitSubField, 
		TVPbkFieldTypeParameter aAddressType )
	{
	TBool updated = EFalse;
	TPtrC textData;
	if ( aPositionField != EPositionFieldNone && aLandmark.IsPositionFieldAvailable( aPositionField ) )
        {
        aLandmark.GetPositionField( aPositionField, textData );
        }
	
	if( iAddressView && iEditorControl )
		{
		TInt countControls = iEditorControl->NumberOfControls();
		for ( TInt i = 0; i < countControls; ++i )
			{
			TArray<TVPbkFieldVersitProperty> props = iEditorControl->EditorField( i ).FieldProperty().FieldType().VersitProperties();
			TInt countProperties = props.Count();
			for ( TInt ii = 0; ii < countProperties; ++ii )
				{
				if( props[ ii ].Name() == EVPbkVersitNameADR && props[ ii ].SubField() == aVersitSubField )
					{
					if ( aLandmark.IsPositionFieldAvailable( aPositionField ) )
						{
						iEditorControl->EditorField( i ).Control()->SetTextL( &textData );
						}
					else
						{
						iEditorControl->EditorField( i ).Control()->SetTextL( &KNullDesC() );
						}
						
					iEditorControl->EditorField( i ).SaveFieldL();
					updated = ETrue;
					continue;
					}
				}
			}
		}
	else 
		{
		MVPbkStoreContactFieldCollection& arrFields = iContact->Fields();
		TInt count = arrFields.FieldCount();
		for( TInt idx = 0; idx < count; idx++)
			{
			MVPbkStoreContactField& field = arrFields.FieldAt(idx);
			const MVPbkFieldType* type = field.BestMatchingFieldType();
			if ( type )
			    {
                TArray<TVPbkFieldVersitProperty> arrProp = type->VersitProperties();
                for (TInt idx2 = 0; idx2 < arrProp.Count(); idx2++ )
                    {
                    if ( arrProp[idx2].Name() == EVPbkVersitNameADR
                        && arrProp[idx2].SubField() == aVersitSubField
                        && (arrProp[idx2].Parameters().Contains( aAddressType )
                            || (aAddressType == EVPbkVersitParamPREF
                                && !arrProp[idx2].Parameters().Contains( EVPbkVersitParamHOME )
                                && !arrProp[idx2].Parameters().Contains( EVPbkVersitParamWORK ))) )
                        {
                        MVPbkContactFieldData& fieldData = field.FieldData();
                        if ( fieldData.DataType() != EVPbkFieldStorageTypeText )
                            {
                            continue;
                            }
                        MVPbkContactFieldTextData& data =
                            MVPbkContactFieldTextData::Cast( fieldData );
                        if ( aLandmark.IsPositionFieldAvailable( aPositionField ) )
                            {
                            data.SetTextL( textData );
                            }
                        else
                            {
                            data.SetTextL( KNullDesC() );
                            }

                        updated = ETrue;
                        continue;
                        }
                    }
			    }
			}
		}
	
	if( !updated && aLandmark.IsPositionFieldAvailable( aPositionField ) )
		{
		MVPbkContactStore& storeContact = iContact->ContactStore();
		const MVPbkContactStoreProperties& storeProp = storeContact.StoreProperties();
		const MVPbkFieldTypeList& fieldList = storeProp.SupportedFields();
		TInt count = fieldList.FieldTypeCount();
		for( TInt idx = 0; idx < count; idx++)
			{
			const MVPbkFieldType& fieldType = fieldList.FieldTypeAt( idx );
			TArray<TVPbkFieldVersitProperty> arrProp = fieldType.VersitProperties();
			for( TInt idx2 = 0; idx2 < arrProp.Count(); idx2++)
				{
				if( arrProp[idx2].Name() == EVPbkVersitNameADR && 
					arrProp[idx2].SubField() == aVersitSubField &&
					( arrProp[idx2].Parameters().Contains(aAddressType) ||
					  ( aAddressType == EVPbkVersitParamPREF && 
						!arrProp[idx2].Parameters().Contains(EVPbkVersitParamHOME) &&
						!arrProp[idx2].Parameters().Contains(EVPbkVersitParamWORK) ) ) )
					{
					MVPbkStoreContactField* contactField = iContact->CreateFieldLC( fieldType );
					if( contactField )
						{
						MVPbkContactFieldData& fieldData = contactField->FieldData();
						if( fieldData.DataType() == EVPbkFieldStorageTypeText )
							{
							MVPbkContactFieldTextData& data = 
							   MVPbkContactFieldTextData::Cast( fieldData );
							data.SetTextL(textData);
							iContact->AddFieldL( contactField );
							}
						CleanupStack::Pop(contactField);
						}
					break;
					}
				}
			}	
		}
	}

// --------------------------------------------------------------------------
// CPmapCmd::IsAddressInContact
// --------------------------------------------------------------------------
//
TBool CPmapCmd::IsAddressInContact()
	{
	MVPbkStoreContactFieldCollection& arrFields = iContact->Fields();
	TInt count = arrFields.FieldCount();
	for( TInt idx = 0; idx < count; idx++)
		{
		MVPbkStoreContactField& field = arrFields.FieldAt(idx);
		const MVPbkFieldType* type = field.BestMatchingFieldType();
		if ( type )
		    {
		    TArray<TVPbkFieldVersitProperty> arrProp = type->VersitProperties();
		    for( TInt idx2 = 0; idx2 < arrProp.Count(); idx2++)
			    {
			    if( arrProp[idx2].Name() == EVPbkVersitNameADR  )
			    	{
				    return ETrue;
				    }
			    }
			}
		}
	return EFalse;
    }

// --------------------------------------------------------------------------
// CPmapCmd::GetFieldGroupL
// --------------------------------------------------------------------------
//
TPbk2FieldGroupId CPmapCmd::GetFieldGroupL(
		MVPbkStoreContactField& aField )
	{
	TPbk2FieldGroupId ret = EPbk2FieldGroupIdNone;

	const MVPbkFieldType* type = aField.BestMatchingFieldType();
	if( type )
	    {
	    TArray<TVPbkFieldVersitProperty> arrProp = type->VersitProperties();
	    for( TInt idx = 0; idx < arrProp.Count(); idx++)
			{
			if( arrProp[idx].Name() != EVPbkVersitNameADR )
				{
				continue;
				}
					
			if( arrProp[idx].Parameters().Contains( 
				    Pbk2AddressTools::MapAddressToVersitParam(
				    		EPbk2FieldGroupIdHomeAddress ) ) )
				{
				ret = EPbk2FieldGroupIdHomeAddress;
				break;
				}
			else if( arrProp[idx].Parameters().Contains( 
				    Pbk2AddressTools::MapAddressToVersitParam(
				    		EPbk2FieldGroupIdCompanyAddress ) ) )
				{
				ret = EPbk2FieldGroupIdCompanyAddress;
				break;
				}
			else
				{
				ret = EPbk2FieldGroupIdPostalAddress;
				break;
				}
			}
	    }

	return ret;
    }

// --------------------------------------------------------------------------
// CPmapCmd::ContactOperationCompleted
// --------------------------------------------------------------------------
//
void CPmapCmd::ContactOperationCompleted
        (TContactOpResult aResult)
    {
    if (aResult.iStoreContact)
        {
        iContact = aResult.iStoreContact;
        }
    
   	CActiveScheduler::Stop();
    }

// --------------------------------------------------------------------------
// CPmapCmd::HandleSelectiOnAssignFromMapsL
// --------------------------------------------------------------------------
//
void CPmapCmd::HandleSelectiOnAssignFromMapsL()
    {
    TBool update = EFalse;
    TBool geocoordsExist = EFalse;
    
    TLocality locality;
    CPosLandmark* result = NULL;
    TInt currentstatus = iStatus.Int();
    if ( iStatus.Int() == KErrNone && iMapView)
        {
        result = iMapView->RetrieveSelectionResultL();
        // Get the location name if available
        //-----------------Emulator Hack-------------------------------
     // result->SetLandmarkNameL(_L("Jupiter Tech Park"));
//      result->SetPositionFieldL(EPositionFieldCity, _L("Bangalore"));
//      result->SetPositionFieldL(EPositionFieldCountry, _L("India"));
        //-------------------------------------------------------------
        if(result->GetPosition( locality ) == KErrNone )
            {
            geocoordsExist = ETrue;
            }     
          
        if( result->IsPositionFieldAvailable( EPositionFieldStreet ) || 
            result->IsPositionFieldAvailable( EPositionFieldCity ) ||
            result->IsPositionFieldAvailable( EPositionFieldCountry ) )
            {
            update= ETrue;
            }
        }
    delete iMapView;
    iMapView = NULL;
    if( result )
        {
        CleanupStack::PushL( result );
        }
    
    if( update )
        {
        if( iAddressUpdatePrompt && !iNoAddress )
            {
            const TInt KGranularity = 4; 
            CDesCArrayFlat* arrFields = new ( ELeave ) CDesCArrayFlat
                    ( KGranularity );
            CleanupStack::PushL(arrFields);
            
            HBufC* addrLebel = NULL;
            
            switch( iAddressType )
                {
                case EVPbkVersitParamPREF:
                    addrLebel = StringLoader::LoadLC(R_QTN_PHOB_HEADER_ADDRESS );
                    break;
                case EVPbkVersitParamHOME:
                    addrLebel = StringLoader::LoadLC( R_QTN_PHOB_HEADER_ADDRESS_HOME );
                    break;
                case EVPbkVersitParamWORK:
                    addrLebel = StringLoader::LoadLC( R_QTN_PHOB_HEADER_ADDRESS_WORK );
                    break;
                default:
                    User::Leave(KErrArgument);
                }
            arrFields->AppendL( addrLebel->Des() );
            
            _LIT(KComma, ",");
            _LIT(KSpace, " ");
            
            TInt lenght = 0; 
            TPtrC street;
            if( result->IsPositionFieldAvailable( EPositionFieldStreet ) )
                {
                result->GetPositionField( EPositionFieldStreet, street );
                lenght += street.Length();
                }
            
            TPtrC city;
            if( result->IsPositionFieldAvailable( EPositionFieldCity ) )
                {
                result->GetPositionField( EPositionFieldCity, city );
                if( lenght )
                    {
                    lenght += KComma().Length() + KSpace().Length();
                    }
                lenght += city.Length();
                }   
            
            TPtrC country;
            if( result->IsPositionFieldAvailable( EPositionFieldCountry ) )
                {
                result->GetPositionField( EPositionFieldCountry, country );
                if( lenght )
                    {
                    lenght += KComma().Length() + KSpace().Length();
                    }
                lenght += country.Length();
                }   
            
            RBuf newAddr;
            newAddr.CreateL( lenght );
            CleanupClosePushL( newAddr );
            
            if( street.Length() )
                {
                newAddr += street;
                }
            
            if( city.Length() )
                {
                if( newAddr.Length() > 0 )
                    {
                    newAddr += KComma();
                    newAddr += KSpace();
                    }
                newAddr += city;
                }
            
            if( country.Length() )
                {
                if( newAddr.Length() > 0 )
                    {
                    newAddr += KComma();
                    newAddr += KSpace();
                    }
                newAddr += country;
                }
    
            arrFields->AppendL( newAddr );
            HBufC* prompt = StringLoader::LoadLC( R_QTN_PHOB_CONFIRM_CHANGE_ADDRESS, *arrFields );
            CAknQueryDialog* dlg = CAknQueryDialog::NewL();
            if( !dlg->ExecuteLD( R_PBK2_GENERAL_CONFIRMATION_QUERY, *prompt ) )
                {
                update = EFalse;
                }
            CleanupStack::PopAndDestroy( prompt );
            CleanupStack::PopAndDestroy( &newAddr );
            CleanupStack::PopAndDestroy( addrLebel );
            CleanupStack::PopAndDestroy( arrFields );
            }
            
        if( update )
            {
            if ( !iEditorControl )
                {
                iContact->LockL( *this );
                CActiveScheduler::Start();
                }
            UpdateFieldL( *result, EPositionFieldCountry, 
                            EVPbkVersitSubFieldCountry, iAddressType );
            UpdateFieldL( *result, EPositionFieldCity, 
                            EVPbkVersitSubFieldLocality, iAddressType );
            UpdateFieldL( *result, EPositionFieldStreet, 
                            EVPbkVersitSubFieldStreet, iAddressType );
            UpdateFieldL( *result, EPositionFieldPostalCode, 
                            EVPbkVersitSubFieldPostalCode, iAddressType );
            UpdateFieldL( *result, EPositionFieldLocality, 
                            EVPbkVersitSubFieldRegion, iAddressType );
            UpdateFieldL( *result, EPositionFieldNone, 
                            EVPbkVersitSubFieldPostOfficeAddress, iAddressType );
            UpdateFieldL( *result, EPositionFieldNone, 
                            EVPbkVersitSubFieldExtendedAddress, iAddressType );
            
            if( geocoordsExist )
                {
                UpdateCoordsL( *result, iAddressType);
                }
            if ( !iEditorControl )
                {
                iContact->CommitL( *this );
                CActiveScheduler::Start();
                }
            if( ControlExtension() )
                {
                ControlExtension()->UpdateControlsL();
                }
            
            HBufC* text = StringLoader::LoadLC( R_QTN_PHOB_NOTE_ADDRESS_UPDATED );
            CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue );
            note->ExecuteLD( *text );
            CleanupStack::PopAndDestroy( text );
            }
        }
    
    if( result )
        {
        CleanupStack::PopAndDestroy( result );
        }
    }

// --------------------------------------------------------------------------
// CPmapCmd::HandleSelectiOnAssignFromMapsL
// --------------------------------------------------------------------------
//
void CPmapCmd::HandleSelectiOnShowOnMapsL()
    {
    TBool update = EFalse;
    TBool geocoordsExist = EFalse;

    TLocality locality;
    CPosLandmark* result = NULL;
    if ( iStatus.Int() == KErrNone && iMapView)
        {
        result = iMapView->RetrieveSelectionResultL();
        if ( result->GetPosition( locality ) == KErrNone )
            {
            geocoordsExist = ETrue;
            }
        if ( result->IsPositionFieldAvailable( EPositionFieldStreet )
            || result->IsPositionFieldAvailable( EPositionFieldCity )
            || result->IsPositionFieldAvailable( EPositionFieldCountry ) )
            {
            update = ETrue;
            }

        }
   
    delete iMapView;
    iMapView = NULL;
    if ( result )
        {
        CleanupStack::PushL( result );
        }

    if ( update )
        {

        const TInt KGranularity = 4;
        CDesCArrayFlat* arrFields = new (ELeave) CDesCArrayFlat(
            KGranularity );
        CleanupStack::PushL( arrFields );
        HBufC* addrLebel = NULL;

        switch ( iAddressType )
            {
            case EVPbkVersitParamPREF:
                addrLebel = StringLoader::LoadLC(
                    R_QTN_PHOB_HEADER_ADDRESS );
                break;
            case EVPbkVersitParamHOME:
                addrLebel = StringLoader::LoadLC(
                    R_QTN_PHOB_HEADER_ADDRESS_HOME );
                break;
            case EVPbkVersitParamWORK:
                addrLebel = StringLoader::LoadLC(
                    R_QTN_PHOB_HEADER_ADDRESS_WORK );
                break;
            default:
                User::Leave( KErrArgument );
            }
        arrFields->AppendL( addrLebel->Des() );
        _LIT(KComma, ",");
        _LIT(KSpace, " ");
        TInt lenght = 0;
        TPtrC street;
        if ( result->IsPositionFieldAvailable( EPositionFieldStreet ) )
            {
            result->GetPositionField( EPositionFieldStreet, street );
            lenght += street.Length();
            }

        TPtrC city;
        if ( result->IsPositionFieldAvailable( EPositionFieldCity ) )
            {
            result->GetPositionField( EPositionFieldCity, city );
            if ( lenght )
                {
                lenght += KComma().Length() + KSpace().Length();
                }
            lenght += city.Length();
            }

        TPtrC country;
        if ( result->IsPositionFieldAvailable( EPositionFieldCountry ) )
            {
            result->GetPositionField( EPositionFieldCountry, country );
            if ( lenght )
                {
                lenght += KComma().Length() + KSpace().Length();
                }
            lenght += country.Length();
            }

        RBuf newAddr;
        newAddr.CreateL( lenght );
        CleanupClosePushL( newAddr );

        if ( street.Length() )
            {
            newAddr += street;
            }

        if ( city.Length() )
            {
            if ( newAddr.Length() > 0 )
                {
                newAddr += KComma();
                newAddr += KSpace();
                }
            newAddr += city;
            }

        if ( country.Length() )
            {
            if ( newAddr.Length() > 0 )
                {
                newAddr += KComma();
                newAddr += KSpace();
                }
            newAddr += country;
            }

        arrFields->AppendL( newAddr );
        HBufC* prompt = StringLoader::LoadLC(
            R_QTN_PHOB_CONFIRM_CHANGE_ADDRESS, *arrFields );
        CAknQueryDialog* dlg = CAknQueryDialog::NewL();
        if ( !dlg->ExecuteLD( R_PBK2_GENERAL_CONFIRMATION_QUERY, *prompt ) )
            {
            update = EFalse;
            }
        CleanupStack::PopAndDestroy( prompt );
        CleanupStack::PopAndDestroy( &newAddr );
        CleanupStack::PopAndDestroy( addrLebel );
        CleanupStack::PopAndDestroy( arrFields );

        }
    if ( update )
        {
        if ( !iEditorControl )
            {
            iContact->LockL( *this );
            CActiveScheduler::Start();
            }
        UpdateFieldL( *result, EPositionFieldCountry,
            EVPbkVersitSubFieldCountry, iAddressType );
        UpdateFieldL( *result, EPositionFieldCity,
            EVPbkVersitSubFieldLocality, iAddressType );
        UpdateFieldL( *result, EPositionFieldStreet,
            EVPbkVersitSubFieldStreet, iAddressType );
        UpdateFieldL( *result, EPositionFieldPostalCode,
            EVPbkVersitSubFieldPostalCode, iAddressType );
        UpdateFieldL( *result, EPositionFieldLocality,
            EVPbkVersitSubFieldRegion, iAddressType );
        UpdateFieldL( *result, EPositionFieldNone,
            EVPbkVersitSubFieldPostOfficeAddress, iAddressType );
        UpdateFieldL( *result, EPositionFieldNone,
            EVPbkVersitSubFieldExtendedAddress, iAddressType );

        if ( geocoordsExist )
            {
            UpdateCoordsL( *result, iAddressType );
            }
        if ( !iEditorControl )
            {
            iContact->CommitL( *this );
            CActiveScheduler::Start();
            }
        if ( ControlExtension() )
            {
            ControlExtension()->UpdateControlsL();
            }

        HBufC* text = StringLoader::LoadLC(
            R_QTN_PHOB_NOTE_ADDRESS_UPDATED );
        CAknInformationNote* note = new (ELeave) CAknInformationNote(
            ETrue );
        note->ExecuteLD( *text );
        CleanupStack::PopAndDestroy( text );
        }

    if ( result )
        {
        CleanupStack::PopAndDestroy( result );
        }
    }

// --------------------------------------------------------------------------
// CPmapCmd::ContactOperationFailed
// --------------------------------------------------------------------------
//
void CPmapCmd::ContactOperationFailed
        (TContactOp /*aOpCode*/, TInt /*aErrorCode*/, TBool /*aErrorNotified*/)
    {
   	CActiveScheduler::Stop();
    }

// ----------------------------------------------------------------------------
// CPmapCmd::RunL
// RunL method to handle the user selection
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//    
void CPmapCmd::RunL()
    {
    // request is completed, inform observer
    TInt maperror = iStatus.Int();
    if(iStatus.Int() == KErrNone)
        {
        switch(iCurrentMapLaunchedByState)
            {
            case EMapAssignFromMaps:
                {
                HandleSelectiOnAssignFromMapsL();
                }
                break;
            case EMapShowOnMaps:
                {
                HandleSelectiOnShowOnMapsL();
                }
                break;
            default:
                break;
            }
        }
    FinishProcess();

    if(!iUiControl)
       {
     //  delete this;
       }
    iCurrentMapLaunchedByState = EMapNone;
    }

// ----------------------------------------------------------------------------
// CPmapCmd::RunError
// Function to handle any errors in async request
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//    
TInt CPmapCmd::RunError( TInt /*aError*/ )
    {
    Reset();
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CPmapCmd::DoCancel
// Cancel method to handle the user selection
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CPmapCmd::DoCancel()
    {
    if ( IsActive() && iMapView )
        {
        iMapView->Cancel();
        }
    Reset();
    }

// ----------------------------------------------------------------------------
// CPmapCmd::Reset
// Disconnects from provider, when operation is completed
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CPmapCmd::Reset()
    {
    if(iMapView)
        {
        iMapView->ResetLandmarksToShow();
        delete iMapView;
        iMapView = NULL;    
        }
    ReleaseLandmarkResources();
    }

// End of File
