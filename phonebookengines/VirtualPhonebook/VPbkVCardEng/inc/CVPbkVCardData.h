/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Virtual Phonebook vCard data class.
*
*/


#ifndef CVPBKVCARDDATA_H
#define CVPBKVCARDDATA_H

// INCLUDES
#include <e32base.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class CVPbkVCardFieldTypeProperty;
class CVPbkVCardIdNameMapping;
class MVPbkFieldTypeList;
class CParserVCard;
class CVPbkContactCopyPolicyManager;
class MVPbkContactCopyPolicy;
class CVPbkContactManager;
class TVPbkContactStoreUriPtr;
class MVPbkContactAttributeManager;
class MVPbkContactStore;
class CVPbkContactDuplicatePolicy;
class MVPbkContactDuplicatePolicy;
namespace VPbkEngUtils
    {
    class CTextStore;
    }

/**
 * Virtual Phonebook vCard data class.
 */
NONSHARABLE_CLASS(CVPbkVCardData) : public CBase
    {
    public: // Construction and destruction
        static CVPbkVCardData* NewL(CVPbkContactManager& aContactManager);
        ~CVPbkVCardData();

    public: // Interface
        const MVPbkFieldTypeList& SupportedFieldTypes() const;
        TArray<CVPbkVCardFieldTypeProperty*> Properties() const;
        TArray<CVPbkVCardIdNameMapping*> VNameIdNameMappings() const;
        TArray<CVPbkVCardIdNameMapping*> ParamIdNameMappings() const;
        MVPbkContactAttributeManager& AttributeManagerL() const;
        /**
         * Returns a reference to policy or NULL if no policy exists
         */
        MVPbkContactDuplicatePolicy* DuplicatePolicy();
        
        /**
        * @exception KErrNotSupported if there is no policy for the store
        */
        MVPbkContactCopyPolicy& CopyPolicyL(
            const TVPbkContactStoreUriPtr& aUri) const;
        
        /**
        * Returns the default contact database store. The store
        * is not necessary open so any operation that needes the store
        * to be open must not be done.
        *
        * @return the default contact database.
        * @exception KErrNotFound if the store is not found ->
        *            store plugin is missing or loading it failed
        */
        MVPbkContactStore& GetCntModelStoreL();
        
         /**
        * A getter method that returns a pointer to the array
        * of masked fields used during a beamed export.
        *
        * @returns a pointer to the array.
        */
        const CDesC8ArrayFlat* GetMaskedFields();
        
        /**
        * A getter method that returns ContactManager
		*
        * @returns ContactManager
        */
        CVPbkContactManager& GetContactManager();
        
    private: // Implementation
        CVPbkVCardData(CVPbkContactManager& aContactManager);
        void ConstructL();
        
    private: // Data
        /// Ref: Contact Manager
        CVPbkContactManager&                        iContactManager;
        /// Own: Array of CVPbkVCardFieldTypeProperty instances for exporting contact
        RPointerArray<CVPbkVCardFieldTypeProperty>  iProperties;
        /// Own: Array of CVPbkVCardIdNameMapping instances for mapping versit 
        ///      parameter name with VPbk id
        RPointerArray<CVPbkVCardIdNameMapping>      iParamIdNameMappings;  
        /// Own: Array of CVPbkVCardIdNameMapping instances for mapping versit 
        ///      name with VPbk id
        RPointerArray<CVPbkVCardIdNameMapping>      iVNameIdNameMappings;
        /// Own: Array of supported field types
        MVPbkFieldTypeList*                         iSupportedFieldTypes;
        /// Own: Copy policy manager for getting copy policies
        CVPbkContactCopyPolicyManager*              iCopyPolicyManager;
        /// Own: A duplicate policy
        CVPbkContactDuplicatePolicy*                iDuplicatePolicy;
        ///Own: X- versit type storage
        VPbkEngUtils::CTextStore* iTextStore;        
        
        //  Own: Array to hold those fields masked during a beamed export
         CDesC8ArrayFlat* iMaskedFieldsArray;
    };

#endif // CVPBKVCARDDATA_H
//End of file

