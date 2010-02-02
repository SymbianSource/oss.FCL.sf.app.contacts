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
* Description:  Provides additional functionality to the Logs UI application
*
*/



#ifndef C_CLOGSUICONTROLEXTENSION_H
#define C_CLOGSUICONTROLEXTENSION_H


#include "MLogsUiControlExtension.h"
#include "mlogsextbrandfetcherobserver.h"
#include "mlogsextpresentityidfetcherobserver.h"
#include "mlogsextservicehandlerobserver.h"

#include "MLogsModel.h"
#include "MLogsEventGetter.h"
#include "MLogsExtObserver.h"
#include "LogsConsts.h"
#include "logsextconsts.h"
#include "MLogsEventGetter.h"

class CAknIconArray;
class CAknDoubleGraphicStyleListBox;
class CLogsPresenceTableManager;
class CLogsExtBrandFetcher;
class CLogsExtPresentityIdFetcher;
class CLogsEventInfoManager;
class TLogsEventInfo;
class CLogsExtServiceHandler;
class RFile;

/**
 *  Logs Ui Extension
 *
 *  Implements the MLogsUiControlExtension interface which is used by the
 *  Logs application to provide the support for the branding icon / 
 *  presence status icon appearance.
 *
 *  @since S60 v3.2 
 */
