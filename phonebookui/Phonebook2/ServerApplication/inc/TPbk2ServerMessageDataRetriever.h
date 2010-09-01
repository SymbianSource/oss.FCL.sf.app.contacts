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
* Description:  Phonebook 2 server application server message
*              : data retriever.
*
*/


#ifndef TPBK2SERVERMESSAGEDATARETRIEVER_H
#define TPBK2SERVERMESSAGEDATARETRIEVER_H

// INCLUDES
#include <e32base.h>
#include <coehelp.h>
#include <Pbk2ServerAppIPC.h>
#include <RVPbkContactFieldDefaultPriorities.h>
#include <AiwContactSelectionDataTypes.h>

// FORWARD DECLARATIONS
class RMessage2;
class CVPbkContactStoreUriArray;
class CVPbkContactLinkArray;
class CPbk2StoreConfiguration;
class CVPbkContactManager;
class MVPbkFieldTypeList;
class CVPbkFieldTypeSelector;
class MVPbkContactLinkArray;


// CLASS DECLARATION

/**
 * Phonebook 2 server application server message data retriever.
 * Responsible for:
 * - reading client-server messages
 * - constructing externalized objects from the message
 */
class TPbk2ServerMessageDataRetriever
    {
    public: // Construction

        /**
         * Constructor.
         */
        TPbk2ServerMessageDataRetriever();

    public: // Interface

        /**
         * Gets URI array of the contact stores to from the given message.
         * If the message does not contain it, the current configuration
         * of Phonebook is used.
         *
         * @param aMessage              The message to retrieve data from.
         * @param aStoreConfiguration   Phonebook store configuration.
         * @param aPreselectedLinks     Preselected contact links, if any.
         * @param aCurrentConfiguration On return contains information if
         *                              returned array is equal to current
         *                              configuration.
         * @return  Contact store URI array.
         */
        CVPbkContactStoreUriArray* GetContactStoreUriArrayL(
                const RMessage2& aMessage,
                const CPbk2StoreConfiguration& aStoreConfiguration,
                MVPbkContactLinkArray* aPreselectedLinks,
                TBool& aCurrentConfiguration ) const;

        /**
         * Gets preselected contact links.
         *
         * @param aMessage              The message to retrieve data from.
         * @param aContactStoreList     Contact store list.
         * @return  Preselected contact links.
         */
        MVPbkContactLinkArray* GetPreselectedContactLinksL(
                const RMessage2& aMessage,
                const CVPbkContactManager& aContactManager ) const;

        /**
         * Gets address select filter.
         *
         * @param aMessage              The message to retrieve data from.
         * @param aFieldTypeList        Virtual Phonebook field type list.
         * @return  Address select filter or NULL if it was not sent.
         */
        CVPbkFieldTypeSelector* GetAddressSelectFilterL(
                const RMessage2& aMessage,
                const MVPbkFieldTypeList& aFieldTypeList ) const;

        /**
         * Gets address select filter buffer.
         *
         * @param aMessage              The message to retrieve data from.
         * @return  Address select filter buffer or NULL if it was not sent.
         */
        HBufC8* GetAddressSelectFilterBufferL(
                const RMessage2& aMessage ) const;

        /**
         * Gets address select type.
         *
         * @param aMessage              The message to retrieve data from.
         * @return  Address select type.
         */
        TAiwAddressSelectType GetAddressSelectTypeL(
                const RMessage2& aMessage ) const;
                
        /**
         * Gets communication address select type.
         *
         * @param aMessage              The message to retrieve data from.
         * @return  Communication address select type.
         */
        TAiwCommAddressSelectType GetCommAddressSelectTypeL(
                const RMessage2& aMessage ) const;

        /**
         * Gets contact view filter.
         *
         * @param aMessage              The message to retrieve data from.
         * @param aFieldTypeList        Virtual Phonebook field type list.
         * @return  Contact view filter or NULL if it was not sent.
         */
        CVPbkFieldTypeSelector* GetContactViewFilterL(
                const RMessage2& aMessage,
                const MVPbkFieldTypeList& aFieldTypeList ) const;

        /**
         * Gets contact view filter for attribute assign.
         *
         * @param aMessage              The message to retrieve data from.
         * @param aFieldTypeList        Virtual Phonebook field type list.
         * @return  Contact view filter or NULL if it was not sent.
         */
        CVPbkFieldTypeSelector* GetContactViewFilterForAttributeAssignL(
                const RMessage2& aMessage,
                const MVPbkFieldTypeList& aFieldTypeList ) const;

        /**
         * Gets title pane text.
         *
         * @param aMessage              The message to retrieve data from.
         * @return  Title pane text or NULL if it was not sent.
         */
        HBufC* GetTitlePaneTextL(
                const RMessage2& aMessage ) const;

        /**
         * Gets contact attribute data from the given message.
         *
         * @param aMessage  The message to retrieve data from.
         * @return  Contact attribute data.
         */
        TPbk2AttributeAssignData GetAttributeDataL(
                const RMessage2& aMessage ) const;

        /**
         * Gets the data buffer to assign to a contact.
         *
         * @param aMessage  The message to retrieve data from.
         * @return  Data buffer.
         */
        HBufC* GetDataBufferL(
                const RMessage2& aMessage ) const;

        /**
         * Gets the focus index for assigning a contact.
         *
         * @param aMessage  The message to retrieve data from.
         * @return  Focus index.
         */
        TInt GetFocusIndexL(
                const RMessage2& aMessage ) const;

        /**
         * Gets the MIME type of the data to assign.
         *
         * @param aMessage  The message to retrieve data from.
         * @return  MIME type.
         */
        TInt GetMimeTypeL(
                const RMessage2& aMessage ) const;

        /**
         * Gets single contact assign flags.
         *
         * @param aMessage  The message to retrieve data from.
         * @return  Flags.
         */
        TUint SingleContactAssignFlagsL(
                const RMessage2& aMessage ) const;

        /**
         * Gets multiple contact assign flags.
         *
         * @param aMessage  The message to retrieve data from.
         * @return  Flags.
         */
        TUint MultipleContactAssignFlagsL(
                const RMessage2& aMessage ) const;

        /**
         * Gets fetch flags.
         *
         * @param aMessage  The message to retrieve data from.
         * @return  Flags.
         */
        TUint FetchFlagsL(
                const RMessage2& aMessage ) const;

        /**
         * Gets attribute assign remove information.
         *
         * @param aMessage  The message to retrieve data from.
         * @return  ETrue if the attribute is to be removed,
         *          EFalse if the attribute is to be added.
         */
        TBool GetAttributeRemovalIndicatorValueL(
                const RMessage2& aMessage ) const;

        /**
         * Gets editor help context.
         *
         * @param aMessage  The message to retrieve data from.
         * @return  Editor help context.
         */
        TCoeHelpContext GetEditorHelpContextL(
                const RMessage2& aMessage ) const;

        /**
         * Gets the fetch dialog resource id.
         *
         * @param aMessage  The message to retrieve data from.
         * @return  Dialog resource id.
         */
        TInt GetFetchDialogResourceL(
                const RMessage2& aMessage ) const;

        /**
         * Gets the default priorities.
         *
         * @param aMessage  The message to retrieve data from.
         * @return  Default priorities or NULL if not set.
         */
        RVPbkContactFieldDefaultPriorities FetchDefaultPrioritiesL(
                const RMessage2& aMessage ) const;
        
        /**
         * Gets the client orientation settings.
         *
         * @param aMessage  The message to retrieve data from.
         * @return  The client orientation settings.
         */
        TInt GetOrietationTypeL(const RMessage2& aMessage ) const;
        
        
        /**
         * Gets StatusPane ID that should be used by the ServerApplication
         *
         * @param aMessage              The message to retrieve data from.
         * @return  valid StatusPane ID or 0 if it was not sent.
         */
        TInt TPbk2ServerMessageDataRetriever::GetStatusPaneIdL
                ( const RMessage2& aMessage ) const;
    };

#endif // TPBK2SERVERMESSAGEDATARETRIEVER_H

// End of File
