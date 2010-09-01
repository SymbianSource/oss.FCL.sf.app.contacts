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
* Description:  Definition of the class CFscSendBusinessCardPluginImpl.
 *
*/


#ifndef CFscSendBusinessCardPluginImpl_H
#define CFscSendBusinessCardPluginImpl_H

#include <e32def.h>
#include "cfsccontactactionplugin.h"
#include "mfscreasoncallback.h"
#include "mfsccontactsetobserver.h"
#include "FscActionPluginUtils.h"
#include "mfsvcardconverterobserver.h"

#include <MVPbkContactStoreObserver.h>

// FORWARD DECLARATIONS
class CFscContactAction;
class CMessageData;
class CIdle;
class CSendUi;
class TFSSendingParams;
class CVPbkVCardEng;
class CFscvCardConverter;
class MPbk2ContactNameFormatter;
class CPbk2SortOrderManager;
class CCoeEnv;

/**
 *  Send Business Card plugin implementation.
 *  Contains implementation of Sending Business Card contact action plugin.
 *
 *  @lib fscsendbusinesscardplugin.lib
 *  @since S60 5.0
 */
class CFscSendBusinessCardPluginImpl : public CFscContactActionPlugin,
									public MFscReasonCallback, 
									public MFscContactSetObserver,
									public MVPbkContactStoreObserver,
									public MFsvCardConverterObserver
    {

public:

    /**
     * Two-phased constructor.
     *
     * @param aParams contact action plugin parameters
     * @return New instance of CFscSendBusinessCardPluginImpl
     */
    static CFscSendBusinessCardPluginImpl* NewL(
            TAny* aParams);

    /**
     * Destructor.
     */
    virtual ~CFscSendBusinessCardPluginImpl();

public:
    // From CFscContactActionPlugin

    /**
     * @see CFscContactActionPlugin::Uid
     */
    virtual TUid Uid() const;

    /**
     * @see CFscContactActionPlugin::ActionList
     */
    virtual const CArrayFix<TUid>* ActionList() const;

    /**
     * @see CFscContactActionPlugin::GetActionL
     */
    virtual const MFscContactAction& GetActionL(
            TUid aActionUid) const;

    /**
     * @see CFscContactActionPlugin::PriorityForContactSetL
     */
    virtual void PriorityForContactSetL(
            TUid aActionUid,
            MFscContactSet& aContactSet,
            TFscContactActionVisibility& aActionMenuVisibility,
            TFscContactActionVisibility& aOptionsMenuVisibility,
            MFscContactActionPluginObserver* aObserver);

    /**
     * @see CFscContactActionPlugin::CancelPriorityForContactSet
     */
    virtual void CancelPriorityForContactSet();

    /**
     * @see CFscContactActionPlugin::ExecuteL
     */
    virtual void ExecuteL(
            TUid aActionUid,
            MFscContactSet& aContactSet,
            MFscContactActionPluginObserver* aObserver);

    /**
     * @see CFscContactActionPlugin::CancelExecute
     */
    virtual void CancelExecute();
public:
    // From MFscReasonCallback

    /**
     * @see MFscReasonCallback::GetReasonL
     */
    virtual void GetReasonL(
            TUid aActionUid,
            TInt aReasonId,
            HBufC*& aReason) const;

public:
    // From MFscContactSetObserver
    /**
     * @see MFscContactSetObserver::NextContactComplete
     */
    virtual void NextContactComplete(
            MVPbkStoreContact* aContact);

    /**
     * @see MFscContactSetObserver::NextContactFailed
     */
    virtual void NextContactFailed(
            TInt aError);

    /**
     * @see MFscContactSetObserver::NextGroupComplete
     */
    virtual void NextGroupComplete(
            MVPbkStoreContact* aContact);

    /**
     * @see MFscContactSetObserver::NextGroupFailed
     *
     */
    virtual void NextGroupFailed(
            TInt aError);

    /**
     * @see MFscContactSetObserver::GetGroupContactComplete
     */
    virtual void GetGroupContactComplete(
            MVPbkStoreContact* aContact);

    /**
     * @see MFscContactSetObserver::GetGroupContactFailed
     */
    virtual void GetGroupContactFailed(
            TInt aError);

private:
    // methods

    /**
     * Send BusinessCard
     */
    void SendBusinessCardL();
    
    /**
     * Prepares store for update
     */
    void PrepareStoreL();

    
    /**
     * Checks if store is valid for saving contacts from RCL
     */
    TBool IsValidStoreL();

    /**
     * Helper method to check if action can be executed with 
     * given contacts in current context
     *
     * @param aActionId Action Id
     * @param aContactSet contains contact list and group lists
     */
    void CanExecuteL(
            TUid aActionUid,
            MFscContactSet& aContactSet);

    /**
     * Updates action icon
     *
     * @param aActionId Action Id
     */
    void UpdateActionIconL(
            TUid aActionUid);

    private:

    /**
     * Constructor.
     *
     * @param aParams contact action plugin parameters
     */
    CFscSendBusinessCardPluginImpl(
            const TFscContactActionPluginParams& aParams);

    /**
     * Second phase constructor.
     */
    void ConstructL();
    
    /**
     * Restores members related to state machine
     */
    void ResetData();
    
    /**
     * Resumes plugin processing
     * Resumes ExecuteL or PriorityForContactSetL execution
     * 
     * @param aActionUid action UID
     * @param aContactSet set of contacts
     * @param aIsExecute is ExecuteL or PriorityForContactSetL launched
     */
    void ResumeAsync(
            TUid aActionUid,
            MFscContactSet& aContactSet,
            TBool aIsExecute );
    
private:
    
    //From MVPbkContactStoreObserver
    
    /**
     * @see MVPbkContactStoreObserver::StoreReady
     */
     void StoreReady(MVPbkContactStore& aContactStore);

     /**
      * @see MVPbkContactStoreObserver::StoreUnavailable
      */
     void StoreUnavailable(MVPbkContactStore& aContactStore, 
             TInt aReason);

     /**
      * @see MVPbkContactStoreObserver::StoreUnavailable
      */
     void HandleStoreEventL(
             MVPbkContactStore& aContactStore, 
             TVPbkContactStoreEvent aStoreEvent);
     
private:    
     
     TUid ShowSendQueryL();
     void SendvCardsLD();
     static TInt SendvCardsLD(
                     TAny* aThis );
     TFSSendingParams CreateParamsLC();
     void ConvertContactL(TInt aSelectionIndex);
     void SendMsgUsingSendUI(CMessageData* aMsgData);
     TBool IsMoreThanOneContact();
     TInt FilterErrors(TInt aErrorCode);
     void ProcessDone(TInt aErrCode);
     TInt SelectionListL() const;
     void MapSelection(
             TInt& aSelection,
             TInt aShownMenu );
     TInt SelectSentDataL();
     TBool AnyThumbnailsL() const;
     
private: //from MFsvCardConverterObserver
    void ConversionDone(TInt aCount);
    void ConversionError(TInt aError);


private:
    // data

    /**
     * Action plugin parameters
     */
    TFscContactActionPluginParams iParams;

    /**
     * Resource handle
     */
    TInt iResourceHandle;

    /**
     * Own: Send Business Card contact action
     */
    CFscContactAction* iSendBusinessCardContactAction;

    /**
     * Own: Contact action array
     */
    CArrayFixFlat<TUid>* iActionList;

    // For async implementation
    
    /**
     * Action Uid
     */
    TUid iActionUid;
    
    /**
     * Contact set
     */
    MFscContactSet* iContactSet;
    
    /**
     * Action menu visbility
     */
    TFscContactActionVisibility* iActionMenuVisibility;
    
    /**
     * Options menu visbility
     */
    TFscContactActionVisibility* iOptionsMenuVisibility;
    
    /**
     * Plug-in observer
     */
    MFscContactActionPluginObserver* iPluginObserver;
    
    /**
     * Current state of state machine 
     */
    TActionPluginLastEvent iLastEvent;
    
    /**
     * Action priority
     */
    TInt iActionPriority;
    
    /**
     * Can display action
     */
    TInt iCanDisplay;

    /**
     * Retrieved store contact
     */
    MVPbkStoreContact* iRetrievedStoreContact;

    
    /**
     * Is ExecuteL or PriorityForContactSetL launched
     */
    TBool iIsExecute;
    
    
    /**
     * Ref: Target store
     */
	MVPbkContactStore* iTargetStore;
	
	/// Own: Store contact array
	RPointerArray<MVPbkStoreContact> iStoreContacts;
	
	/// Own: vCard engine
    CVPbkVCardEng* iVCardEngine;    
    
    /// Own: vCard sender
    CIdle* iVCardSender;
    
    /// Own: Send UI
    CSendUi* iSendUi;
    
    /// Own: Send command's service uid
    TUid iMtmUid;
    
     ///Owns
    CPbk2SortOrderManager* iSortOrderManager;   
    
    ///Owns NameFormatter
    MPbk2ContactNameFormatter* iNameFormatter;
    
    /// Own: vCard converter
    CFscvCardConverter* iConverter;   
    
    ///Doesnt Own
    CCoeEnv& iCoeEnv;
    };

#endif // CFscSendBusinessCardPluginImpl_H


// End of File
