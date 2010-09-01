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
* Description:  Send command's utility functions
*
*/


#ifndef FsSendCmdUtils_H
#define FsSendCmdUtils_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MVPbkBaseContact;
class MVPbkFieldType;
class MVPbkBaseContactField;
class MVPbkFieldTypeList;

// CLASS DECLARATIONS
/** 
 * Send command's utility functions
 */
class FsSendCmdUtils
	{
	public:
		/**
		 * Finds if contact has any data in its fields.
		 * @param aContact contact to be checked
		 * @return ETrue if contact is empty, otherwise EFalse.
		 * @see MVPbkContactFieldData::IsEmpty
		 */
		static TBool IsContactEmpty(const MVPbkBaseContact* aContact);
		
		/**
		 * Finds specified field from contact.
		 * @param aContact contact where to search
		 * @param aResIdForFieldType VPBK_FIELD_TYPE_MAPPING resource id
		 * @param master field type list from contact manager.
		 * @return contact field of type aResIdForFieldType, NULL if 
		 *					field does not found
		 */
		static const MVPbkBaseContactField* FindFieldL(
			const MVPbkBaseContact& aContact,
			TInt aResIdForFieldType,
			const MVPbkFieldTypeList& aMasterFieldTypeList);
			
		/**
		 * Finds specified field from contact.
		 * @param aContact contact where to search
		 * @param aFieldType a field type to find from contact
		 * @param master field type list from contact manager.
		 * @return contact field of type aFieldType, NULL if 
		 *					field does not found
		 */
		static const MVPbkBaseContactField* FindField(
			const MVPbkBaseContact& aContact,
			const MVPbkFieldType& aFieldType,
			const MVPbkFieldTypeList& aMasterFieldTypeList);
			
			
		/**
		 * Reads field type mapping from resource file
		 * @param aResId VPBK_FIELD_TYPE_MAPPING resource id
		 * @param master field type list from contact manager.
		 * @return field type
		 * @exception If field type not found leaves with KErrNotFound
		 */
		static const MVPbkFieldType& ReadFieldTypeFromResL(
			TInt aResId,
			const MVPbkFieldTypeList& aMasterFieldTypeList);
		
		/**
		 * Returns the contact field's field type.
		 * @param field from where to find field type
		 * @param master field type list from contact manager.
		 * @return field's field type
		 * @see MVPbkBaseContactField::MatchFieldType
		 */
		static const MVPbkFieldType* FieldType(
			const MVPbkBaseContactField& aField,
			const MVPbkFieldTypeList& aMasterFieldTypeList);
			
		/**
		 * Checks if aField's field type and aFieldType are matching
		 * @param field from where to find field type
		 * @param aFieldType a field type to match with contact's field type
		 * @param master field type list from contact manager.
		 * @return field's field type
		 * @see MVPbkBaseContactField::MatchFieldType
		 */
		static TBool IsFieldMatching(
			const MVPbkBaseContactField& aField,
			const MVPbkFieldType& aFieldType,
			const MVPbkFieldTypeList& aMasterFieldTypeList);		
	};
	
#endif // FsSendCmdUtils_H

// End of file
