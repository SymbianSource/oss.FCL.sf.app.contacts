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
* Description:  Class for handlind the launchpad related data
*
*/


#ifndef __CCAPPCOMMLAUNCHERLPADMODEL_H__
#define __CCAPPCOMMLAUNCHERLPADMODEL_H__

#include <e32base.h>
#include "ccalogger.h"
#include <e32hashtab.h>
#include <spdefinitions.h>
#include <mspnotifychangeobserver.h>
#include <VPbkFieldType.hrh>


_LIT( KPbk2ECEIconFileName, "\\resource\\apps\\phonebook2ece.mif");

class CCCAppCommLauncherContainer;
class CAknIconArray;
class CCoeEnv;
class CFbsBitmap;
class CCCAppCommLauncherPlugin;
class CCCAppCommLauncherPbkCmd;
class CSPSettings;
class CSPNotifyChange;

const TInt KCCAppCommLauncherMaxButtonDataTextLength = 150;

enum TCCAppCommLauncherAddressFields
	{
	EAddressPO,
	EAddressExt,
	EAddressStreet,
	EAddressLocal,
	EAddressRegion,
	EAddressPostcode,
	EAddressCountry
	};

/**
 *  Small utility class for launchpad data storage
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ccaappcommlauncher.dll
 *  @since S60 v5.0
 */
class TCommLauncherButtonData
	{
public:
    TCommLauncherButtonData( VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction ):
        iContactAction( aContactAction ),
        iNumberOfAddresses(0),
        iFlags(0),
        iClipFromBegining(EFalse){};

	enum TFlags
	    {
	    EDefaultSet = 0x01,
	    EHasPresenceIcon = 0x02
	    };

	VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector iContactAction;
	TBuf<KCCAppCommLauncherMaxButtonDataTextLength> iText;	
	TInt iNumberOfAddresses;
	TUint iFlags;
	TBuf<KCCAppCommLauncherMaxButtonDataTextLength> iPopupText;
	TBool iClipFromBegining;
	};

/**
 *  Class for handlind the launchpad related data
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ccaappcommlauncher.dll
 *  @since S60 v5.0
 */
