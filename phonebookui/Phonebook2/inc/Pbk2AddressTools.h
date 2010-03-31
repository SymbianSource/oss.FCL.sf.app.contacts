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

#ifndef PBK2ADDRESSTOOLS_H
#define PBK2ADDRESSTOOLS_H

#include "TPbk2ContactEditorParams.h"
#include <Pbk2FieldProperty.hrh>
#include <VPbkFieldType.hrh>
#include <MVPbkFieldType.h>
#include <e32hashtab.h>

class MVPbkStoreContact;

class Pbk2AddressTools
	{
public:
	 /**
	 * Maps UI contol type to feld group
	 *
	 * @param aCtrlType  Type of control ( TPbk2FieldCtrlTypeExt )
	 * @return Suitable gruup id ( TPbk2FieldGroupId )
	 */
	IMPORT_C static TPbk2FieldGroupId MapCtrlTypeToAddress( 
			TInt aCtrlType );
	
	 /**
	 * Maps UI view type  to feld group
	 *
	 * @param aViewType  Type of view
	 * @return Suitable gruup id
	 */
	IMPORT_C static TPbk2FieldGroupId MapViewTypeToAddress( 
			TPbk2ContactEditorParams::TActiveView aViewType );
	
	/**
	 * Maps feld group to field ordering
	 *
	 * @param aGroupAddress  field group
	 * @return Suitable field ordering
	 */
	IMPORT_C static TInt MapAddressToOrdering( 
			TPbk2FieldGroupId aGroupAddress );
	
	/**
	 * Maps feld group to field ordering
	 *
	 * @param aGroupAddress  field group
	 * @return Suitable field ordering
	 */
	IMPORT_C static TPbk2ContactEditorParams::TActiveView MapCtrlTypeToViewType( 
			TInt aCtrlType );
	
	/**
	 * Maps feld group to field type parameter
	 *
	 * @param aGroupAddress  field group
	 * @return Suitable field type parameter
	 */
	IMPORT_C static TVPbkFieldTypeParameter MapAddressToVersitParam( 
			TPbk2FieldGroupId aGroupId );
	
	/**
	 * Creates map field type parameters to text data from these fields
	 *
	 * @param aContact  contact
	 * @param aAddressGroup  field group
	 * @param aFieldsMap  return parameter, allocated and put on CleanapStack
	 * 					  	
	 */
	IMPORT_C static void GetAddressFieldsLC(
				MVPbkStoreContact& aContact, 
				TPbk2FieldGroupId aAddressGroup, 
				RHashMap<TInt, TPtrC>& aFieldsMap );
	
	/**
	* Creates address preview text from QTN_PHOB_ADDRESS_PREVIEW
	* or QTN_PHOB_CONTACT_EDITOR_DEFINE if suitable fields are empty
	*
	* @param aContact  contact
	* @param aAddressGroup  field group
	* @param aFieldsMap  return parameter, allocated and put on CleanapStack
	* 					  	
	*/
	IMPORT_C static void GetAddressPreviewLC(
			MVPbkStoreContact& aContact, 
			TPbk2FieldGroupId aAddressGroup, 
			RBuf& aText );
	
	/**
	* Creates address preview text from qtn_phob_commlauncher_onelinepreview  
	* or QTN_PHOB_POPUP_INCOMPLETE_ADDRESS if suitable fields are empty
	*
	* @param aContact  contact
	* @param aAddressGroup  field group
	* @param aFieldsMap  return parameter, allocated and put on CleanapStack
	* 					  	
	*/
	IMPORT_C static void GetAddressShortPreviewLC(
				MVPbkStoreContact& aContact, 
				TPbk2FieldGroupId aAddressGroup, 
				RBuf& aText );
	
	 /**
	 * Maps VPbkFieldType to field group
	 *
	 * @param aVPbkFieldType  Type of virtual phonebook field type
	 * @return Suitable group id ( TPbk2FieldGroupId )
	 */
	IMPORT_C static TPbk2FieldGroupId MapVPbkFieldTypeToAddressGroupId( 
			const MVPbkFieldType* aVPbkFieldType );
	
	 /**
	 * Judge whether the address preview of one contact is empty
	 * Address preview include the following four fields: 
	 * EVPbkVersitSubFieldStreet, EVPbkVersitSubFieldLocality, 
	 * EVPbkVersitSubFieldRegion and EVPbkVersitSubFieldCountry.
	 * If the four fields are all empty, return ETrue; Otherwise, return EFalse.
	 *
	* @param aContact  contact
	* @param aAddressGroup  field group
	* @return If address preview is empty, return ETrue; Otherwiase, EFalse
	 */
	IMPORT_C static TBool IsAddressPreviewEmptyL( 
			MVPbkStoreContact& aContact,
			TPbk2FieldGroupId aAddressGroup );
	};

#endif // PBK2ADDRESSTOOLS_H
            
// End of File
