/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook2 command execution handler
*
*/


#ifndef C_CCCAPPCOMMLAUNCHERPBKCMD_H
#define C_CCCAPPCOMMLAUNCHERPBKCMD_H

#include <e32base.h>

#include <RPbk2LocalizedResourceFile.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactStoreObserver.h>
#include <MPbk2ContactUiControl.h>
#include <MPbk2ContactUiControl2.h>
#include <MPbk2MenuCommandObserver.h>
#include <Pbk2Commands.hrh>

class CPbk2CommandHandler;
class CPbk2ApplicationServices;
class MVPbkContactOperationBase;
class MVPbkContactLinkArray;
class MVPbkBaseContactField;
class MVPbkContactLink;
class MPbk2Command;
class CCCAppCommLauncherPlugin;

/**
 *  Class to use Phonebook2 command implementations from out an external application.
 *  ?more_complete_description
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ccappcommlauncherplugin.dll
 *  @since S60 v3.2
 */
class CCCAppCommLauncherPbkCmd :
            public CBase,
            public MVPbkContactStoreObserver,
            public MVPbkSingleContactOperationObserver,
            public MPbk2ContactUiControl,
            public MPbk2ContactUiControl2,
            private MPbk2MenuCommandObserver
    {
public:

    static CCCAppCommLauncherPbkCmd* NewL( CCCAppCommLauncherPlugin& aPlugin );
    static CCCAppCommLauncherPbkCmd* NewLC( CCCAppCommLauncherPlugin& aPlugin );

    /**
    * Destructor.
    */
    virtual ~CCCAppCommLauncherPbkCmd();

public: // new
    void ExecutePbk2CmdAssignDefaultL( const TDesC8& aContact );
    void ExecutePbk2CmdShowMapL( const HBufC8* aContact, TPbk2CommandId aCommandId );
    void ExecutePbk2CmdViewImageL( const TDesC8& aContact );
    void ExecutePbk2CmdChangeImageL( const TDesC8& aContact );
    void ExecutePbk2CmdRemoveImageL( const TDesC8& aContact );
    void ExecutePbk2CmdAddImageL( const TDesC8& aContact );
    void ExecutePbk2CmdDeleteL( const TDesC8& aContact );
    void ExecutePbk2CmdSendBCardL( const TDesC8& aContact );
    TBool IsPbk2CommandRunning();
    //Close all the pending pbk2commands launched from CCA
    void DeleteAllRunningCmd();
    
public: // from MVPbkContactStoreObserver

    void StoreReady(
        MVPbkContactStore& aContactStore);

    void StoreUnavailable(
        MVPbkContactStore& aContactStore,
        TInt aReason);

    void HandleStoreEventL(
        MVPbkContactStore& aContactStore,
        TVPbkContactStoreEvent aStoreEvent );

public: // from MVPbkSingleContactOperationObserver

    void VPbkSingleContactOperationComplete(
            MVPbkContactOperationBase& aOperation,
            MVPbkStoreContact* aContact );

    void VPbkSingleContactOperationFailed(
            MVPbkContactOperationBase& aOperation,
            TInt aError );

public: // From MPbk2ContactUiControl

    MPbk2ContactUiControl* ParentControl() const;
    TInt NumberOfContacts() const;
    const MVPbkBaseContact* FocusedContactL() const;
    const MVPbkViewContact* FocusedViewContactL() const;
    const MVPbkStoreContact* FocusedStoreContact() const;
    void SetFocusedContactL(
        const MVPbkBaseContact& aContact );
    void SetFocusedContactL(
        const MVPbkContactBookmark& aContactBookmark );
    void SetFocusedContactL(
        const MVPbkContactLink& aContactLink );
    TInt FocusedContactIndex() const;
    void SetFocusedContactIndexL(
        TInt aIndex );
    TInt NumberOfContactFields() const;
    const MVPbkBaseContactField* FocusedField() const;
    TInt FocusedFieldIndex() const;
    void SetFocusedFieldIndex(
        TInt aIndex );
    TBool ContactsMarked() const;
    MVPbkContactLinkArray* SelectedContactsL() const;
    MVPbkContactLinkArray* SelectedContactsOrFocusedContactL() const;
    MPbk2ContactLinkIterator* SelectedContactsIteratorL() const;
    CArrayPtr<MVPbkContactStore>* SelectedContactStoresL() const;
    void ClearMarks();
    void SetSelectedContactL(
        TInt aIndex,
        TBool aSelected );
    void SetSelectedContactL(
        const MVPbkContactBookmark& aContactBookmark,
        TBool aSelected );
    void SetSelectedContactL(
        const MVPbkContactLink& aContactLink,
        TBool aSelected );

    void DynInitMenuPaneL(
        TInt aResourceId,
        CEikMenuPane* aMenuPane ) const;
    void ProcessCommandL(
        TInt aCommandId ) const;
    void UpdateAfterCommandExecution();
    TInt GetMenuFilteringFlagsL() const;
    CPbk2ViewState* ControlStateL() const;
    void RestoreControlStateL(
        CPbk2ViewState* aState );
    const TDesC& FindTextL();
    void ResetFindL();
    void ShowThumbnail();
    void HideThumbnail();
    void SetBlank(
        TBool aBlank );
    void RegisterCommand(
        MPbk2Command* aCommand );
    void SetTextL(
        const TDesC& aText );
    TAny* ContactUiControlExtension(TUid aExtensionUid );

public: // From MPbk2ContactUiControl2
    TInt CommandItemCount() const;
    const MPbk2UiControlCmdItem& CommandItemAt( TInt aIndex ) const;
    const MPbk2UiControlCmdItem* FocusedCommandItem() const;
    void DeleteCommandItemL( TInt aIndex );
    void AddCommandItemL(MPbk2UiControlCmdItem* /*aCommand*/, TInt /*aIndex*/);

private: // From MPbk2MenuCommandObserver
    void PreCommandExecutionL( const MPbk2Command& /*aCommand*/ ) {}
    void PostCommandExecutionL( const MPbk2Command& aCommand );
    
private:

    void ConstructL();
    CCCAppCommLauncherPbkCmd( CCCAppCommLauncherPlugin& aPlugin );

private:

    void PreparePbk2ApplicationServicesL();
    void HandleError( TInt aError );

public:
    /* 
    * API reponsible for returning reference to
    * Phonebook 2 application services.
    * CPbk2ApplicationServices is Responsible for owning and delivering commonly 
    * used shared objects and providing some common application level services.
    * @return - Pointer to Application services
    */
    CPbk2ApplicationServices& ApplicationServices() const;
private:
    /**
     * Pointer to Pbk2 command handler.
     * Own.
     */
    CPbk2CommandHandler* iCommandHandler;

    /**
     * Pointer to Application services.
     * Own.
     */
    CPbk2ApplicationServices* iAppServices;

    /// Own: Phonebook 2 commands dll resource file
    RPbk2LocalizedResourceFile iCommandsResourceFile;

    /// Own: Phonebook 2 UI controls dll resource file
    RPbk2LocalizedResourceFile iUiControlsResourceFile;

    /// Own: Phonebook 2 common UI dll resource file
    RPbk2LocalizedResourceFile iCommonUiResourceFile;

    /// Own: MVPbkStoreContact
    MVPbkStoreContact* iStoreContact;

    /// Own: MVPbkContactOperationBase
    MVPbkContactOperationBase* iOperation;

    /// Own: MVPbkContactLinkArray
    MVPbkContactLinkArray* iLinks;

    /// Ref: command in execution
    MPbk2Command* iCommand;

    /// Own: TPbk2CommandId
    TPbk2CommandId iPbk2CommandId;
    
    CCCAppCommLauncherPlugin& iPlugin;
    
    /// Own: contact store in use, has to be closed when done
    MVPbkContactStore* iContactStore;
    };

#endif // C_CCCAPPCOMMLAUNCHERPBKCMD_H
