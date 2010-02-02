/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handler to manage groupcard property during import and export
 *                
 *
*/


#ifndef CVPBKGROUPCARDHANDLER_H
#define CVPBKGROUPCARDHANDLER_H

// INCLUDES
#include <e32base.h>
#include <s32mem.h>
#include <e32hashtab.h>
#include <CVPbkContactIdConverter.h>
#include <MVPbkContactLink.h>
#include <MVPbkStoreContact.h>
#include <MVPbkSingleContactLinkOperationObserver.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactObserver.h>
#include <MVPbkContactGroup.h>
#include <vprop.h>
#include <vcard.h>
#include "CVPbkVCardParserParamArray.h"
#include "CVPbkVCardData.h"


//Group Card Example

/* The Group Card is Identified by field Name X-CATEGORIES and Values are seperated by ";".

Single Group Card Vcard
BEGIN:VCARD
VERSION:2.1
N:FirstName
X-CLASS:private
TEL;CELL:1
X-CATEGORIES:Group1
END:VCARD

Two Group Card Vcard

BEGIN:VCARD
VERSION:2.1
N:FirstName
X-CLASS:private
TEL;CELL:1
X-CATEGORIES:Group1;Group2
END:VCARD
*/

// CLASS DECLARATIONS
/**
 * Handler to manage groupcard property during import and export
 */

class CVPbkGroupCardHandler : 
         public CBase,
         public MVPbkSingleContactOperationObserver,
         public MVPbkContactObserver

    {
public:

    // Creates a new instance of this class. Two-Phase Construction
    static CVPbkGroupCardHandler* NewL(CVPbkVCardData& aData);

    //destruction
    ~CVPbkGroupCardHandler();

    /**
     * Creates X-CATEGORIES property to be added to the Vcard
     * Client takes Ownership in deleting returned property.
     * @param aStore VPBK Contact Store Link
     */
    CParserProperty*  CreateXGroupPropertyL(const MVPbkStoreContact* aStore);

    /**
     * Set the contactLink as Group Store Contact
     * @param  aContactLink  VPBK contact Link
     *								
     */
    void GetContactGroupStoreL(const MVPbkContactLink& aContactLink);

    /**
     * Decodes the Current Group Card Field to add the contact to exisiting group Or New Group.
     * @param  aValue  Value parm of the Group Card Property
     *								
     */
    void DecodeContactGroupInVCardL(TPtr16 aValue);

    /**
     * Builds the Hash Table with Key as "Group Name" and "VBPK Store Pointer" as Value.
     * This table is used to Find whether a Group is existing or not in current VPBK.
     * @param  aTargetContactStore  VPBK ContactStore.
     *								
     */

    void BuildContactGroupsHashMapL(MVPbkContactStore& aTargetContactStore);

    /**
     * Remvoes the Contact from all the Groups it belongs. This Api is called as part of DecodeContactGroupInVcardL() before 
     * adding/Creating a contact to groups.
     */

    void DeleteContactFromGroupsL();

    /**
     * Creates the New group in current VPBK and adds the Contact to it.This Api is called as part of DecodeContactGroupInVcardL() 
     * before Creating a contact to groups.
     * @param  aGroupLabel  Name of the Group to create new group.
     *								
     */

    void CreateNewGroupFromContactL(TPtr16&  aGroupLabel);

private:
    //Constructor
    CVPbkGroupCardHandler(CVPbkVCardData& aData);

    //Second Phase Construction
    void ConstructL();    

public:  

    //From MVPbkSingleContactOperationObserver         
    /** 
     * Called when the contact operation is complete
     * Client takes Ownership in deleting returned StoreContact.
     */
    void VPbkSingleContactOperationComplete(
            MVPbkContactOperationBase& aOperation,
            MVPbkStoreContact* aContact);

    /**
     * From MVPbkSingleContactOperationObserver  
     * Called when the contact operation fails
     */
    void VPbkSingleContactOperationFailed
    (MVPbkContactOperationBase& aOperation, TInt aError);

public:

    //From MVPbkContactObserver  
    /**
     * Called when a contact operation has succesfully completed.
     * Client takes Ownership in deleting returned aResult.
     */    
    void ContactOperationCompleted(TContactOpResult aResult);

    /**
     * From MVPbkContactObserver  
     * Called when a contact operation has failed.
     */
    void ContactOperationFailed
    (TContactOp aOpCode, TInt aErrorCode, TBool aErrorNotified);

private:
    //Own: Scheduler for wait.
    CActiveSchedulerWait *iWait;
    //Ref: Vcard Data
    CVPbkVCardData& iData;
    //Own: Current contact VPBK Store
    MVPbkStoreContact * iStore;
    //Own: for temp VPBK Store Contact
    MVPbkStoreContact * iTargetStore;
    //Own: Hash Table to store Group Names.
    RPtrHashMap<TDesC16, MVPbkStoreContact> *iContactsGroupsMap;
    };

#endif // CVPbkGroupCardHandler_H
// End of file