NONSHARABLE_CLASS(CLogsUiControlExtension) : 
        public CBase,
        public MLogsUiControlExtension,
        public MLogsExtBrandFetcherObserver,        
        public MLogsExtPresentityIdFetcherObserver,
        public MLogsExtServiceHandlerObserver
    {
    
public: 

    /**
     * Two-phased constructor.
     *
     * @since S60 v3.2
     */
    static CLogsUiControlExtension* NewL();
    
    /**
     * Two-phased constructor.
     *
     * @since S60 v3.2
     */
    static CLogsUiControlExtension* NewLC();        
        
    /**
     * Destructor.
     *
     * @since S60 v3.2
     */
    ~CLogsUiControlExtension();

public:

// from MLogsUiControlExtension        
       
    /**
     * From MLogsUiControlExtension
     * Method does nothing.
     *
     * @since S60 v3.2
     * @param aArray array of icons
     */
    void AppendIconsL( CAknIconArray* aArray );

    /**
     * From MLogsUiControlExtension
     * Method does nothing.
     *
     * @since S60 v3.2
     * @param aContactId contact id
     * @param aIndex icon index
     * @return ETrue if found, otherwise EFalse
     */
    TBool GetIconIndexL( TContactItemId aContactId, TInt& aIndex );
        
    /**
     * From MLogsUiControlExtension
     * Creates and starts all the icon fetching processes
     *
     * @since S60 v3.2
     * @param aModel the data model that contains the
     * information about the log event entries in the current view.
     * @param aListBox fetched icons will be added to the icon array
     * of the specified listbox
     */
    void HandleAdditionalData( 
            MLogsModel& aModel, 
            CAknDoubleGraphicStyleListBox& aListBox );
        
    /**
     * From MLogsUiControlExtension
     * Modifies the given icon string.
     *
     * @since S60 v3.2
     * @param aDes the icon string to be modified
     * @param aServiceId the service id of the log event
     * this icon string is modified for
     */ 
    void ModifyIconString( 
            TDes& aDes, 
            const MLogsEventGetter& aLogsEventGetter );
        
    /**
     * From MLogsUiControlExtension
     * Sets the observer of the extension
     *
     * @since S60 v3.2
     * @param aObserver the observer that handles the callback
     * of this extension.
     */ 
    void SetObserver(MLogsExtObserver& aObserver);
        
        
// from MLogsExtBrandFetcherObserver

    /**
     * From MLogsExtBrandFetcherObserver
     * Callback for the brand fetcher objects.
     *
     * @since S60 v3.2
     * @param aServiceId service id
     * @param aBitmapFile file handle to bitmap file
     * @param aBitmapId id of the the presence icon's bitmap in the file
     * @param aBitmapMaskId id of the the presence icon's mask in the file
     */
    void BrandIconFetchDoneL( const TUint32 aServiceId,
                              RFile& aBitmapFile,
                              const TInt aBitmapId,   
                              const TInt aBitmapMaskId );                              
                              

// from MLogsExtPresentityIdFetcherObserver                              

    /**
     * From MLogsExtPresentityIdFetcherObserver
     * Callback for presentity id fetcher objects.     
     *
     * @since S60 v3.2
     * @param aServiceId a service id
     * @param aPresentityId the presentity id that was retrieved 
     *        by the fetcher
     * @param aLogId a unique event id associated with a log event
     */                                             
    void PresentityIdFetchDoneL( const TUint32 aServiceId, 
                                 const TDesC& aPresentityId,
                                 const TLogId aLogId );

// from MLogsExtServiceHandlerObserver
    
    /**
     * From MLogsExtServiceHandlerObserver
     * Handles the service handler's callback.
     *
     * @since S60 v3.2
     * @param aServiceId service id
     * @param aPresentityId presentity id
     * @param aPresenceStatus a presence status
     * @param aShowStatus presentity presense status display flag
     */
    void HandlePresentityPresenceStatusL( 
                const TUint32 aServiceId, 
                const TDesC& aPresentityId,
                const TDesC& aPresenceStatus,
                TBool aShowStatus );
    
    /**
     * From MLogsExtServiceHandlerObserver
     * This function is used to notify the observer of a service handler
     * if the presence status of a certain presentity should be displayed
     * or not.
     *
     * @since S60 v3.2
     * @param aServiceId service id
     * @param aPresentityId presentity id
     * @param aShowStatus presentity presense status display flag
     */
    void SetDisplayStatusFlag( const TUint32 aServiceId, 
                               const TDesC& aPresentityId,
                               TBool aShowStatus );
    
private: 

    /**
     * Constructor.
     */
    CLogsUiControlExtension();

    /**
     * Symbian second-phase constructor
     */ 
    void ConstructL();
       

// from MLogsUiExtensionBase
        
    /**
     * From MLogsUiExtensionBase
     * Releases this object and any resources it owns.
     *
     * @since S60 v3.2
     */
    void DoRelease();


private:

    /**
     * Creates and starts all the icon fetching processes
     *
     * @since S60 v3.2
     * @param aModel the data model that contains the information about 
     *        the log event entries in the current view.
     */ 
    void DoAdditionalDataHandlingL( MLogsModel& aModel );
        
    /**
     * Notifies the update function of the observer of this extension.
     * 
     * @since S60 v3.2
     */ 
    void NotifyObserverL();        
 
    /**
     * Checks whether servicehandler with the specified service id already 
     * exists
     *
     * @since S60 v3.2
     * @param aServiceId a service id
     * @return ETrue, if there is already a service handler for the specified 
     *         service id; EFalse otherwise
     */ 
    TBool ServiceHandlerExists( const TUint32 aServiceId );

    
    /**
     * Checks whether brandfetcher with the specified service id already 
     * exists
     *
     * @since S60 v3.2
     * @param aServiceId a service id
     * @return ETrue, if there is already a fetcher for the specified 
     *         service id; EFalse otherwise
     */ 
    TBool BrandFetcherExists( const TUint32 aServiceId );


    /**
     * Checks whether a presence fetcher for the specified service id 
     * and presentity id already exists
     *
     * @since S60 v3.2
     * @param aServiceId a service id
     * @param aPresentityId a presentity id
     * @return ETrue, if there is already a fetcher for the specified 
     *         service id and presentity id; EFalse otherwise
     */ 
    TBool PresenceFetcherExists( const TUint32 aServiceId, 
                                 const TDesC& aPresentityId );

 
    /**
     * Stores the brand icon's index in the listbox's icon array
     * to a manager, that keeps track of this information.
     *
     * @since S60 v3.2
     * @param aServiceId a service id
     * @param aIndex the index
     * @return KErrNone, if setting the index succeeded or one errorcode
     *         if setting failed(e.g. KErrNotFound)
     */ 
    TInt SetBrandIndex( const TUint32 aServiceId, 
                        const TInt aIndex );



    /**
     * Stores the presence icon's index in the listbox's icon array
     * to a manager, that keeps track of this information.
     *
     * @since S60 v3.2
     * @param aServiceId a service id
     * @param aPresenceStatus a presence status
     * @param aIndex the index
     * @return KErrNone, if setting the index succeeded; system-wide
     *         errorcode otherwise
     */
    TInt SetPresenceIndex( const TUint32 aServiceId, 
                           const TDesC& aPresenceStatus,
                           const TInt aIndex );


    /**
     * Creates a proper icon and adds it to the listbox' icon array.
     *
     * @since S60 v3.2
     * @param aBitmap the bitmap of the fetched icon
     * @param aBitmask the bitmask of the fetched icon
     * @param aIndex the index of the appended icon in the icon array
     * @return The index of the added icon in the icon array 
     */ 
    void AddIconL( CFbsBitmap* aBitmap, CFbsBitmap* aBitmask, TInt& aIndex );
    
    /**
     * Performs the icon string modification.
     *
     * @since S60 v3.2
     * @param aDes reference of the icon string the descriptor 
     * @param aDataField a unparsed data field of a log event
     */ 
    void DoIconStringModificationL( 
            TDes& aDes, 
            const MLogsEventGetter& aLogsEventGetter );
            
    
    /**
     * Adds the Branding icon index to a local copy of the icon string
     * that we want to modify.
     *
     * @since S60 v3.2
     * @param aTempDes the icon string where we append the branding
     *        icon
     * @param aServiceId service id
     * @return True, if appending successful, false otherwise
     */ 
    TBool AddBrandIconIndex( TDes& aTempDes, const TUint32 aServiceId );
    
    /**
     * Copies that part of the global icon string to the local copy 
     * which contains the non-icon index related data.
     *
     * @since S60 v3.2
     * @param aDes the original icon string
     * @param aTempDes the icon string where we append the data
     * @return True, if appending successful, false otherwise
     */ 
    TBool AddNonIndexData( const TDesC& aDes, TDes& aTempDes );
    
    /**
     * Appends the trailing delimiters (if they do not already exist)
     * and the presence icon index.
     *
     * @since S60 v3.2
     * @param aTempDes the icon string where we append the data
     * @param aServiceId service id
     * @param aPresentityId presentity id  
     * @return True, if appending successful, false otherwise
     */ 
    TBool AddPresenceIconIndex( TDes& aTempDes, 
                                const TUint32 aServiceId, 
                                const TDesC& aPresentityId );
    
    /**
     * Appends 'aNumber' trailing delimiters to the specified icon string.
     *
     * @since S60 v3.2
     * @param aTempDes the icon string where we append the data
     * @param aNumber the number of trailing delimiters that should
     *        be appended to aTempDes
     * @return KErrNone, if appending was successful, KErrNoMemory otherwise
     */ 
    TInt AppendTrailingDelimiters( TDes& aTempDes, TInt aNumber );

    /**
     * Determines the number of delimiters in the specified string.
     *
     * @since S60 v3.2
     * @param aTempDes a string
     * @return the number of delimiters 'aTempDes' contains
     */ 
    TInt DetermineNumberDelimiters( const TDesC& aTempDes );

    /**
     * Deletes the bitmaps.
     *
     * @since S60 v3.2
     */
    void DeleteBitmaps();
    
    /**
     * Creates a certain service handler.
     *
     * @since S60 v3.2
     * @param aServiceId a service id
     */        
    void CreateAndBindServiceHandlerL( TUint32 aServiceId );
    
    /**
     * Creates and starts the brand fetchers.
     *
     * @since S60 v3.2
     * @param aModel the data model that contains the
     * information about the log event entries in the current view.
     */    
    void CreateAndStartBrandFetchersL( MLogsModel& aModel );
    
    /**
     * Initiates the presence / presentity id  fetcher creation.
     *
     * @since S60 v3.2
     * @param aModel the data model that contains the
     * information about the log event entries in the current view.
     */    
    void CreateAndStartPresentityIdFetchersL( MLogsModel& aModel );
    
    
    /**
     * Adds a single presentity to a service handler.
     *
     * @since S60 v3.2
     * @param aServiceId service id
     * @param aPresentityId presentity id
     */    
    void AddPresentityToServiceHandlerL( const TUint32 aServiceId, 
                                         const TDesC& aPresentityId );


    /**
     * Creates and starts one presentity id fetcher.
     *
     * @since S60 v3.2
     * @param aServiceId service id
     * @param aCntLink a contact link
     * @param aLogId a log id
     */    
    void DoPresentityIdFetcherCreationL( const TUint32 aServiceId, 
                                         const TDesC8& aCntLink,
                                         const TLogId aLogId  );
                                     
    /**
     * Retrieves a presentity id using the specified log id.
     *
     * @since S60 v3.2
     * @param aLogId a log id
     * @return the retrieved presentity id or NULL if no mapping was found 
     *         for the specified lod id.
     */
    const TDesC* LookupPresentityId( const TLogId aLogId );
    
    /**
     * Adds another mapping to the collection of mappings.
     *
     * @since S60 v3.2
     * @param aLogId a log id
     * @param aPresentityId presentity id     
     */    
    void AddLogIdToPresentityIdMappingL( const TLogId aLogId, 
                                         const TDesC& aPresentityId );
   
    /**
     * Resets the presentity id fetcher array and also deletes the objects
     * whose pointers are stored in the array.
     *
     * @since S60 v3.2     
     */    
    void ResetPresentityIdFetcherArray();    
    
    /**
     * Resets the presence status of all entries in the status table.
     */
    void ResetPresenceStatuses();    
    
    /**
     * Gets the service handler of the specified service.
     *
     * @since S60 v3.2
     * @param aServiceId a service id
     * @param aServiceHandler service handler object pointer. 
     * @return KErrNone in case data has been retrieved successfully,
     *         system-wide errorcode otherwise 
     */
    TInt GetServiceHandler( TUint32 aServiceId, 
                            CLogsExtServiceHandler*& aServiceHandler );

    /**
     * Retrieves the icon bitmaps from the branding server
     *
     * @since S60 v3.2
     * @param aServiceId service id. 
     * @param aPresenceStatus the current presence status.
     * @param aBitmapFile file handle to bitmap file
     * @param aBitmapId id of the presence image bitmap in the file
     * @param aBitmapMaskId id of the presence image mask in the file
     */
    void FetchBitmapsFromBrandingServerL( 
            const TUint aServiceId,
            const TDesC& aPresenceStatus,
            RFile& aBitmapFile,
            TInt& aBitmapId,
            TInt& aBitmapMaskId );
    
private: // data

    /**
     * A reference to a listbox. The fetched icons will be added 
     * to this listbox.
     * Not own.
     */        
    CAknDoubleGraphicStyleListBox* iListBox;

    /**
     * The observer of this extension.
     * Not own. 
     */
    MLogsExtObserver* iLogsExtObserver;

    /**
     * Stores the information about the service ids and the brand
     * icon indices that correspond to the service ids of each
     * log event.
     * Own.
     */
    CLogsEventInfoManager* iLogEntryInfoManager;    

    
    /**
     * Stores presence related information. Uses 
     * CLogsPresenceStatusTableInformation and 
     * CLogsPresenceIndexTableInformation objects in order to achieve that.
     * Own.
     */
    CLogsPresenceTableManager* iPresenceTableManager;
    
    /**
     * The array that contains the different fetchers 
     * Own. 
     */  
    RPointerArray<CLogsExtBrandFetcher> iBrandFetcherArray;
        
    /**
     * The array that contains the presence fetchers. 
     * Own. 
     */  
    RPointerArray<CLogsExtPresentityIdFetcher> iPresentityIdFetcherArray;

    /**
     * The array that contains the presence fetchers. 
     * Own. 
     */  
    RPointerArray<CLogsExtServiceHandler> iServiceHandlerArray;
    
    /**
     * The icon's bitmap.
     * Own.
     */
    CFbsBitmap* iBitmap;
    
    /**
     * The icon's bitmap mask.
     * Own.
     */
    CFbsBitmap* iBitmask;
    
    };

#endif // C_CLOGSUICONTROLEXTENSION_H
