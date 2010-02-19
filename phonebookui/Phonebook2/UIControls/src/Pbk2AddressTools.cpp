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
* Description:  Phonebook 2 contact address tools.
*
*/

#include "Pbk2AddressTools.h"

// Phonebook 2
#include <Pbk2UIControls.rsg>
#include "Pbk2UIFieldProperty.hrh"

// Virtual Phonebook
#include <MVPbkBaseContactField.h>
#include <MVPbkStoreContact.h>
#include <MVPbkFieldType.h>
#include <TVPbkFieldVersitProperty.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <TVPbkFieldTypeParameters.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactFieldTextData.h>

//System
#include <badesca.h>
#include <StringLoader.h>

// --------------------------------------------------------------------------
// Pbk2AddressTools::MapCtrlTypeToAddress
// --------------------------------------------------------------------------
//
EXPORT_C TPbk2FieldGroupId Pbk2AddressTools::MapCtrlTypeToAddress(TInt aCtrlType)
    {
    TPbk2FieldGroupId ret = EPbk2FieldGroupIdNone;

	switch( aCtrlType )
		{
		case EPbk2FieldCtrlTypeExtAddressEditor:
			ret =  EPbk2FieldGroupIdPostalAddress;
			break;
		case EPbk2FieldCtrlTypeExtAddressHomeEditor:
			ret =  EPbk2FieldGroupIdHomeAddress;
			break;
		case EPbk2FieldCtrlTypeExtAddressOfficeEditor:
			ret =  EPbk2FieldGroupIdCompanyAddress;
			break;
		}

    return ret;
    }

// --------------------------------------------------------------------------
// Pbk2AddressTools::MapCtrlTypeToAddress
// --------------------------------------------------------------------------
//
EXPORT_C TPbk2FieldGroupId Pbk2AddressTools::MapViewTypeToAddress(TPbk2ContactEditorParams::TActiveView aViewType)
    {
    TPbk2FieldGroupId ret = EPbk2FieldGroupIdNone;

	switch( aViewType )
		{
		case TPbk2ContactEditorParams::EEditorAddressView:
			ret =  EPbk2FieldGroupIdPostalAddress;
			break;
		case TPbk2ContactEditorParams::EEditorAddressHomeView:
			ret =  EPbk2FieldGroupIdHomeAddress;
			break;
		case TPbk2ContactEditorParams::EEditorAddressOfficeView:
			ret =  EPbk2FieldGroupIdCompanyAddress;
			break;
		}

    return ret;
    }

// --------------------------------------------------------------------------
// Pbk2AddressTools::MapCtrlTypeToAddress
// --------------------------------------------------------------------------
//
EXPORT_C TInt Pbk2AddressTools::MapAddressToOrdering(TPbk2FieldGroupId aGroupAddress)
    {
    TPbk2FieldOrder ret = EPbk2FieldOrderUndefinied;

	switch( aGroupAddress )
		{
		case EPbk2FieldGroupIdPostalAddress:
			ret =  EPbk2FieldOrderAddress;
			break;
		case EPbk2FieldGroupIdHomeAddress:
			ret =  EPbk2FieldOrderAddressHome;
			break;
		case EPbk2FieldGroupIdCompanyAddress:
			ret =  EPbk2FieldOrderAddressOffice;
			break;
		}

    return ret;
    }

// --------------------------------------------------------------------------
// Pbk2AddressTools::MapCtrlTypeToViewType
// --------------------------------------------------------------------------
//
EXPORT_C TPbk2ContactEditorParams::TActiveView Pbk2AddressTools::MapCtrlTypeToViewType(TInt aCtrlType)
    {
    TPbk2ContactEditorParams::TActiveView ret = TPbk2ContactEditorParams::EEditorUndefinied;

	switch( aCtrlType )
		{
		case EPbk2FieldCtrlTypeExtAddressEditor:
			ret =  TPbk2ContactEditorParams::EEditorAddressView;
			break;
		case EPbk2FieldCtrlTypeExtAddressHomeEditor:
			ret =  TPbk2ContactEditorParams::EEditorAddressHomeView;
			break;
		case EPbk2FieldCtrlTypeExtAddressOfficeEditor:
			ret =  TPbk2ContactEditorParams::EEditorAddressOfficeView;
			break;
		}

    return ret;
    }