class CCCAppCommLauncherLPadModel : 
    public CBase,
    public MDesCArray,
    public MSPNotifyChangeObserver
{
public:

    /**
     * Two phased constructor.
     */
    static CCCAppCommLauncherLPadModel* NewL( 
        CCCAppCommLauncherContainer& aContainer, CEikListBox& aListBox, CCCAppCommLauncherPlugin& aPlugin );

    /**
     * Destructor.
     */
    ~CCCAppCommLauncherLPadModel();

    /**
     * See more details from MDesCArray.
     *
     * @since S60 v5.0
     */
    TInt MdcaCount() const;
	
protected:

    /**
     * See more details from MDesCArray.
     *
     * @since S60 v5.0
     */
    TPtrC MdcaPoint(TInt aIndex) const;

public: // New

    /**
     * Fills the communication button related data based on
     * CCmsContactFieldInfo. Initial layout is created based
     * on this.
     *
     * @since S60 v5.0
     */
    void FillButtonArrayL();

    /**
     * Loads all the icons needed for listbox.
     *
     * @since S60 v5.0
     */
    void LoadIconArrayL();

    /**
     * Loads one particular icon.
     *
     * @since S60 v5.0
     * @param aBmpId for the icon
     * @param aBmpId for the icon mask
     */
    CGulIcon* LoadIconLC( TInt aBmpId, TInt aMaskId );

    /**
     * Mapping of icons in iButtonIconArray.
     *
     * @since S60 v5.0
     */    
    enum TIconIndex
        {
        ECallIconIndex,
        EMsgIconIndex,
        EEmailIconIndex,
        EVoipIconIndex,
        EInstMsgIconIndex,
        EUrlIconIndex,
        EVideocallIconIndex,
        EAddressValIconIndex,
        EAddressNotValIconIndex,
        EMultiIconIndex,
        EPresenceIconIndex
        };

    /**
     * Getter for icon array.
     *
     * @since S60 v5.0
     * @return CAknIconArray containing the icons
     */
    CAknIconArray* IconArray();

    /**
     * Getter for TCommLauncherButtonData
     *
     * @since S60 v5.0
     * @param Index of wanted buttondata
     * @return TCommLauncherButtonData
     */
    TCommLauncherButtonData& ButtonData( TInt aIndex );

    /**
     * Similar as in MCCAppContactHandlerNotifier, but this is
     * forwarded by container-class.
     *
     * @since S60 v5.0
     * @param aContactField
     */
    void ContactFieldFetchedNotifyL( 
        const CCmsContactField& aContactField );    

    /**
     * Getter for popup-texts
     *
     * @since S60 v5.0
     * @param aButtonIndex
     * @return descriptor containing text to be shown
     */
    TPtrC TextForPopUpL( TInt aButtonIndex );
    
    /**
     * Returns address information for popup-texts
     *
     * @param aButtonIndex
     * @return descriptor containing text to be shown
     */
    TPtrC AddressTextForPopUpL();

    /**
     * Resets the model, does not unload icons
     *
     * @since S60 v5.0
     */
    void Reset();
    
    /**
     * Notifies of presence change
     * 
     * @param aContactField
     */
    void ContactPresenceChangedL( const CCmsContactField& aContactField );

    /**
     * Updates information if addresses are validated
     *
     * @param aContactFieldInfo 
     */    
    void UpdateAddressesValidationL( const CCmsContactFieldInfo& aContactFieldInfo );
    
    
    /**
     * Handle resource change
     *
     */ 
    void ResourceChangedL();
   

private: // New

    /**
     * Maps communication methods to icons in launchpad-listbox.
     *
     * @since S60 v5.0
     * @param aContactAction communication method
     * @return icon array index of the icon 
     */
    TInt MapCommMethodToIcon( 
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction ) const;

    /**
     * Updates button's popup text
     *
     * @since S60 v5.0
     * @param aButtonIndex
     * @param aContactAction communication method
     * @param aContactField method
     */
    void CheckPopupTextL(
        const TInt aButtonIndex,
        const VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction, 
        const CCmsContactField& aContactField );

    /**
     * Getter for button label text
     *
     * @since S60 v5.0
     * @param aContactAction communication method
     * @param aText descriptor the text will be copied to, needs size of at least KCCAppCommLauncherMaxButtonDataTextLength
     */    
    void ButtonTextL( 
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction, 
        TDes& aText );

    /**
     * Updates addresses button popup text
     *
     * @since S60 v5.0
     * @param aButtonIndex
     * @param aContactAction communication method
     * @param aContactField method
     */
    void CheckAddressesPopupTextL(
        const TInt aButtonIndex,
        const VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction, 
        const CCmsContactField& aContactField );

private:

    /**
     * Private constructor
     */    
    CCCAppCommLauncherLPadModel( CCCAppCommLauncherContainer& aContainer,
    		CEikListBox& aListBox, 
            CCCAppCommLauncherPlugin& aPlugin );

    /**
     * ConstructL
     */    
    void ConstructL();        
    
    /**
     * Finds the number of services that support internet call 
     * by scanning all the services in the SPSettings Table.
     * @param aServiceId - Stores the last matched service id    
     * @return - Total Number of services on which Internet call is possible
     */ 
    TInt GetSupportedVOIPServicesL( TServiceId& aServiceId );    
    
    /**
     * Loads the Branded VOIP icon & Localised ServiceName
     * from PBK2 service manager interface     
     * @param aServiceId - Service Id of the Service that supports InternetCall
     * @param aBitmap - Contains the Retreived BrandIcon Bitmap. Ownership
     *           is passed to the caller
     * @param aMask - Contains the Retreived BrandIcon Bitmap MAsk. Ownership
     *           is passed to the caller
     * @param aLocalisedServiceName - Contains the localised service name     
     * @return None
     */
    void LoadVoipButtonInfoFromPbkL( 
                TServiceId aServiceId,
                CFbsBitmap*& aBitmap, CFbsBitmap*& aMask, HBufC*& aLocalisedServiceName );
    
    /**
     * Handles SPSettings Changes
     */            
	void DoHandleNotifyChangeL() ;
	
    /**
     * Loads the VOIP Button Icon & Label 
     * Usecase : If we have only one voip service, the voip(Internet Call)
     * button should have the Branded Icon of that Service and the label
     * must be "ServiceName" appended with "Call".
     * eg : If we have a service named SKYPE installed in the Phone
     * and if SKYPE supports VOIP, then the VOIP Button Icon should be
     * the Branded Icon of SKYPE and the Button Label should be
     * "SKYPE CALL". 
     * @return TInt - Stores the Info regd whether Image/Text was set for the
     *          VOIP Buttton or not. We use KVOIPButtonImageSet && with the returnval
     *          to know whether Image has been set or not
     *          Will be used in   HandleNotifyChange
     */
    TInt LoadVoipButtonInfoL();
    
    /*
     * Calculate the layout size
     */
    void CalculateLayoutSize();
    
    
private:
    //From MSPNotifyChangeObserver
    void HandleNotifyChange( TServiceId aServiceId );
    void HandleError( TInt aError );
    
private: 
	
	/* Check if multi icon should be show
	 * @param aButtonIndex - The button index
	 * @return - EFalse for not show, ETrue for show the icon
	 */
    TBool IfShowMultiIcon(TInt aButtonIndex) const;
    
   
    /* Set clip direction for label text
     * @param aContactAction: The action type
     * @param aCmsFieldType: Cms field type
     * @return - none
     */
    void SetTextClipDirection( 
    		VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction,
    		TInt aButtonIndex
    		/*CCmsContactFieldItem::TCmsContactField aCmsFieldType*/ );
    
    /* Clip the text from begining
     * @param aBuffer
     * @param aItemIndex
     * @param aSubCellNumber
     * @return - the result of clipping
     */
    TBool ClipFromBeginning(
        TDes& aBuffer,
        TInt aItemIndex,
        TInt aSubCellNumber) const;

    /*
     * In Arabic variant the displaying of numbers in phonebook should follow 
     * the Arabic/Latin number setting that is found in General Settings > 
     * Phone > Language, So the second row text must be digits.
     * @param aContactField method
     */
    TBool IsPhoneNumber( const CCmsContactField& iContactField );

private: // Data

    /**
     * Array of button data
     * Own.
     */
    RArray<TCommLauncherButtonData> iButtonDataArray;
    /**
     * Array of icons used in listbox
     * Own.
     */
    CAknIconArray* iButtonIconArray;
    /**
     * For storing the texts
     * Own.
     */   
    HBufC* iTempText;
    /**
     * Ref to container class
     * Not own.
     */   
    CCCAppCommLauncherContainer& iContainer;
    /**
     * Addresses validation status
     * Own.
     */   
    TBool iAddressesValidated;
    /**
     * For storing the address text
     * Own.
     */   
    RHashMap<TInt, TPtrC> iAddressFields;

    /**
     * Launching performance logger
     * Will be removed (hopefully) when launching performance is sufficient.
     */        
    TBool iPerfLauncherCalled;

    /**
     * Environment.
     * Not Own.
     */
    CCoeEnv& iCoeEnv;
    
    /*
     * Reference to list box control
     */
    CEikListBox& iListBox;

    /**
     * Ref to plugin.
     * Not own.
     */
    CCCAppCommLauncherPlugin& iPlugin;
    
    /// Own: CCCAppCommLauncherPbkCmd
    // used to get the instance of the Phonebook 2 application services.
    CCCAppCommLauncherPbkCmd* iPbkCmd;

    //Owns - Instance to spSettings    
    CSPSettings* iSettings;
    
    //Owns - Service provider settings change notifer
    CSPNotifyChange* iSPNotifyChange;
    
	// Owns - Text buffer
    HBufC* iTextBuf; 
    
	// Size for service icon
    TSize iServiceIconSize;
    
	// Size for presence icon
    TSize iPresenceIconSize;
        
    inline void RunLaunchLogger()
        {
        #ifdef PERF_LAUNCH
        if(!iPerfLauncherCalled)
            {
            WriteToPerfLaunchLog(_L(" Ready"));
            iPerfLauncherCalled = ETrue; 
            }
        #endif // PERF_LAUNCH
        }
};


    
#endif // __CCAPPCOMMLAUNCHERLPADMODEL_H__
//End of file
