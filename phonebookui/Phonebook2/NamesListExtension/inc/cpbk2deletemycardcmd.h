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
* Description:  Phonebook 2 delete mycard command.
*
*/

#ifndef CPBK2DELETEMYCARDCMD_H
#define CPBK2DELETEMYCARDCMD_H

// INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkBatchOperationObserver.h>

// FORWARD DECLARATIONS
class MPbk2ContactUiControl;
class MVPbkContactLink;
class MVPbkStoreContact;
class CVPbkContactLinkArray;
class CPbk2ApplicationServices;

// CLASS DECLARATION

/**
 * Phonebook 2 delete mycard command.
 */
NONSHARABLE_CLASS( CPbk2DeleteMyCardCmd ) :
    public CActive,
    public MPbk2Command,
    public MVPbkSingleContactOperationObserver,
    public MVPbkBatchOperationObserver
    {
public: // Construction and destruction

    /**
     * Creates a new instance of this class.
     *
     * @param aUiControl    UI control.
     * @param aLink Contact to be deleted
     * @return  A new instance of this class.
     */
    static CPbk2DeleteMyCardCmd* NewL(
            MPbk2ContactUiControl& aUiControl,
            const MVPbkContactLink& aLink );

    /**
     * Destructor.
     */
    ~CPbk2DeleteMyCardCmd();

public: // From MPbk2Command
    
    void ExecuteLD();
    void AddObserver( MPbk2CommandObserver& aObserver );
    void ResetUiControl(MPbk2ContactUiControl& aUiControl);

private: // From CActive
    
    void DoCancel();
    void RunL();
    TInt RunError( TInt aError );

private: // From MVPbkSingleContactOperationObserver
    
    void VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& aOperation,
        MVPbkStoreContact* aContact );
    void VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& aOperation, 
        TInt aError );

private: // From MVPbkBatchOperationObserver
    
    void StepComplete( 
        MVPbkContactOperationBase& aOperation,
        TInt aStepSize );
    TBool StepFailed( 
        MVPbkContactOperationBase& aOperation,
        TInt aStepSize, TInt aError );
    void OperationComplete( 
        MVPbkContactOperationBase& aOperation );

private: // Data structures
    /// Process states
    enum TProcessState
        {
        ERetrieving,
        EConfirming,
        EStarting,
        EDeleting,
        EStopping,
        ECanceling
        };

private: // Implementation
    
    /**
     * Constructor
     * 
     * @param aUiControl Active UI control  
     */
    CPbk2DeleteMyCardCmd( MPbk2ContactUiControl& aUiControl );

    /**
     * Constructor
     * 
     * @param aLink Contact to be deleted
     */
    void ConstructL( const MVPbkContactLink& aLink );

    /**
     * Issue asynchronous request to move into state
     * 
     * @param aState next state
     */
    void IssueRequest( TProcessState aState );
    
    /**
     * Complete the command process
     * 
     * @param aCancelCode result of the command process
     */
    void ProcessDismissed( TInt aCancelCode );

    /**
     * Retrieve contact
     * 
     * @param aContactLink link for the contact to retrieve 
     */
    void RetrieveContactL( const MVPbkContactLink& aContactLink );

    /**
     * Delete contact that was given in the construction
     */
    void DeleteContactL();
    
    /**
     * Show confirmation query for delete operation
     */
    void ConfirmDeletionL();

private: // Data
    /// Ref: UI control
    MPbk2ContactUiControl* iUiControl;
    /// Ref: Command observer
    MPbk2CommandObserver* iCommandObserver;
    
    /// Own: Current state of process
    TProcessState iState;
    /// Own: Retrieve operation
    MVPbkContactOperationBase* iRetrieveOperation;
    /// Own: Delete operation
    MVPbkContactOperationBase* iDeleteOperation;
    /// Own: The store contact to delete
    MVPbkStoreContact* iStoreContact;
    /// Own: Contact to be deleted (only one contact)
    CVPbkContactLinkArray* iContactsArray;
    /// Own: Application Services instance
    CPbk2ApplicationServices* iAppServices;
    };

#endif // CPBK2DELETEMYCARDCMD_H

// End of File
