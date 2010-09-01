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
*       Phonebook application UI base class. 
*
*/


#ifndef __CPbkAppUiBase_H__
#define __CPbkAppUiBase_H__


//  INCLUDES
#include <aknviewappui.h>  // CAknAppUi
#include <Phonebook.hrh>    // TPbkViewTransitionEvent
#include <MPbkKeyEventHandler.h>
#include <MPbkCommandList.h>


//  FORWARD DECLARATIONS
class CPbkViewState;
class CPbkDocumentBase;
class CPbkAppViewBase;
class CDigViewGraph;
class CEikImage;
class CPbkAppGlobalsBase;
class MPbkCommandHandler;


//  CLASS DECLARATION 

/**
 * Phonebook application UI base class. 
 */
class CPbkAppUiBase : public CAknViewAppUi, 
                      public MPbkKeyEventHandler,
                      public MPbkCommandList
    {
    public:  // Constants and types
        /**
         * View activation transaction base class. 
         * @see CPbkAppUiBase::HandleActivationL
         * @see CPbkAppUiBase::HandleViewActivationLC
         */
        class CViewActivationTransaction : public CBase
            {
            public:  // Interface
                /**
                 * Commits this transaction -- tells destructor to keep changes.
                 */
                virtual void Commit() =0;

                /**
                 * Virtual destructor. Override to roll back any changes made
                 * at construction if Commit() is not called.
                 */
                virtual ~CViewActivationTransaction() { }
            };

    public: // from MPbkCommandList
        IMPORT_C virtual void AddAndExecuteCommandL(MPbkCommand* aCommand);

    protected:  // Constructors and destructor
        /**
         * Constructor.
         */
        IMPORT_C CPbkAppUiBase();

        /**
         * Destructor.
         */
        IMPORT_C ~CPbkAppUiBase();


    public:  // New functions
        /**
         * Returns the Phonebook document object.
         */
        IMPORT_C CPbkDocumentBase* PbkDocument() const;

        /**
         * Returns the currently active view.
         */
        IMPORT_C CPbkAppViewBase* ActiveView() const;

        /**
         * Returns true if aViewId is a phonebook view.
         */
        virtual TBool IsPhonebookView(const TVwsViewId& aViewId) const =0;

        /**
         * Activates a Phonebook view.
         *
         * @param aViewId       id of the view to activate.
         * @param aViewState    the state to pass to the activated view.
         * @see ActivateLocalViewL(TUid)
         */
        virtual void ActivatePhonebookViewL
            (TUid aViewId, const CPbkViewState* aViewState=NULL) =0;

        /**
         * Activate the previous view.
		 * @param aViewState desired view state
         */
        virtual void ActivatePreviousViewL
			(const CPbkViewState* aViewState = NULL) =0;

        /**
         * Flags for HandleViewActivationL.
         * @see HandleViewActivationL
         */
        enum TViewActivationFlags
            {
            EUpdateTitlePane = 0x0001,
            EUpdateContextPane = 0x0002,
            EUpdateNaviPane = 0x0004,
            EUpdateAll = 0xffff
            };

        /**
         * Updates application-level objects (view navigation state and 
         * status pane) leave-safely at view activation. Called from each 
         * application view's DoActivateL().
         *
         * @param aViewId the activated view's id.
         * @param aPrevViewId previously active view's id.
         * @param aTitlePaneText text to set to title pane. If NULL default
         *        text is used.
         * @param aContextPanePicture picture to set into context pane. 
         *        If NULL default picture is used.
         * @param aFlags combination of TViewActivationFlags telling which
         *        parts of the status pane are updated.
         * @return a transaction object. Commit the object before destroying 
         *         it to keep the changes to application-level objects.
         * @see CViewActivationTransaction
         * @see TViewActivationFlags
         */
        virtual CViewActivationTransaction* HandleViewActivationLC
            (const TUid& aViewId, const TVwsViewId& aPrevViewId,
            const TDesC* aTitlePaneText, 
            const CEikImage* aContextPanePicture,
            TUint aFlags=EUpdateAll) =0;

		/**
		 * Returns Phonebook app-level globals.
		 */
		virtual CPbkAppGlobalsBase* AppGlobalsL() =0;

        /**
         * Check critical FFS level
         * @param aCommandHandler command handler wrapper
         * @param aBytesToWrite how many bytes will be written to FFS.
		 *        Not known to default.
         */
        virtual void FFSClCheckL(const MPbkCommandHandler& aCommandHandler,
			TInt aBytesToWrite = 0) =0;

        /**
	     * Returns a pointer to the control environment. Does not imply transfer of ownership. 
	     */
	    inline CCoeEnv* ControlEnv() const { return iCoeEnv; }

    private: // Data
        /// Own: Phonebook command store
        MPbkCommandList* iCommandStore;
    };

#endif  // __CPbkAppUiBase_H__

// End of File
