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
* Description:    Responsible for the import of contacts from an XML file
*
*/

#ifndef C_PDCXMLIMPORTER_H
#define C_PDCXMLIMPORTER_H

// System includes
#include <e32base.h> // CActive

// User includes
#include "CVPbkContactManager.h"

// Forward declarations
class MVPbkContactStore;
class RFs;
class CPdcXmlContentHandler;
class CVPbkContactLinkArray;

namespace Xml
    {
    class CParser;
    class CMatchData;
    }

// Constants 
// xml data block size;
const TInt KXmlDataBlockSize = 128; // xml data block size; 

/**
 *  This class is responsible for the import of contacts from
 *  an XML file. It owns a content handler which converts 
 *  the contents of the XML file into a contacts item.
 */
NONSHARABLE_CLASS( CPdcXmlImporter ): public CActive
    {
public:
    /**
    * Symbian 1st phase constructor
    * @return Self pointer to CPdcXmlImporter pushed to
    * the cleanup stack.
    * @param    aFs     file system
    * @param    aContactDatabase   contacts store
    * @param    aLinkArray  links to contacts added.
    * @return a CPdcXmlImporter object.
    */
    static CPdcXmlImporter* NewL( RFs& aFs,
                              MVPbkContactStore& aContactStore,
                              CVPbkContactLinkArray& aLinkArray );
    
    /**
    * Destructor.
    */
    virtual ~CPdcXmlImporter();

public:
    /**
    * Symbian 1st phase constructor
    * @return Self pointer to CPdcXmlImporter pushed to
    * the cleanup stack.
    * @param    aFileDirectory    directory containing XML script.
    * @param    aCallerStatus     Caller's iStatus.
    */
    void GetXmlContactsL( const TDesC& aFileDirectory,
                                     TRequestStatus& aCallerStatus  ); 
    
     /**
     * Sets the contactmanager 
     * @param aContactManager   engine's contactmanager
     */                                 
    void SetContactManager(CVPbkContactManager* aContactManager);

private: // C++ constructor and the Symbian second-phase constructor
    CPdcXmlImporter( RFs& aFs, MVPbkContactStore& aContactStore,
                                CVPbkContactLinkArray& aLinkArray );
    void ConstructL();

private: // From CActive
    void DoCancel();
	void RunL();
	TInt RunError( TInt aError );

private: // data
    /// XML file
    RFile iFile;
    /// XML data.
    TBuf8<KXmlDataBlockSize> iXmlData;
    
    /**
     * XML content handler, receives callbacks as XML is parsed.
     * Own
     */
    CPdcXmlContentHandler* iContentHandler;
    
    /**
     *  XML parser
     *  Own
     */
    Xml::CParser* iXmlParser;
    
    /**
    *  XML used by framework to select the correct xml plugin engine
    *  to use.
    *  Own
    */
    Xml::CMatchData* iXmlMatchData;
    /// Ref: Caller's TRequestStatus
    TRequestStatus* iCallerStatus;
    /// Ref: Contacts store 
    MVPbkContactStore& iContactStore;
    /// Ref: File system. 
    RFs& iFs;
    /// Ref: Ids of the contacts that have been added
    CVPbkContactLinkArray& iLinkArray;
    CVPbkContactManager* iContactManager;
    };

#endif //  C_PDCXMLIMPORTER_H
