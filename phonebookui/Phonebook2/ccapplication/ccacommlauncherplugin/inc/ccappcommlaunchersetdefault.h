/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of comm launcher view plugin
*
*/

#ifndef CCAPPCOMMLAUNCHERSETDEFAULT_H_
#define CCAPPCOMMLAUNCHERSETDEFAULT_H_

#include <e32base.h>
#include "ccalogger.h"
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactAttributeManager.h>
#include <MVPbkContactStoreObserver.h>
#include <VPbkFieldType.hrh>
#include <MVPbkContactObserver.h>
#include <RPbk2LocalizedResourceFile.h>
#include <MVPbkBatchOperationObserver.h>

class CCCAppCmsContactFetcherWrapper;
class CPbk2ApplicationServices;

class CCCAppCommLauncherLSetDefault: 
    public CBase,
    public MVPbkSingleContactOperationObserver,
    public MVPbkSetAttributeObserver,
    public MVPbkContactStoreObserver,
    public MVPbkContactObserver
{
public:
	/**
	 * Two phased constructor.
	 */
	 static CCCAppCommLauncherLSetDefault* NewL( CCCAppCommLauncherPlugin& aPlugin );

	/**
	 * Destructor.
	 */
	 ~CCCAppCommLauncherLSetDefault();
	 
	 void ExecuteAssignDefaultL();
	 
public: // From MVPbkSingleContactOperationObserver

    void VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& aOperation,
        MVPbkStoreContact* aContact);

    void VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& aOperation,
        TInt aError);
    
private: // From MVPbkSetAttributeObserver
	
    void AttributeOperationComplete(
            MVPbkContactOperationBase& aOperation );
    void AttributeOperationFailed(
            MVPbkContactOperationBase& aOperation,
            TInt aError );
    
public: // From MVPbkContactStoreObserver

    void StoreReady(
        MVPbkContactStore& aContactStore);

    void StoreUnavailable(
        MVPbkContactStore& aContactStore,
        TInt aReason);

    void HandleStoreEventL(
        MVPbkContactStore& aContactStore,
        TVPbkContactStoreEvent aStoreEvent );
    
private: // From MVPbkContactObserver
    
	void ContactOperationCompleted(
            TContactOpResult aResult );
    
	void ContactOperationFailed(
            TContactOp aOpCode,
            TInt aErrorCode,
            TBool aErrorNotified );
	
private:

    /**
     * Private constructor
     */    
	CCCAppCommLauncherLSetDefault(CCCAppCommLauncherPlugin& aPlugin );

    /**
     * ConstructL
     */    
    void ConstructL();
    
private: // New implemetations
	
	/* Wait to finish
     * @param - none
	 * @return - none
     */
	void WaitFinish();
	
	/* Stop wait
	 * @param - none
	 * @return - none
	 */
	void StopWait();
	
	/* Lock contact before setting default
	 * @param - none
     * @return - none
	 */
	void LockContactL();
	
	/* Loop through all fields to find if have field for this default type
	 * @param:aDefaultType - default type
	 * @return - ETrue if find, EFalse if not find
	 */
	TBool FindContactFieldWithAttributeL( TVPbkDefaultType aDefaultType );
	
	/* Do set default atrribut
	 * @param: aDefaultType - default type
	 * @return: aIndex - the field index which will be set default
	 */
	void DoSetDefaultL( TVPbkDefaultType aDefaultType, TInt aIndex );
	
	/* Set next
	 * @param - none
     * @return - none
	 */
	void SetNextL();
	
	/* Check if has field for this selector
	 * @param:aContactAction - contact action
	 * @param:aIndex - the field index 
	 * @return - true for have, false for not have
	 */
	TBool IsContactActionHasField(
	        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction, TInt& aIndex);
	
	/* Map selector id to default type
	 * @param:aSelectorID - selector id
	 * @return - default type
	 */
	TVPbkDefaultType MapSelectorIdToDefaultType( 
			const VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aSelectorID );
	
	/* Get next selector id
	 * @param - none
	 * @return - next selector id
	 */
	VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector NextAttribute();
	
	/* Default setting complete
	 * @param - none
	 * @return - none
	 */
	void DefaultSettingCompleteL();
	
	/* Process finished
	 * @param:aError - error id
	 * @return - none
	 */
	void ProcessFinished( TInt aError );
	
	/* Handle error
	 * @param:aError - error id
	 * @return - none
	 */
	void HandleError( TInt aError );
	
	/* Check if the contact is phone memory contact
	 * @param:aContact - the contact
	 * @return - true for phone memory contact, otherwise return false
	 */
	TBool IsPhoneMemoryContact(
	        const MVPbkStoreContact& aContact) const;
    
private: // Member variables
	
	// Own: array for action id
	CArrayFixFlat<VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector>* iDefaultActions;
	
	// Own: Scheduler wait
	CActiveSchedulerWait* iWaitFinish;
	
	// Own: store contact
	MVPbkStoreContact* iStoreContact;
	
	// Own: retrieve operation
    MVPbkContactOperationBase* iRetrieveOperation; 
    
    // Own: links
    MVPbkContactLinkArray* iLinks;
    
    // Own: attribut operation
    MVPbkContactOperationBase* iSetAttributeOperation;
    
    // Own: contact store
    MVPbkContactStore* iContactStore;
    
    // referenct to plugin
    CCCAppCommLauncherPlugin& iPlugin;
    
    // wrapper
    CCCAppCmsContactFetcherWrapper* iCmsWrapper;
    
    // link array des
    HBufC8* iContactLinkArrayDes;
    
    // Application service
    CPbk2ApplicationServices* iAppServices;
    
    /// Own: Phonebook 2 commands dll resource file
    RPbk2LocalizedResourceFile iCommandsResourceFile;

    /// Own: Phonebook 2 UI controls dll resource file
    RPbk2LocalizedResourceFile iUiControlsResourceFile;

    /// Own: Phonebook 2 common UI dll resource file
    RPbk2LocalizedResourceFile iCommonUiResourceFile;
       
    };

#endif /* CCAPPCOMMLAUNCHERSETDEFAULT_H_ */
