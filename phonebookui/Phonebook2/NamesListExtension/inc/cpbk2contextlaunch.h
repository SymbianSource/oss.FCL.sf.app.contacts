/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 NamesList context launcher
*
*/

#ifndef CPBK2CONTEXTLAUNCH_H
#define CPBK2CONTEXTLAUNCH_H

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION
class MCCAConnection;

/**
 * Context launcher class that handles custom view activation messages.
 * Custom message can include e.g. launch request for CCA application.
 * 
 * CONTEXT LAUNCHER MESSAGE STREAM FORMAT
 *
 *  - Context launcher custom message UID is KPbk2ContextLaunchCustomMessageUID
 *
 *  - Format of the stream in (slightly freeform) EBNF:
 *
 *  stream  ::= version , command ;
 *  version ::= Int32(1) ;
 *  command ::= Int32(ECmdLaunchMyCard) , Int32(viewuid) | Int32(0) ;
 *  command ::= Int32(ECmdLaunchContactCard) , Int32(viewuid) | Int32(0) , contactlink ;
 *  contactlink ::= Int32(length) , MVPbkContactLink(link) ;
 *
 *  Constants:
 *      ECmdLaunchMyCard           = 1  // Launch CCA's MyCard view
 *      ECmdLaunchContactCard      = 2  // Launch CCA view for spesific contact 
 *      
 *      
 * Notes:     
 *  If viewuid parameter is set to 0, then the default view will be launched
 *
 * Example:
 *  Activate Contact card view 0x01010101 into Phonebook2's context. This
 *  example assumes there is a contactLink variable of type MVPbkContactLink.
 *
 *  // Write parameters in a buffer
 *  TBuf8<256> param;
 *  RDesWriteStream stream( param );
 *  stream.PushL();
 *  stream.WriteInt32L( 1 );               // version number
 *  stream.WriteInt32L( 2 );               // opcode ECmdLaunchContactCard
 *  stream.WriteInt32L( 0x01010101 );      // view uid 0x01010101
 *  HBufC8* buf = contactLink->PackLC();   // pack the contact link
 *  stream.WriteInt32L( buf->Length() );   // write length
 *  stream.WriteL( *buf );                 // write the packed link
 *  CleanupStack::PopAndDestroy();         // cleanup buf
 *  stream.CommitL();
 *  CleanupStack::PopAndDestroy();         // cleanup stream
 *
 */
NONSHARABLE_CLASS( CPbk2ContextLaunch )  : 
    public CActive 
    {
    public: // Constructors and destructor
        
        /**
         * Constructor
         */
        CPbk2ContextLaunch( MCCAConnection*& aCCAConnection );
        
        /**
         * Destructor
         */
        ~CPbk2ContextLaunch();
        
        
    public: // Interface

        /**
         * Context commands
         */
        enum TCommands
            {
            /// Command for opening mycard  
            ECmdLaunchMyCard       = 1,
            /// Command for opening contect card  
            ECmdLaunchContactCard  = 2
            };
        
        /**
         * Returns the message uid for use with view server messages.
         *
         * @return View activation custom message uid.
         */
        static TUid Uid();

        /**
         * Handle context launch request. See header for request format.
         * 
         * @param aMessage streamed context launch request. 
         */
        void HandleMessageL( const TDesC8& aMessage );
    
        
    public: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError( TInt aError );

        
    private: // new
        
        /**
         * Launch CCA application for contact
         * 
         * @param aPackedLink packed contact link
         * @param aViewUid view to launch. If KNullUid, then default view is launched
         */
        void LaunchCCAL( const TDesC8& aPackedLink, TUid aViewUid );

        /**
         * Launch CCA's application with MyCard
         * 
         * @param aViewUid view to launch. If KNullUid, then default view is launched
         */
        void LaunchMyCardL( TUid aViewUid );
        
        
    private: // data
        /// Not own. CCA Connection instance
        MCCAConnection*& iCCAConnection;
        /// Own. Handled message
        HBufC8* iMessage; 
    };

#endif // CPBK2CONTEXTLAUNCH_H

// End of File
