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


#ifndef __CMSCONTACTFIELD__
#define __CMSCONTACTFIELD__

// INCLUDES
#include <s32std.h>
#include "cmscontactbase.h"
#include "cmscontactfielditem.h"

enum TCmsVoIPSupport
    {
    ECmsVoIPSupportBasic    = 1,
    ECmsVoIPSupportXspId    = 2,
    ECmsVoIPSupportCallout  = 4,
	ECmsVoIPSupportSip = 8
    };

class RCmsContact;
class CCmsContactFieldItem;

// CLASS DECLARATION
class CCmsContactField : public CCmsContactBase
    {
    public :
    	
        /**
    	* Get the number of items in this field
        * 
        * @return TInt Number of items
    	*/
		IMPORT_C TInt ItemCount() const;
        
        /**
    	* Get the type of this field
        * 
        * @return CCmsContactFieldItem::TCmsContactField Field type
    	*/
        IMPORT_C CCmsContactFieldItem::TCmsContactField Type() const;

        /**
    	* Get the type of the group this field belongs to
        * 
        * @return CCmsContactFieldItem::TCmsContactFieldGroup Group type
    	*/
        IMPORT_C CCmsContactFieldItem::TCmsContactFieldGroup GroupType() const;

        /**
    	* Get the indexed item
        * 
        * @param TInt Item index
        * @return const CCmsContactFieldItem& Field item
    	*/
		IMPORT_C const CCmsContactFieldItem& ItemL( TInt aIndex ) const;

        /** 
        * Get the field item of given default attribute type.
        * Leaves with KErrNotFound if nothing found.
        *
        * @see TCmsDefaultAttributeTypes enum from CCmsContactFieldItem
        * @param TCmsDefaultAttributeTypes default type
        * @return const CCmsContactFieldItem& Field item
        */        
        IMPORT_C const CCmsContactFieldItem& ItemL( CCmsContactFieldItem::TCmsDefaultAttributeTypes aDefaultType ) const;            

        /**
    	* Get all the items
        * 
        * @return const RPointerArray<CCmsContactFieldItem>& Item array
    	*/
		IMPORT_C const RPointerArray<CCmsContactFieldItem>& Items() const;

        /** 
        * For asking if field has default attributes.
        *
        * @see TCmsDefaultAttributeTypes enum from CCmsContactFieldItem
        * @param TCmsDefaultAttributeTypes default type
        * @return TBool
        */        
        IMPORT_C TBool HasDefaultAttribute( CCmsContactFieldItem::TCmsDefaultAttributeTypes aDefaultType ) const;            

        /** 
        * Go through all field items and returns bitmasks of default attributes.
        *
        * @see TCmsDefaultAttributeTypes enum from CCmsContactFieldItem
        * @return TInt: TCmsDefaultAttributeTypes bitmask
        */        
        IMPORT_C TInt HasDefaultAttribute() const;            
        
    public:
	    
        /**
    	* Symbian constructor
        *
        * @param RCmsContact& Handle to the client-side contact
        * @return CCmsContactField* New instance
    	*/
		static CCmsContactField* NewL( RCmsContact& aContact,
                                       CCmsContactFieldItem::TCmsContactField aFieldType );

        /**
    	* Symbian constructor
        *
        * @param RCmsContact& Handle to the client-side contact
        * @param TRequestStatus& Request status of the caller
        * @return CCmsContactField* New instance
    	*/
		static CCmsContactField* NewL( RCmsContact& aContact,
                                       TRequestStatus& aClientStatus,
                                       CCmsContactFieldItem::TCmsContactField aFieldType );

        /**
    	* Symbian constructor
        *
        * @param RCmsContact& Handle to the client-side contact
        * @param TRequestStatus& Request status of the caller
        * @param CCmsContactFieldItem::TCmsContactFieldGroup Field group
        * @return CCmsContactField* New instance
    	*/
		static CCmsContactField* NewL( RCmsContact& aContact,
                                       TRequestStatus& aClientStatus,
                                       CCmsContactFieldItem::TCmsContactFieldGroup aFieldGroup );

        /**
	    * Create a new field item 
        * 
        * @param const TDesC& The data for the item
        * @param const TDesC& The infofor the item        
        * @param const TInt Default attribute value   
        * @return void
	    */
        void CreateFieldItemL( const TDesC& aItemData, const TDesC& aItemInfo, const TInt aDefaultAttribute = 0 );
    
		/**
		* Destructor
	    */
		~CCmsContactField();

    private:
        
        /**
	    * C++ constructor is private
        *
        * @return CCmsContactField
	    */
        CCmsContactField( RCmsContact& aContact,
                          CCmsContactFieldItem::TCmsContactField aFieldType );
        
        /**
	    * C++ constructor is private
        *
        * @param RCmsContact& Handle to the client-side contact
        * @param TRequestStatus& Request status of the caller
        * @return CCmsContactField
	    */
        CCmsContactField( RCmsContact& aContact,
                          TRequestStatus& aClientStatus,
                          CCmsContactFieldItem::TCmsContactFieldGroup aFieldGroupp );
        

        /**
	    * C++ constructor is private
        *
        * @param RCmsContact& Handle to the client-side contact
        * @param TRequestStatus& Request status of the caller
        * @return CCmsContactField
	    */
        CCmsContactField( RCmsContact& aContact,
                          TRequestStatus& aClientStatus,
                          CCmsContactFieldItem::TCmsContactField aFieldType );

        /**
	    * Symbian OS second-phase constructor
	    */
		void ConstructL();
        
        /**
	    * Select the type of the field
        * 
        * @return void
	    */
        void SelectFieldType();
        
        /**
	    * Handle textual data   
        * 
        * @return void
	    */
        void HandleSixteenBitDataL();
        
        /**
	    * Handle binary data   
        * 
        * @return void
	    */
        void HandleBinaryDataL();
        
        /**
	    * Handle basic binary data   
        * 
        * @return void
	    */
        void HandleBasicDataL();
        
        /**
	    * Handle presence data   
        * 
        * @return void
	    */
        void HandlePresenceDataL();
        
        /**
	    * Handle presence data   
        * 
        * @return void
	    */
        TInt ExtractFieldType( TPtr8& aDataDesc );
        
        /**
	    * Handle presence data   
        * 
        * @return void
	    */
        TPtrC8 ExtractData( const TDesC8& aDataDesc, TInt& aDelIndex );
        
        /**
	    * Handle presence data   
        * 
        * @return void
	    */
        void HandlePresencePackageL( const TDesC8& aPresenceData );

        /**
	    * Handle binary data
        * 
        * @return void
	    */
        void CreateFieldItemL( const TDesC8& aItemData );

   private:  //From CActive
        
        /**
	    * Complete an asynchronous operation
	    */
        void RunL();

    private : // data
        
		RPointerArray<CCmsContactFieldItem>             iFieldItems;
        CCmsContactFieldItem::TCmsContactField          iFieldType;
        CCmsContactFieldItem::TCmsContactFieldGroup     iFieldGroup;
    };

#endif  //__CMSCONTACTFIELD__


// End of File
