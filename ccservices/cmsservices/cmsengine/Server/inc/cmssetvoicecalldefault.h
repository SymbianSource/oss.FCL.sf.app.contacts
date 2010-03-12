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
* Description:  Implementation of auto setting default for voice call
*
*/

#ifndef CMSSETVOICECALLDEFAULT_H_
#define CMSSETVOICECALLDEFAULT_H_

#include <e32base.h>
#include <mvpbksinglecontactoperationobserver.h>
#include <mvpbkcontactattributemanager.h>
#include <mvpbkcontactstoreobserver.h>
#include <vpbkfieldtype.hrh>
#include <mvpbkcontactobserver.h>
#include <mvpbkbatchoperationobserver.h>

class CmsSetVoiceCallDefault: 
    public CBase,
    public MVPbkSetAttributeObserver,
    public MVPbkContactObserver
{
public:
	
	enum TCmsVoiceCallPriority
	    {
	    ECCACommLauncherCallMobile = 0,                
	    ECCACommLauncherCallMobileHome,
	    ECCACommLauncherCallMobileWork,
	    ECCACommLauncherCallTelephone,
	    ECCACommLauncherCallTelephoneWork,
	    ECCACommLauncherCallTelephoneHome,
	    ECCACommLauncherCallCarPhone,
	    ECCACommLauncherCallAssistant,
	    ECCACommLauncherCallLast   
	    };
	
	/**
	 * Two phased constructor.
	 */
	 static CmsSetVoiceCallDefault* NewL();

	/**
	 * Destructor.
	 */
	 ~CmsSetVoiceCallDefault();
	 
    
private: // From MVPbkSetAttributeObserver
	
    void AttributeOperationComplete(
            MVPbkContactOperationBase& aOperation );
    void AttributeOperationFailed(
            MVPbkContactOperationBase& aOperation,
            TInt aError );
    
public:
	
	void SetVoiceCallDefaultL( MVPbkStoreContact* aContact, CVPbkContactManager* aContactManager );
    
private: // From MVPbkContactObserver
    
	void ContactOperationCompleted(
            TContactOpResult aResult );
    
	void ContactOperationFailed(
            TContactOp aOpCode,
            TInt aErrorCode,
            TBool aErrorNotified );
	
private:

    /**
     * ConstructL
     */    
    void ConstructL();
    
    CmsSetVoiceCallDefault();
    
private: // New implemetations
	
	/* Lock contact before setting default
	 * @param - none
     * @return - none
	 */
	void LockContactL();
	
	
	/* Do set default atrribut
	 * @param: aDefaultType - default type
	 * @return: aIndex - the field index which will be set default attribute
	 */
	void DoSetDefaultL( TVPbkDefaultType aDefaultType, TInt aIndex );
	
	
	/* 
	 * Check if default setting is needed
	 * if the contact has no call field 
	 * or call default has been set, then the setting doesn't need.
	 * @return: True for setting needed, otherwize return false.
	 */
	TBool IsSettingDefaultNeededL();
	
	/* Get call field resource id
	 * @param: aPriority - setting priority
	 * @return: field resource id
	 */
	TInt GetCallFieldResourceId( TInt aPriority );

	/* 
	 * Get field index from field resource id
	 * @param: aResourceId - field resource id
	 * @param: aIndex - the field index 
	 * @return: Ture for find the specified field, otherwize return false.
	 */
    TBool FieldIndex( TInt aResourceId, TInt& aIndex );
	
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
	 * @param: aContact - the contact to be check
	 * @return: true for phone memory contact, otherwise return false
	 */
	TBool IsPhoneMemoryContact(
	        const MVPbkStoreContact& aContact) const;
	
	/* 
	 * To set voice call default
	 */
	void ToSetVoiceCallDefaultL();
    
private: // Member variables
	
	// Not own: store contact
	MVPbkStoreContact* iStoreContact;
	
    // Not own: contact manager
    CVPbkContactManager*       iContactManager;
    
    // Own: attribut operation
    MVPbkContactOperationBase* iSetAttributeOperation;
    
    // Own: filed index array
    RArray<TInt>  iFieldIndexArray;
         
    };

#endif /* CMSSETVOICECALLDEFAULT_H_ */
