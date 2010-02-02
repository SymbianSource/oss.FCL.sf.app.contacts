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


#ifndef __CMSCONTACTFIELDINFO__
#define __CMSCONTACTFIELDINFO__

// INCLUDES
#include <s32std.h>
#include "cmscontactbase.h"
#include "cmscontactfielditem.h"

// CLASS DECLARATION
class CCmsContactFieldInfo : public CCmsContactBase
    {
    public :

        /**
    	* Get the enabled contact fields
        *
        * @return RArray<CCmsContactFieldItem::TCmsContactField> Array of field IDs
    	*/
		IMPORT_C const RArray<CCmsContactFieldItem::TCmsContactField>& Fields() const;

		/**
    	* Destructor
    	*/
		IMPORT_C ~CCmsContactFieldInfo();

    public:
        
        /**
    	* Symbian constructor
        *
        * @param RCmsContact& Handle to the client-side contact 
        * @return CCmsContactFieldInfo* New class instance
    	*/
        static CCmsContactFieldInfo* NewL( RCmsContact& aContact );

    	/**
    	* Symbian constructor
        *
        * @param RCmsContact& Handle to the client-side contact
        * @param TRequestStatus& Request status of the client
        * @return CCmsContactFieldInfo* New class instance
    	*/
		static CCmsContactFieldInfo* NewL( RCmsContact& aContact,
                                           TRequestStatus& aClientStatus );
    
    private:
        
        /**
	    * C++ constructor is private
        * 
        * @return CCmsContactFieldInfo
	    */
        CCmsContactFieldInfo( RCmsContact& aContact );
        
        /**
	    * C++ constructor is private
        * 
        * @param RCmsContact& Handle to the client-side contact
        * @param TRequestStatus& Request status of the caller
        * @return CCmsContactFieldInfo
	    */
        CCmsContactFieldInfo( RCmsContact& aContact, 
                              TRequestStatus& aClientStatus );

        /**
	    * Symbian OS second-phase constructor
        *
        * @return void
	    */
		void ConstructL();

    private:  //From CActive

        /**
	    * Asynchronous operation completes
        *
        * @return void
	    */
		void RunL();

    private : // data
        
        TInt                                           iFieldCount;
        RArray<CCmsContactFieldItem::TCmsContactField> iEnabledFields;
    };

#endif  //__CMSCONTACTFIELD__


// End of File
