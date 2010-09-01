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
* Description: 
*       Phonebook command factory.
*
*/


#ifndef __MPbkCommandFactory_H__
#define __MPbkCommandFactory_H__

//  INCLUDES
#include <cntdef.h>	// for TContactItemId
#include <CPbkAddressSelect.h>
#include <TPbkSendingParams.h>

//  FORWARD DECLARATIONS
class MPbkCommand;
class CPbkContactItem;
class CContactIdArray;
class CPbkContactEngine;
class CBCardEngine;
class TPbkContactItemField;
class CApaDocument;
class MObjectProvider;
class CAiwServiceHandler;
class CAiwGenericParamList;
class CPbkCallTypeSelector;

//  CLASS DECLARATION 

/**
 * Phonebook command factory.
 */
class MPbkCommandFactory
    {
    public:  // Interface
        /**
         * Creates send contact command object.
         * @param aParams, sending params
		 * @param aEngine reference to phonebook engine
		 * @param aBCardEng reference to business card engine         
         * @param aContactId the contact id to send
         * @param aField currently focused field if any
         * @return a new send contact command object.
         */
        virtual MPbkCommand* CreateSendContactCmdL(
            TPbkSendingParams aParams,
			CPbkContactEngine& aEngine,
            CBCardEngine& aBCardEng,			
            TContactItemId aContactId,
            TPbkContactItemField* aField = NULL
            ) = 0;

		/**
         * Creates send contact command object for multiple contacts.
         * @param aParams, sending params
		 * @param aEngine reference to phonebook engine
		 * @param aBCardEng reference to business card engine         
         * @param aContacts contacts to send
         * @return a new send contact command object.
         */
		virtual MPbkCommand* CreateSendContactsCmdL(
		    TPbkSendingParams aParams,
			CPbkContactEngine& aEngine,
            CBCardEngine& aBCardEng,			
            const CContactIdArray& aContacts
            ) = 0;

		/**
         * Creates call command object.
         * @param aContact the contact to call
         * @param aSelectedField the phonenumber field of aContactItem to dial
         * @param aFocusedField currently focused field. May be NULL if caller
         *                      doesn't have field-level focus. Used to deduct
         *                      should a video call launched (the mere selected
         *                      field information is not enough)
         * @param aCommandId command id
         * @param aServiceHandler aiw service handler
         * @param aCallTypeSelector call type selector
         * @return a new call command object.
         */
        virtual MPbkCommand* CreateCallCmdL(
		    const CPbkContactItem& aContactItem,
    		const TPbkContactItemField* aSelectedField,
    		const TPbkContactItemField* aFocusedField,
            const TInt aCommandId,
            CAiwServiceHandler& aServiceHandler,
            const CPbkCallTypeSelector& aCallTypeSelector) =0;

		/**
         * Creates send message command object.
         * @param aEngine,      Reference to pbk engine, used to retrieve contact
         *                      info
         * @param aParams, sending params
         * @param aContacts     Array of contact ID's where message should be send
         * @param aFocusedField currently focused field. May be NULL if caller
         *                      doesn't have field-level focus. If field is 
         *                      applicable address field it is used directly
         *                      as the recipient address.
         * @param aUseDefaultDirectly   if ETrue and a default address is found
         *                              from the contact the default address is 
         *                              used without presenting address 
         *                              selection list.
         * @return a new send message command object.
         */
		virtual MPbkCommand* CreateSendMultipleMessageCmdL(
		    CPbkContactEngine& aEngine,
            TPbkSendingParams aParams,
			const CContactIdArray& aContacts,
			const TPbkContactItemField* aFocusedField,
			TBool aUseDefaultDirectly) =0;
			
		/**
         * Creates go to URL command object.
         * @param aContact      contact to operate with.
         * @param aFocusedField currently focused field
         */
		virtual MPbkCommand* CreateGoToURLCmdL(
            const CPbkContactItem& aContact,
			const TPbkContactItemField* aFocusedField) = 0;

		/**
         * Creates settings command object.
         * @param aEngine   phonebook contact engine
         * @param aParent   object provider parent   
         */
		virtual MPbkCommand* CreateSettingsCmdL(
            CPbkContactEngine& aEngine,
            MObjectProvider& aParent) = 0;

		/**
		 * Returns true if email sending is enabled in SendAppUi.
		 * @return ETrue if email is enabled, EFalse otherwise
		 */
		virtual TBool IsEmailEnabledL() = 0;

        /**
         * Creates poc command object.
         * @param aCommandId command id
         * @param aContacts the contacts to operate with
         * @param aControlFlags control's menu filtering flags
         * @param aServiceHandler aiw service handler
         * @param aCallTypeSelector call type selector
         * @return a new poc call command object.
         */
        virtual MPbkCommand* CreatePocCmdL(
            TInt aCommandId,
            const CContactIdArray& aContacts,
            TUint aControlFlags,
            CAiwServiceHandler& aServiceHandler,
            const CPbkCallTypeSelector& aCallTypeSelector) =0;
            
        /**
         * Creates SyncMl command object.         
         * @param aCommandId command id
         * @param aServiceHandler aiw service handler
         * @return a new call command object.
         */
        virtual MPbkCommand* CreateSyncMlCmdL(            
            TInt aCommandId,
            CAiwServiceHandler& aServiceHandler) =0;

	protected:
	    /**
		 * Protected virtual destructor.
		 */
		virtual ~MPbkCommandFactory() { }
    };

#endif // __MPbkCommandFactory_H__

// End of File
