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
* Description:  Phonebook 2 Group UI extension plug-in.
*
*/


// INCLUDES
#include <e32base.h>
#include <CPbk2UIExtensionPlugin.h>
#include <Pbk2PluginCommandListerner.h>

#include <Pbk2Commands.hrh>
#include <MVPbkBaseContact.h>
#include <MVPbkContactStoreObserver.h>

#ifndef NAMESLISTUIEXTENTIONPLUGIN_H
#define NAMESLISTUIEXTENTIONPLUGIN_H

// FORWARD DECLARATIONS
class CPbkAppViewBase;
class CPbk2AppGlobalsBase;
class MPbk2ContactUiControl;
class CPbk2PresentationContact;
class MPbk2ContactEditorControl;
class CAiwServiceHandler;
class CPbk2NamesListExView;
class CPbk2ApplicationServices;

// CCA
class MCCAConnection;
class MCCAParameter;
class CSpbContentProvider;
// CLASS DECLARATION

/**
 * Phonebook 2 Names List UI extension plug-in.
 * Creates instances of the extension classes.
 */
class CNamesListUIExtensionPlugin : public CPbk2UIExtensionPlugin,
                                    public MPbk2PluginCommandListerner,
                                    private MVPbkContactStoreObserver
    {
    public: // Construction and destruction

    /**
     * Creates a new instance of this class.
     *
     * @return  A new instance of this class.
     */
    static CNamesListUIExtensionPlugin* NewL();
    
    /**
     * Destructor.
     */
    ~CNamesListUIExtensionPlugin();
    
    private:	//new function
    	
    	/*
    	 * Creates content provider IF not already created and social phonebook is defined
    	 */
    	inline CSpbContentProvider& ContentProviderL();
    
    
    public: //MPbk2CommandListerner
    void HandlePbk2Command( TInt aCommand );
    
    private: // From CPbk2UIExtensionPlugin
    MPbk2UIExtensionView* CreateExtensionViewL( TUid aViewId,
                                                CPbk2UIExtensionView& aView );
    void DynInitMenuPaneL( TInt aResourceId,
                           CEikMenuPane* aMenuPane,
                           MPbk2ContactUiControl& aControl );
    void UpdateStorePropertiesL( CPbk2StorePropertyArray& aPropertyArray );

    MPbk2ContactEditorExtension* CreatePbk2ContactEditorExtensionL(
        CVPbkContactManager& aContactManager,
        CPbk2PresentationContact& aContact,
        MPbk2ContactEditorControl& aEditorControl );

    MPbk2SettingsViewExtension* CreatePbk2SettingsViewExtensionL();

    MPbk2Command* CreatePbk2CommandForIdL( 
        TInt aCommandId,
        MPbk2ContactUiControl& aUiControl ) const;

    MPbk2AiwInterestItem* CreatePbk2AiwInterestForIdL( 
        TInt aInterestId,
        CAiwServiceHandler& aServiceHandler ) const;

    TBool GetHelpContextL( TCoeHelpContext& aContext,
                           const CPbk2AppViewBase& aView,
                           MPbk2ContactUiControl& aUiControl );

    void ApplyDynamicViewGraphChangesL( CPbk2ViewGraph& aViewGraph);

    void ApplyDynamicPluginInformationDataL( CPbk2UIExtensionInformation& aUiExtensionInformation );
    
    TAny* UIExtensionPluginExtension( TUid aExtensionUid );

    private: // MPbk2UIExtensionFactory

    MPbk2ContactEditorExtension* CreatePbk2ContactEditorExtensionL(
                    CVPbkContactManager& aContactManager,
                    MVPbkStoreContact& aContact,
                    MPbk2ContactEditorControl& aEditorControl );
                    
    MPbk2ContactUiControlExtension*  CreatePbk2UiControlExtensionL(
                    CVPbkContactManager& aContactManager );
                    
    MPbk2SettingsViewExtension* CreatePbk2SettingsViewExtensionL(
                    CVPbkContactManager& aContactManager );
                    
    MPbk2AppUiExtension* CreatePbk2AppUiExtensionL(
                    CVPbkContactManager& aContactManager );
                    
    MPbk2Command* CreatePbk2CommandForIdL(
                    TInt aCommandId,
                    MPbk2ContactUiControl& aUiControl );
                    
    MPbk2AiwInterestItem* CreatePbk2AiwInterestForIdL(
                    TInt aInterestId,
                    CAiwServiceHandler& aServiceHandler );

    private: // From MVPbkContactStoreObserver
        
        void StoreReady(
                MVPbkContactStore& aContactStore );
        
        void StoreUnavailable(
                MVPbkContactStore& aContactStore,
                TInt aReason );
        
        void HandleStoreEventL(
                MVPbkContactStore& aContactStore,
                TVPbkContactStoreEvent aStoreEvent );
        
    private: // Implementation
    CNamesListUIExtensionPlugin();
    void ConstructL();
    void CmdOpenGroupL( MPbk2ContactUiControl& aUiControl ) const;
    void DimItem( CEikMenuPane* aMenuPane, TInt aCmd );
    TBool IsTopContact( const MVPbkBaseContact* aContact );
    MVPbkContactLink* MyCardLink() const;
    void InitLocalStoreObserverL();
    void InitAdnStoreObserverL();

    private: // Data
    MCCAConnection* iCCAConnection;  // own
    CPbk2NamesListExView* iNamesListExViewRef; //ref  
    //OWN:
    CSpbContentProvider*	iContentProvider;
    // number of contacts in local store
    TInt iLocalStoreContactsCount;
    // Not own:
    MVPbkContactStore* iLocalStore;
    MVPbkContactStore* iAdnStore;
    // Own
    CPbk2ApplicationServices*  iAppServices;
    };

#endif // NAMESLISTUIEXTENTIONPLUGIN_H

// End of File
