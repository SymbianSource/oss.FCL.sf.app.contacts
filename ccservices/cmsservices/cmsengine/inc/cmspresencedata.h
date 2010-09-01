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
* Description: 
*       
*
*/


#ifndef __CMSPRESENCEDATA__
#define __CMSPRESENCEDATA__

// INCLUDES
#include <s32std.h>
#include "cmscontactbase.h"
#include "cmscontactfield.h"
#include "cmscontactfielditem.h"

enum TCmsIconData
    {
    ECmsIcon = 0,
    ECmsIconMask    
    };

_LIT8( KDefaultVoipApplicationId,                   "xsp" );

class CFbsBitmap;
class CBSFactory;
class RCmsContact;
class CCmsContactFieldItem;
class MBSAccess;
class TSize;

// CLASS DECLARATION
class CCmsPresenceData : public CCmsContactFieldItem
    {
    public:
        
        /**
    	* Get the contact field data.
        * 
        * @return TPtrC contact field data. May be zero length especially when public traffic light
    	*/
        IMPORT_C TPtrC8 UserIdentity() const;
        
        /**
    	* Get the mask of the presence icon
        * 
        * @return mask of the presence icon
    	*/
        IMPORT_C CFbsBitmap* Mask() const;
        
        /**
    	* Get the bitmap of the presence icon
        * 
        * @return bitmap of the presence icon
    	*/
        IMPORT_C CFbsBitmap* Bitmap() const;

        /**
    	* Get branded status text
        * 
        * @return branded status text
    	*/
        IMPORT_C TPtrC BrandedText() const;

        /**
    	* Get the type of this presence
        * 
        * @return TUint32. Refer to CCmsContactFieldItem::TCmsContactNotification.
    	*/
        IMPORT_C TUint32 ServiceType() const;

        /**
    	* Get the type of this presence
        * 
        * @param aBitmapSize size of the returned bitmap
        * @return error code
    	*/
        IMPORT_C TInt PreparePresenceDataL( TSize aBitmapSize );
        
		/**
		* Destructor
	    */
		IMPORT_C virtual ~CCmsPresenceData();
    
    public:
	    
        /**
    	* Symbian constructor
        *
        * @param RCmsContact& Handle to the client-side contact
        * @return CCmsPresenceIcon* New instance
    	*/
		static CCmsPresenceData* NewL( RCmsContact& aContact );

        /**
    	* Append a data field
        *
        * @param const TDesC8& Data descriptor
        * @return CCmsPresenceData::TPresenceDataField Type of the data
    	*/
        void AssignDataL( TInt aFieldType, const TDesC8& aFieldData );
        
        /**
    	* Append a data field
        *
        * @param const TDesC8& Data descriptor
        * @return CCmsPresenceData::TPresenceDataField Type of the data
    	*/
        CFbsBitmap* CopyBitmapL( CFbsBitmap& aBitmap );

    private:
        
        /**
    	* C++ constructor is private
        *
        * @param RCmsContact& Handle to the client-side contact
        * @return CCmsPresenceIcon
    	*/
        CCmsPresenceData( RCmsContact& aContact );

        /**
	    * Symbian OS second-phase constructor
        *
        * @return void
	    */
		void ConstructL();
        
        /**
	    * Symbian OS second-phase constructor
        *
        * @return void
	    */
        TPtrC8 ElementIdImage();
        
        /**
	    * Symbian OS second-phase constructor
        *
        * @return void
	    */
        CBSFactory* CreateBrandingFactoryL();
        
        /**
        * Retreives the icon from the presencetrafficlights.mif
        * @param aBitmapSize - Bitmap Size
        */
        TInt DoGetIconFromFileL( TSize aBitmapSize );            
        
        /**
        * Retreives the icon from the Branding Server
        * @param aFactory - Branding Server Factory object
        * @param aBitmapSize - Bitmap Size
        * @param aLanguageId - Language for which Brand is requested
        * @return ErrCode        
        */
        TInt DoGetIconFromBrandSrvL( CBSFactory& aFactory, TSize aBitmapSize, TLanguage aLanguage );      

        /**
        * Retreives the icon from the Branding Server
        * @param aFactory - Branding Server Factory object
        * @param aBitmapSize - Bitmap Size
        * @return ErrCode        
        */
        TInt DoGetIconFromBrandSrvL( CBSFactory& aFactory, TSize aBitmapSize );
    		
    private:  //Data
        
        TInt                            iLanguageId;
        HBufC*                          iBrandedText;
        HBufC8*                         iUserIdentity;
        HBufC8*                         iBrandId;
        HBufC8*                         iElementIdText;
        HBufC8*                         iElementIdImage;
        CFbsBitmap*                     iMask;
        CFbsBitmap*                     iBitmap;
        RCmsContact&                    iContact;
        TInt                            iServiceType;
    };

#endif  //__CMSPRESENCEDATA__


// End of File
