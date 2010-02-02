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


#ifndef __CMSCONTACTBASE__
#define __CMSCONTACTBASE__

// INCLUDES
#include <s32std.h>

const TInt KStreamBufferDefaultSizeText             = 256;
const TInt KStreamBufferDefaultSizeBinary           = 2048;

class RCmsContact;

// CLASS DECLARATION
class CCmsContactBase : public CActive
    {
    public:

        /**
    	* Get a pointer to the 16-bit stream buffer
        *
        * @return TAny* Pointer to the stream buffer
    	*/
        TPtr* StreamDesc16();
            
        /**
    	* Get a pointer to the 8-bit stream buffer
        *
        * @return TAny* Pointer to the stream buffer
    	*/
        TPtr8* StreamDesc8();
                
        /**
	    * Check whether this is a binary field
        *
        * @return TBool Binary or not
	    */
		TBool IsBinary() const;

        /**
    	* Activate this object and return its request status
        *
        * @return TRequestStatus& Request status
    	*/
        TRequestStatus& Activate();

		/**
    	* Destructor
    	*/
		~CCmsContactBase();

    protected:
        
        /**
	    * C++ constructor is private
        * 
        * @return CCmsContactFieldInfo
	    */
        CCmsContactBase( RCmsContact& aContact );
        
        /**
	    * C++ constructor is private
        * 
        * @param RCmsContact& Contact handle
        * @param TRequestStatus& Request status of the caller
        * @return CCmsContactFieldInfo
	    */
        CCmsContactBase( RCmsContact& aContact, TRequestStatus& aClientStatus );

        /**
	    * Symbian second-phase constructor
        * 
        * @return void
	    */
        void BaseConstructL();
        
        /**
	    * Reallocate the stream buffer
        * 
        * @param TInt The size of the new buffer
        * @return void
	    */
        void ReAllocBufferL( TInt aBufferSize );
        
        /**
	    * Reallocate the stream buffer
        * 
        * @param TInt The size of the new buffer
        * @return void
	    */
        void HandleError();

    private:
        
        /**
	    * Reallocate the stream buffer
        * 
        * @param TInt The size of the new buffer
        * @return void
	    */
        void DoCancel();

    protected : // data
        
        TPtr                                           iStreamDesc16;
        TPtr8                                          iStreamDesc8;
        TBool                                          iBinary;
        HBufC*                                         iStreamBuffer16;
        HBufC8*                                        iStreamBuffer8;
        RCmsContact&                                   iContact;
        TRequestStatus*                                iClientStatus;
    };

#endif  //__CMSCONTACTFIELD__


// End of File
