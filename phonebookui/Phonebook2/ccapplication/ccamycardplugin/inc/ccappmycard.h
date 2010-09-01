/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of MyCard
*
*/

#ifndef CCAPPMYCARD_H
#define CCAPPMYCARD_H

#include <e32base.h>
#include <MVPbkContactStoreObserver.h>
#include <MVPbkSingleContactLinkOperationObserver.h>
#include <MVPbkContactObserver.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MPbk2EditedContactObserver.h>	
#include <MPbk2ExitCallback.h>

class CVPbkContactManager;
class MVPbkContactLink;
class MVPbkStoreContact;
class CPbk2FieldPropertyArray;
class CPbk2StoreSpecificFieldPropertyArray;
class CPbk2PresentationContact;
class MPbk2ContactNameFormatter;
class CPbk2SortOrderManager;
class RFs;
class CCreateMyCard;
class CPbk2ApplicationServices;
class CCCAppMyCardPlugin;	
class MPbk2DialogEliminator;
class CTimerCallBack;

/**
 * MyCard Observer class
 */
class MMyCardObserver
    {
public:
    /**
     * MyCard observer event types
     */
    enum TEvent
        {
        /// Undefined event
        EEventNull = 0,
        /// Contact loaded for the first time
        EEventContactLoaded,
        /// Contact has been changed
        EEventContactChanged
        };
    
    /**
     * Notification method to report MyCard events to observers
	 * @param aEvent event type
     */
    virtual void MyCardEventL( TEvent aEvent ) = 0;

protected:
    virtual ~MMyCardObserver() {}
    };

/**
 *
 *
 *  @lib ccappmycardplugin.dll
 *  @since S60 9.2
 */
