/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*    Abstract interface for managing/mapping contacts
*
*/


#ifndef __MXSPCONTACTMANAGER_H__
#define __MXSPCONTACTMANAGER_H__


// INCLUDES
#include <e32base.h>
#include <vwsdef.h>

// CLASS DECLARATION
class CPbk2ViewState;
class MVPbkContactLink;

/**
 *  Abstract interface for managing/mapping contacts
 */
class MxSPContactManager
    {
    public: // mapping functions
    
        /**
         *  Maps xSP contact ID to Phonebook contact ID.
         *
         *  @param aId xSP implementation id for identification
         *  @param aPbkContactLink Phonebook contact link
         *  @param axSPContactId xSP contact ID
         */
        virtual void MapContactL( TUint32 aId, 
                                  const MVPbkContactLink& aPbkContactLink,
                                  TInt32 axSPContactId ) = 0;
                                    
        /**
         *  Unmaps xSP contact ID from Phonebook contact ID.
         *
         *  @param aId xSP implementation id for identification
         *  @param aPbkContactLink Phonebook contact link
         *  @param axSPContactId xSP contact ID
         */
        virtual void UnmapContactL( TUint32 aId, 
                                    const MVPbkContactLink& aPbkContactLink,
                                    TInt32 axSPContactId ) = 0;
                                    
        /**
         *  Gets Phonebook contact IDs which are mapped to
         *  the given xSP contact ID
         *
         *  @param aId xSP implementation id for identification
         *  @param axSPContactId xSP contact ID
         *  @param aPbkContactLinks Placeholder for Phonebook contact links
         */                         
        virtual void GetMappedPbkContactsL( 
                TUint32 aId, 
                TInt32 axSPContactId,
                RPointerArray<MVPbkContactLink>& aPbkContactLinks ) const = 0;
                                    
        /**
         *  Gets xSP contact IDs which are mapped to
         *  the given Phonebook contact ID
         *
         *  @param aId xSP implementation id for identification
         *  @param aPbkContactLink Phonebook contact link
         *  @param axSPContactIds Placeholder for xSP contact IDs
         */                         
        virtual void GetMappedxSPContactsL( 
                TUint32 aId, 
                const MVPbkContactLink& aPbkContactLink,
                RArray<TInt32>& axSPContactIds ) const = 0;
    
    public: // initializer for Phonebook "Names List" related getters
    
        /**
         *  View activation callback. To be called in the beginning of the 
         *  view DoActivateL method
         *
         *  @param aPrevViewId Previous view info
         *  @param aCustomMessageId Custom message ID
         *  @param aCustomMessage Custom message
         */
        virtual void ViewActivatedL( const TVwsViewId& aPrevViewId,
                                     TUid aCustomMessageId,
                                     const TDesC8& aCustomMessage ) = 0;
                                                                                
    public: // Phonebook "Names List" state getter
                                        
        /**
         *  "Names List" view state getter. Ownership is not transferred.
         *
         *  @return Pointer to "Names List" state or NULL
         */
        virtual const CPbk2ViewState* NamesListState() const = 0;
        
    protected: // Protected interface

        /**
         * Destructor.
         */
        virtual ~MxSPContactManager()
                {};
    };


#endif // __MXSPCONTACTMANAGER_H__

// End of File
