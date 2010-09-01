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
* Description:  Extension for Phonebook engine, handles all sind related 
*                activities
*
*/


#ifndef __CPBKSINDHANDLER_H__
#define __CPBKSINDHANDLER_H__


// INCLUDES
#include <f32file.h>
#include <CPbkSINDHandlerInterface.h>
#include <NssVasApi.h>

// FORWARD DECLARATIONS
class CPbkConstants;
class CPbkFieldsInfo;
class TPbkContactItemField;

// CLASS DECLARATION

/**
 * Extension for Phonebook engine.
 */
class CPbkSINDHandler : public CPbkSINDHandlerInterface,
                        public MNssGetContextClient,
                        public MNssGetTagClient
    {
    public: // Constructor and destructor
        /**
         * Creates a new instance of CPbkSINDHandler.         
         */
        static CPbkSINDHandler* NewL();

        /**
         * Destructor.
         */
        ~CPbkSINDHandler();
        
    public: // From CPbkSINDHandlerInterface
    
        TInt VoiceTagField(TContactItemId aContactId);
        
    public: // From MNssGetContextClient
    
        void GetContextCompleted(MNssContext* aContext);
        void GetContextListCompleted(MNssContextListArray *aContextList);
        void GetContextFailed(TNssGetContextClientFailCode aFailCode);
    
    public: // From MNssGetTagClient
    
        void GetTagListCompleted(MNssTagListArray* aTagList);
        void GetTagFailed(TNssGetTagClientFailCode aFailCode);
        
    private: // own methods:
        /**
        * Initializes all SIND instances.
        * @return TInt KErrNone on success
        */
        TInt InitializeL();

    private: // Implementation
        CPbkSINDHandler();
        void ConstructL();
        
    private: // Data
        
        /// Own: Vas databse manager
        CNssVASDBMgr* iVasDbManager;        
        /// Own: Nss context
        MNssContext* iContext;
        /// Own: scheduler wait object
        CActiveSchedulerWait* iWait;
        /// Ref: Context manager
        MNssContextMgr* iContextManager;        
        /// Ref: Tag manager
        MNssTagMgr* iTagManager;
        /// Error code received from observer call-back methods.
        TInt iErr;
        /// Id of the voice tagged field id
        TInt iVoiceTaggedFieldId;
        /// Has the SindHandler been initialized
        TBool iInitialized;
    };

#endif  // __CPBKSINDHANDLER_H__

// End of file
