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
* Description:  Implementation of detailsview plugin control container
 *
*/

#ifndef C_CCAPPDETAILSVIEWCONTAINER_H
#define C_CCAPPDETAILSVIEWCONTAINER_H

// INCLUDES
#include <aknlists.h>
#include <ccappviewpluginakncontainer.h>
#include <eiklbo.h>
#include <Pbk2Commands.hrh>

#include <MPbk2ContactUiControl.h>
#include <MPbk2ContactUiControl2.h>

class CPbk2UIExtensionManager;
class CPbk2ApplicationServices;
class CVPbkContactManager;
class CCCAppDetailsViewListBoxModel;
class CPbk2CommandHandler;

class MVPbkStoreContact;
class MVPbkBaseContactField;
class MVPbkContactLink;
class MPbk2Command;
class MCCAppEngine;
class CCCAppDetailsViewPlugin;

// CLASS DECLARATION
class CCCAppDetailsViewContainer : public CCCAppViewPluginAknContainer,
                                   public MPbk2ContactUiControl,
                                   public MPbk2ContactUiControl2,
                                   public MEikListBoxObserver
{

public:

    /**
     * Constructor. This is called from plugin class NewContainerL.
     */
    CCCAppDetailsViewContainer(const TUid& aPluginUid,
            CCCAppDetailsViewPlugin& aPlugin);

    /**
     * Destructor.
     */
    ~CCCAppDetailsViewContainer();

public: // MCCAppEngine setters and getters

    /**
     * Reference to the cca application engine
     * not owned
     */
    MCCAppEngine& GetAppEngine()
            {
        return *iAppEngine;
            }
    void SetAppEngine(MCCAppEngine* aAppEngine)
            {
        iAppEngine = aAppEngine;
            }


public: // From CCoeControl

    void SizeChanged();

    void FocusChanged(TDrawNow aDrawNow);

    TInt CountComponentControls() const;

    CCoeControl* ComponentControl(TInt aIndex) const;

    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

    void ProcessCommandL( TInt aCommandId ) const;

    /**
     * From MEikListBoxObserver
     * (see details from baseclass )
     */
    void HandleListBoxEventL(
        CEikListBox *aListBox, TListBoxEvent aEventType ); 

    void GetHelpContext(TCoeHelpContext& aContext) const;

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
public: // implementation

    /**
     *
     */
    void SetApplicationServices(CPbk2ApplicationServices* aApplicationServices);

    /**
     *
     */
    CVPbkContactManager& ContactManager() const;

    /**
     * Helper function to get (temporary) listbox model.
     *
     * @see CCCAppDetailsViewListBoxModel
     * @return reference to listbox model object.
     */
    CCCAppDetailsViewListBoxModel& ListBoxModel() const;

private:

    // Called by ccapplication framework
    void ConstructL();

    /**
     * See more details from CCoeControl.
     */
    void HandleResourceChange(TInt aType);

    /**
     * Calls map commands.
     * 
     * @param aCommandId Command id 
     */
    void DoShowMapCmdL( TPbk2CommandId aCommandId );
    
    /*
    * Launch Call 
    */
    void HandleLaunchCallRequestL();
    

private: // data

    /**
     * Pointer to listbox.
     * Own.
     */
    CAknFormDoubleGraphicStyleListBox* iListBox;

    /**
     * Pointer to temporary listbox model.
     * Own.
     */
    CCCAppDetailsViewListBoxModel* iListBoxModel;

    /**
     * Reference to Application services.
     * Not own.
     */
    CPbk2ApplicationServices* iAppServices;

    /**
     * Reference to plugin uid.
     * Not own.
     */
    const TUid& iPluginUid;

    /**
     * Pointer to Pbk2 command handler.
     * Own.
     */
    CPbk2CommandHandler* iCommandHandler;

    /// Ref: command in execution
    MPbk2Command* iCommand;

    /// Own: UI Extension manager
    CPbk2UIExtensionManager* iExtensionManager;

    /**
     * Reference to the cca application engine
     * not owned
     */
    MCCAppEngine* iAppEngine;

    /**
     * Ref to plugin.
     * Not own.
     */
    CCCAppDetailsViewPlugin& iPlugin;
};

#endif // C_CCAPPDETAILSVIEWCONTAINER_H

// End of File