NONSHARABLE_CLASS( CCCAppMyCard ) : 
		public CBase, 
		public MVPbkContactStoreObserver,
		public MVPbkSingleContactLinkOperationObserver,
		public MVPbkSingleContactOperationObserver,
		public MVPbkContactObserver,
		public MPbk2EditedContactObserver,
		public MPbk2ExitCallback
    {

public:

    /**
     * Two-phased constructor.
     */
    static CCCAppMyCard* NewL( CCCAppMyCardPlugin& aPlugin, RFs* aFs = NULL );

    /**
     * Destructor.
     */
    ~CCCAppMyCard();

public: // New methods
    
	/*
	 * Locks contact and launches the contact editor
	 * @param	aFocusedFieldIndex, focused contact field index
	 */
	void EditContactL( TInt aFocusedFieldIndex );
	
	/**
     * Getter for presentation contact of own contact.
     *   
     * @return presentation contact
     */
    CPbk2PresentationContact& PresentationContactL();
    
    /**
     * Getter for store contact of own contact.
     *   
     * @return phonebook store contact
     */
    MVPbkStoreContact& StoreContact();
    
    /**
     * Getter for Contact Manager
     * 
     * @return phonebook contact manager
     */
    CVPbkContactManager& ContactManager();   

    /**
     * Check if contact link already available. This should be called before
     * calling ContactLink
     * 
     * @return ETrue if contact link is available
     */
    TBool IsContactLinkReady() const;
    
    /**
     * Getter for contact link of own contact. Will panic if contact link is 
     * not available.
     *   
     * @return phonebook contact link
     */
    MVPbkContactLink& ContactLink();
    
    /**
     * Add observer to MyCard
     * 
     * @param aObserver New observer for MyCard
     */
    void AddObserverL( MMyCardObserver* aObserver );
    
    /**
     * Remove observer from MyCard
     * 
     * @param aObserver Observer to be removed from MyCard
     */
    void RemoveObserver( MMyCardObserver* aObserver );

    /**
     * Set contact link. If link is provided it will be used instead of resolving
     * the own contact link from contact store. This should be called before call to
     * FetchMyCardL().
     * 
     * @param aLink contact link 
     */
    void SetLinkL( const MVPbkContactLink& aLink );
    
    /**
     * Start fetching the data of own contact and observe it's changing.
     */
    void FetchMyCardL();
    
    /**
     * Force MyCard to open editor and create new own contact once the 
     * contact store is opened. This should be called before calling FetchMyCardL.
     */
    void ForceCreateMyCard();
    
    /*
    * Returns information about header control blocking
    * 
    * @return ETrue if the header control events are blocked, EFalse otherwise 
    */
    TBool HeaderControlBlocked();
    
private: // from MVPbkContactStoreObserver
     void StoreReady(MVPbkContactStore& aContactStore);
     void StoreUnavailable(MVPbkContactStore& aContactStore, 
             TInt aReason);
     void HandleStoreEventL(
             MVPbkContactStore& aContactStore, 
             TVPbkContactStoreEvent aStoreEvent);

private: // From MVPbkSingleContactLinkOperationObserver
    void VPbkSingleContactLinkOperationComplete(
            MVPbkContactOperationBase& aOperation,
            MVPbkContactLink* aLink );
    void VPbkSingleContactLinkOperationFailed(
            MVPbkContactOperationBase& aOperation,
            TInt aError );

private: // From MVPbkSingleContactOperationObserver
    virtual void VPbkSingleContactOperationComplete(
            MVPbkContactOperationBase& aOperation,
            MVPbkStoreContact* aContact );
    virtual void VPbkSingleContactOperationFailed(
            MVPbkContactOperationBase& aOperation, 
            TInt aError );    

private: // From MVPbkContactObserver
    void ContactOperationCompleted(TContactOpResult aResult);
    void ContactOperationFailed
        (TContactOp aOpCode, TInt aErrorCode, TBool aErrorNotified);    
   
private:	// form MPbk2EditedContactObserver
	 void ContactEditingComplete(
	                MVPbkStoreContact* aEditedContact );
	 void ContactEditingDeletedContact(
	                MVPbkStoreContact* aEditedContact );
	 void ContactEditingAborted();
    
private: // From MPbk2ExitCallback
     TBool OkToExitL( TInt aCommandId );

private: // constructors
    inline CCCAppMyCard( CCCAppMyCardPlugin& aPlugin );
    inline void ConstructL(RFs* aFs);

private:  // new functions
	/*
	 * Function for launching contact editor.
	 * 
	 * @param	aFlags - flags for opening the editor (@see TPbk2ContactEditorParams)
	 */
	void LaunchContactEditorL( TUint32 aFlags );
	
    /**
     * Notify all observers that aEvent has occured.
     * 
     * @param aEvent Occured event.
     */
    void NotifyObservers( MMyCardObserver::TEvent aEvent ) const;
    
    /**
     * Load own contact information
     */
    void LoadContact();
    
    /*
     * Async callback function. Calls  LaunchContactEditorL - function
     */
    static TInt CreateMyCardContact( TAny* aPtr );
    
    /*
     * Async callback function for closing CCa. Calls DoCloseCCa
     */
    static TInt CloseCcaL( TAny* aPtr );
    
    /**
     * Async callback function for closing editor dialog.
     */
    static TInt ExitDlgL( TAny* aPtr );
    
    /**
     * Async callback function for opening stores.
     */
    static TInt OpenStoresL( TAny* aPtr );

    /*
     * Closes the CCA
     */
    void DoCloseCCaL();
    

private: // data
	
	/// REF
	CVPbkContactManager* iVPbkContactManager;
	/// Own
	MVPbkContactOperationBase* iOperation;
    /// Own
    MVPbkContactOperationBase* iFetchOperation;
	/// Own
	MVPbkContactLink* iMyCard;
	/// Own
	MVPbkStoreContact* iMyCardContact;
	/// Own.
	CPbk2PresentationContact* iPresentationContact;
	/// Own
	CPbk2StoreSpecificFieldPropertyArray* iSpecificFieldProperties;
    /// Own
	CPbk2FieldPropertyArray* iFieldProperties;
	/// Elements are not owned.
	RPointerArray<MMyCardObserver> iObservers;
	/// Own: Pointer to Application services
	CPbk2ApplicationServices* iAppServices;
	/// Own: Asynchronous callback for close
	CAsyncCallBack*	iCloseCallBack; 
    /// Own: Asynchronous callback for create
    CAsyncCallBack* iCreateCallBack; 
    /// Own: Asynchronous callback for close dialog
    CAsyncCallBack* iDlgCloseCallBack; 
	/// Ref: plugin
	CCCAppMyCardPlugin& iPlugin;
	/// Focused contact field index for the editor
	TInt iFocusedFieldIndex;	
	/// Now own. Editor dialog eliminator
	MPbk2DialogEliminator* iEditorEliminator;
	/// Current event
	MMyCardObserver::TEvent iEvent;
	/// Own. Store opener callback
	CTimerCallBack* iStoreCallBack;
	/// Force editor flag
	TBool iForceCreateMyCard;
	// Flag for editor state
	TBool iDialogIsRunning;	
	};

#endif // CCAPPMYCARD_H

// End of File
