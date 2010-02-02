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
* Description:    Interacts with the central respository, and the internal data 
*                file to store and restore data.
*
*/

#ifndef C_PDCDATA_H
#define C_PDCDATA_H

// System includes
#include <e32base.h>        // CBase
#include <sysutil.h>        // KSysUtilVersionTextLength

// Forward declarations
class RFs;
class CRepository;
class RReadStream;
class RWriteStream;
class CVPbkContactLinkArray;

/**
 *  Predefined Contacts engine
 *  This class is the reponsible for
 *  deciding if predefined contacts need to be added,
 *  and persisting the ids of any contacts added.
 */
NONSHARABLE_CLASS( CPdcData ): public CBase
    {
public:    
    /**
    * Symbian 1st phase constructor
    * @return Self pointer to CPdcData pushed to
    * the cleanup stack.
    * @param    aFs file system
    * @param    aContactIds ids of any predefined contacts that
    *                       have been added.
    */
    static CPdcData* NewL( RFs& aFs,  CVPbkContactLinkArray& aLinkArray );

    /**
    * Destructor.
    */
    virtual ~CPdcData();

public:

    /**
    * Checks if the predefined contacts need to be added.
    * @return ETrue if the predefined contacts need to be added
    */
    TBool ContactsUpToDateL(); 
    
    /**
    * Stores the most recent info about the firmware etc, plus 
    * the ids of any contacts that have been added.
    */
    void StoreL();

    /**
    *  Gets the location of the vCards and XML file
    *  @return  file location
    */
    HBufC* GetFileLocationL();
    
    /**
     * Gets the buffer of contact links
     * @return  buffer of contact links
     */
    HBufC8* LinkArrayBuffer();

private: // C++ constructor and the Symbian second-phase constructor
    CPdcData( RFs& aFs, CVPbkContactLinkArray& aLinkArray );
    void ConstructL();

private: // private member functions
    void InternalizeL( RReadStream& aStream );
	void ExternalizeL( RWriteStream& aStream ) const;

private: // data
    /// Ref: File system.
    RFs& iFs;
    /// Central repository
    CRepository* iRepository;
    /// Ref: Contacts link array
    CVPbkContactLinkArray& iLinkArray;
    /// Own: Buffer containing list of contact links
    HBufC8* iLinkBuffer;
    };

#endif // C_PDCDATA_H