// --------------------------------------------------------------------------
// Pbk2AddressTools::MapAddressToVersitParam
// --------------------------------------------------------------------------
//
EXPORT_C TVPbkFieldTypeParameter Pbk2AddressTools::MapAddressToVersitParam( TPbk2FieldGroupId aGroupId )
	{
	TVPbkFieldTypeParameter addressParameter = EVPbkVersitParamPREF;
	switch(aGroupId)
		{
		case EPbk2FieldGroupIdPostalAddress:
			addressParameter = EVPbkVersitParamPREF;
			break;
		case EPbk2FieldGroupIdHomeAddress:
			addressParameter = EVPbkVersitParamHOME;
			break;
		case EPbk2FieldGroupIdCompanyAddress:
			addressParameter = EVPbkVersitParamWORK;
			break;
		}
	return addressParameter;
	}

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::GetAddressPreviewLC
// --------------------------------------------------------------------------
//
EXPORT_C void Pbk2AddressTools::GetAddressPreviewLC(
		MVPbkStoreContact& aContact,
		TPbk2FieldGroupId aAddressGroup,
		RBuf& aText )
    {
    if( aAddressGroup != EPbk2FieldGroupIdPostalAddress &&
		aAddressGroup != EPbk2FieldGroupIdCompanyAddress &&
		aAddressGroup != EPbk2FieldGroupIdHomeAddress )
    	{
    	User::Leave(KErrArgument);
    	}

    const TInt KGranularity = 4;
	CDesCArrayFlat* arrFields = new ( ELeave ) CDesCArrayFlat
            ( KGranularity );
    CleanupStack::PushL(arrFields);

    RHashMap<TInt, TPtrC> aFieldsMap;
    GetAddressFieldsLC( aContact, aAddressGroup, aFieldsMap );

    TBool appended = EFalse;
    TPtrC* data = NULL;
    data = aFieldsMap.Find( EVPbkVersitSubFieldStreet );
    if( data )
    	{
    	arrFields->AppendL( *data );
    	appended = ETrue;
    	}
    else
    	{
    	arrFields->AppendL( KNullDesC() );
    	}

    data = aFieldsMap.Find( EVPbkVersitSubFieldLocality );
	if( data )
		{
		arrFields->AppendL( *data );
		appended = ETrue;
		}
	else
		{
		arrFields->AppendL( KNullDesC() );
		}

	data = aFieldsMap.Find( EVPbkVersitSubFieldRegion );
	if( data )
		{
		arrFields->AppendL( *data );
		appended = ETrue;
		}
	else
		{
		arrFields->AppendL( KNullDesC() );
		}

	data = aFieldsMap.Find( EVPbkVersitSubFieldCountry );
	if( data )
		{
		arrFields->AppendL( *data );
		appended = ETrue;
		}
	else
		{
		arrFields->AppendL( KNullDesC() );
		}

    _LIT(KComma, ",");
    _LIT(KSpace, " ");

    HBufC* addressPreview = NULL;

    if( !appended )
    	{
    	if( aFieldsMap.Count() )
    		{
    		addressPreview = StringLoader::LoadLC( R_QTN_PHOB_POPUP_INCOMPLETE_ADDRESS );
    		}
    	else
    		{
    		addressPreview = StringLoader::LoadLC( R_QTN_PHOB_CONTACT_EDITOR_DEFINE );
    		}
     	}
    else
    	{
		addressPreview = StringLoader::LoadLC( R_QTN_PHOB_ADDRESS_PREVIEW, *arrFields );
		TPtr16 addressPreviewTPtr = addressPreview->Des();

		TBool prevField = EFalse;
		TInt idx = 0;
		while( idx < addressPreviewTPtr.Length() )
			{
			if( addressPreviewTPtr[idx] == KComma()[0] )
				{
				if( !prevField )
					{
					addressPreviewTPtr.Delete( idx, 1 );
					if( idx < addressPreviewTPtr.Length() && addressPreviewTPtr[idx] == KSpace()[0] )
						{
						addressPreviewTPtr.Delete( idx, 1 );
						}
					continue;
					}
				else
					{
					prevField = EFalse;
					}
				}
			else if( !(addressPreviewTPtr[idx] == KSpace()[0] && !prevField) )
				{
				prevField = ETrue;
				}
			idx++;
			}

		TInt lenght = addressPreviewTPtr.Length();
		if( lenght > 2 && addressPreviewTPtr[lenght - 2] == KComma()[0] )
			{
			addressPreviewTPtr.Delete( lenght - 2, 2 );
			}
    	}
    CleanupStack::Pop( addressPreview );
    CleanupStack::PopAndDestroy( &aFieldsMap );
	CleanupStack::PopAndDestroy( arrFields );

	aText.Assign( addressPreview );
	CleanupClosePushL( aText );
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::GetAddressShortPreviewLC
// --------------------------------------------------------------------------
//
EXPORT_C void Pbk2AddressTools::GetAddressShortPreviewLC(
		MVPbkStoreContact& aContact,
		TPbk2FieldGroupId aAddressGroup,
		RBuf& aText )
    {
    if( aAddressGroup != EPbk2FieldGroupIdPostalAddress &&
    		aAddressGroup != EPbk2FieldGroupIdCompanyAddress &&
    		aAddressGroup != EPbk2FieldGroupIdHomeAddress )
        	{
        	User::Leave(KErrArgument);
        	}

        const TInt KGranularity = 4;
    	CDesCArrayFlat* arrFields = new ( ELeave ) CDesCArrayFlat
                ( KGranularity );
        CleanupStack::PushL(arrFields);

        RHashMap<TInt, TPtrC> aFieldsMap;
        GetAddressFieldsLC( aContact, aAddressGroup, aFieldsMap );

        TPtrC* data = aFieldsMap.Find( EVPbkVersitSubFieldStreet );
        if( data )
        	{
        	arrFields->AppendL( *data );
        	}

        data = aFieldsMap.Find( EVPbkVersitSubFieldLocality );
    	if( data )
    		{
    		arrFields->AppendL( *data );
    		}

        HBufC* addressPreview = NULL;

        if( arrFields->Count() != 2 )
        	{
        	addressPreview = StringLoader::LoadLC( R_QTN_PHOB_POPUP_INCOMPLETE_ADDRESS );
         	}
        else
        	{
    		addressPreview = StringLoader::LoadLC( R_QTN_PHOB_COMMLAUNCHER_ONELINEPREVIEW, *arrFields );
        	}

        CleanupStack::Pop( addressPreview );
        CleanupStack::PopAndDestroy( &aFieldsMap );
    	CleanupStack::PopAndDestroy( arrFields );

    	aText.Assign( addressPreview );
    	CleanupClosePushL( aText );
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::GetAddressFieldsLC
// --------------------------------------------------------------------------
//
EXPORT_C void Pbk2AddressTools::GetAddressFieldsLC(
		MVPbkStoreContact& aContact,
		TPbk2FieldGroupId aAddressGroup,
		RHashMap<TInt, TPtrC>& aFieldsMap )
    {
    if( aAddressGroup != EPbk2FieldGroupIdPostalAddress &&
		aAddressGroup != EPbk2FieldGroupIdCompanyAddress &&
		aAddressGroup != EPbk2FieldGroupIdHomeAddress )
    	{
    	User::Leave(KErrArgument);
    	}

    CleanupClosePushL( aFieldsMap );

    MVPbkStoreContactFieldCollection& arrContactFields = aContact.Fields();
    TInt countAll = arrContactFields.FieldCount();
    for(TInt idx = 0; idx < countAll; idx++)
    	{
    	MVPbkStoreContactField& field = arrContactFields.FieldAt( idx );
        if ( field.FieldData().IsEmpty() )
            {
            continue;
            }
    	const MVPbkFieldType* fieldType = field.BestMatchingFieldType();
        if ( fieldType )
            {
            TArray<TVPbkFieldVersitProperty> arrProp =
                fieldType->VersitProperties();
            for (TInt idx2 = 0; idx2 < arrProp.Count(); idx2++ )
                {
                if ( arrProp[idx2].Name() != EVPbkVersitNameADR )
                    {
                    continue;
                    }

                if ( !arrProp[idx2].Parameters().Contains(
                    MapAddressToVersitParam( aAddressGroup ) )
                    && aAddressGroup != EPbk2FieldGroupIdPostalAddress )
                    {
                    continue;
                    }

                if ( (arrProp[idx2].Parameters().Contains(
                    MapAddressToVersitParam( EPbk2FieldGroupIdCompanyAddress ) )
                    || arrProp[idx2].Parameters().Contains(
                        MapAddressToVersitParam( EPbk2FieldGroupIdHomeAddress ) ))
                    && aAddressGroup == EPbk2FieldGroupIdPostalAddress )
                    {
                    continue;
                    }

                MVPbkContactFieldData& fieldData = field.FieldData();
                if ( fieldData.DataType() != EVPbkFieldStorageTypeText )
                    {
                    continue;
                    }

                MVPbkContactFieldTextData& data =
                    MVPbkContactFieldTextData::Cast( fieldData );
                if ( data.Text().Length() )
                    {
                    aFieldsMap.InsertL( arrProp[idx2].SubField(), data.Text() );
                    }
                }
            }
     	}
    }




