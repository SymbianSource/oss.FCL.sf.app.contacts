/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The current extension does not provide a view.
*
*/

#ifndef C_CLOGSVIEWEXTENSION_H
#define C_CLOGSVIEWEXTENSION_H


//  INCLUDES
#include "MLogsViewExtension.h"

// FORWARD DECLARATIONS
class CSendUi;
class CSendingServiceInfo;
class CSPSettings;
class CSPEntry;

// CLASS DECLARATION

/**
 * View extension .
 */
NONSHARABLE_CLASS(CLogsViewExtension) : public CBase,
                                        public MLogsViewExtension
    {
friend class ut_clogsuicontrolextension;

public: 
    
    /**
     * Two-phased constructor.
     *
     * @since S60 v3.2
     */
    static CLogsViewExtension* NewL();
    
    /**
     * Two-phased constructor.
     *
     * @since S60 v3.2
     */
    static CLogsViewExtension* NewLC();

    /**
     * Destructor.
     */
    ~CLogsViewExtension();


protected:    

// From MLogsUiExtensionBase        

    /**
     * Releases this object and any resources it owns.
     *
     * @since S60 v3.2
     */
    void DoRelease();


// From MLogsViewExtension

    /**
     * From MLogsViewExtension
     * Filters the menu pane of the Logs view connect to this 
     * extension.
     *
     * @since S60 v3.2
     * @param aResourceId menu resource id.
     * @param aMenuPane menu pane which will be filtered.
     * @param aEvent getter of current event
     */
    void DynInitMenuPaneL(  TInt aResourceId, 
                            CEikMenuPane* aMenuPane,
                            const MLogsEventGetter* aEvent); 

    /**
     * From MLogsViewExtension    
     * Handles Logs extension commands.
     *
     * @since S60 v3.2
     * @param aCommandId command id.
     * @return ETrue if command was handled.
     */
    TBool HandleCommandL(TInt aCommandId);

    /**
     * Reference to SendUi from the Logs Application UI object.     
     *
     * @since S60 v3.2
     * @param aSendUi reference to SendUi object
     */
    void SetSendUi( CSendUi& aSendUi );
    
    /**
     * Set SendUi menu command text
     *     
     * @since S60 v3.2
     * @param aSendUiText  menu command text     
     */    
    void SetSendUiText( const TDesC& aSendUiText );
    
private: 
    
    /**
     * Constructor.
     */
    CLogsViewExtension();

    /**
     * Symbian second-phase constructor
     */  
    void ConstructL();
    
    /**
     * Verify if IM sending service provider exists.
     * @param aServiceProviderUid service provider uid to find in available services.
     * @param aServiceProviderExists ETrue if service is available.
     */
    void VerifyServiceProviderL( const TUid& aServiceProviderUid ,
                                 TBool& aServiceProviderExists );

    /**
     * Get WLM Send ui plugin UID from service table.
     * @param aServiceId service id to look for in service table.
     * @param aServiceProviderUid service provider uid from service table.     
     * @return ETrue if plugin uid was found in service table's entry.
     */
    TBool GetServicePluginUidL( TInt aServiceId, TUid& aServiceProviderUid ); 

    /**
     * Set contact link.
     * @param aContactLink contact link from logs.
     */    
    void SetContactlink( const TDesC8& aContactLink );

    /**
     * Set service id.
     * @param aServiceId service id from logs.
     */    
    void SetServiceId( const TUint32& aServiceId );

    /**
     * Set provider uid.
     * @param aUid uid of service provider from service table and is available in services list.
     */    
    void SetProviderUid( const TUid& aUid );
        
    /**
     * Send message.
     * @return ETrue if command was handled.
     */
    TBool SendMessageL();

private:
        
    /**
     * Services to exclude.         
     * Own     
     */
    CArrayFixFlat<TUid>* iServiceProvidersToDim;
        
    /**
     * Available services list.         
     * Own
     */
    RPointerArray<CSendingServiceInfo>* iServiceList;

    /**
     * SP settings.         
     * Own
     */    
    CSPSettings* iSettings;

    /**
     * SP setting's entry.         
     * Own
     */    
    CSPEntry* iEntry;
            
    /**
     * Reference to the SendUi from Logs Application UI object.
     * Not own
     */
    CSendUi* iSendUi;
        
    /**
     * SendUi command text
     */
    TPtrC iSendUiText;
        
    /**
     * Service id in service table
     */
    TUint32 iServiceId;

    /**
     * ContactLink
     */
    TPtrC8 iContactLink;    
        
    /**
     * Service UID of WLM Plugin
     */
    TUid iServiceProviderUid; 
        
    };

#endif // C_CLOGSVIEWEXTENSION_H
