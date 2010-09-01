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
* Description:    Responsible for the import of contacts from vCards.
*
*/

#ifndef C_PDCVCARDIMPORTER_H
#define C_PDCVCARDIMPORTER_H

// System includes
#include <e32base.h>                  // CActive
#include <MVPbkContactCopyObserver.h> // MVPbkContactCopyObserver

// Forward declarations
class RFs;
class MVPbkContactStore;
class CVPbkVCardEng;
class CVPbkContactLinkArray;
class MVPbkContactLinkArray;

/**
 *  This class is responsible for the import of contacts from
 *  vCards. 
 */
NONSHARABLE_CLASS( CPdcVCardImporter ): public CActive,
    public MVPbkContactCopyObserver
    {
public:    
    /**
    * Symbian 1st phase constructor
    * @return Self pointer to CPdcXmlContentHandler
    * @param    aFs             file server
    * @param    aContactManager   contacts manager
    * @param    aContactStore   contacts store
    * @param    aLinkArray      Links to the  contacts added.
    * @return  a CPdcVCardImporter object
    */
    static CPdcVCardImporter* NewL( RFs& aFs,
                                      CVPbkContactManager& aContactManager,
                                      MVPbkContactStore& aContactStore,
                                      CVPbkContactLinkArray& aLinkArray );

    /**
    * Destructor.
    */
    virtual ~CPdcVCardImporter();

public:
    /**
    * Asyncronously imports all the vCards in the given directory.
    * @param    aFileDirectory    directory containg vCards.
    * @param    aCallerStatus   caller's TRequestStatus
    */
    void GetVCardsL( const TDesC& aFileDirectory,
                                     TRequestStatus& aCallerStatus  );

private: // C++ constructor and the Symbian second-phase constructor

    CPdcVCardImporter( RFs& aFs, MVPbkContactStore& aContactStore,
                                CVPbkContactLinkArray& aLinkArray );
    void ConstructL( CVPbkContactManager& aContactManager );

private:
    void SelfComplete();
    void ImportContactsL(const TDesC& aFileName);
    void AddToLinkArrayL( const MVPbkContactLinkArray& aResults);

private: // From CActive
    void DoCancel();
	void RunL();
	TInt RunError( TInt aError );
	
private: // From MVPbkContactCopyObserver
    void ContactsSaved( MVPbkContactOperationBase& aOperation,
            MVPbkContactLinkArray* aResults );
            
   void ContactsSavingFailed( 
                MVPbkContactOperationBase& aOperation, TInt aError );

private: // data
    /// Ref: Contacts store
    MVPbkContactStore& iContactStore;
    /// Own: VCard import engine
    CVPbkVCardEng* iVCardEngine;
    /// Ref: File server
    RFs& iFs;
    /// Own: List of vCards in directory
    CDir* iVCardList;
    /// Index of current vCard
    TInt iVCardIndex;
    /// Ref: Caller's TRequestStatus
    TRequestStatus* iCallerStatus;
    /// Own: Directory containing vCards
    HBufC* iFileDirectory;
    /// Ref: Ids of the contacts that have been added
    CVPbkContactLinkArray& iLinkArray;
    /// Own: Import operation
    MVPbkContactOperationBase* iImportOperation;
    };

#endif // C_PDCVCARDIMPORTER_H
