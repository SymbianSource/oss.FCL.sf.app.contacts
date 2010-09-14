/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*/


//  CLASS HEADER
#include "T_MVPbkContactStoreMain.h"

//  EXTERNAL INCLUDES
#include <CEUnitTestSuite.h>
#include <barsc.h>
#include <barsread.h>
#include <eikenv.h>
#include <bautils.h>

// INTERNAL INCLUDES
#include "T_MVPbkContactStoreSingle.h"
#include "T_MVPbkContactStoreComposite.h"
#include "TVPbkContactStoreUriPtr.h"
#include <t_mvpbkcontactstore.rsg>
#include <VPbkConfig.hrh>

// LOCAL
namespace
    {
#if defined(__WINS__)
    _LIT( KResFile, "z:\\resource\\T_MVPbkContactStore.rsc" );
#else
    _LIT( KResFile, "c:\\resource\\T_MVPbkContactStore.rsc" );
#endif // defined(__WINS__)    
    /**
     * Populates a URI list with URIs from a resource.
     *
     * @param aUriList URI list to populate
     * @param aReader Resource reader to use
     * @param aBuffer Resource buffer to read
     * @param aUriCount URI count in resource
     */
    void PopulateUriListFromResource(
            RArray< TVPbkContactStoreUriPtr >& aUriList,
            TResourceReader& aReader,
            const TDesC8* aBuffer,
            const TInt aUriCount )
        {
        for( TInt i = 0; i < aUriCount; ++i )
            {
            aUriList.Append( aReader.ReadTPtrC16( i, aBuffer ) );
            }
        }

    } // namespace

EXPORT_C MEUnitTest* CreateTestSuiteL()
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    TFileName path;        
    // NearestLanguageFile takes only TFileName
    path.Zero();
    path.Append( KResFile() );

    if ( path.Length() != 0 )
        {
        BaflUtils::NearestLanguageFile( fs, path );
        }
	
    RResourceFile resFile;	
    CleanupClosePushL( resFile );
    resFile.OpenL( fs, path );
    
    TResourceReader reader;
    
    // Read resources for ContactStoreSingle test suite

    HBufC8* singleBuffer = NULL;

#ifdef VPBK_USE_ETEL_TESTSERVER
    singleBuffer = resFile.AllocReadLC( R_SINGLE_STORE_WITH_SIM );
#else
    singleBuffer = resFile.AllocReadLC( R_SINGLE_STORE );
#endif

    reader.SetBuffer( singleBuffer );

    TInt count = reader.ReadInt16();
    RArray< TVPbkContactStoreUriPtr > singleUriList( count );

    PopulateUriListFromResource(
        singleUriList, reader, singleBuffer, count );
    CleanupClosePushL( singleUriList );

    CEUnitTestSuite* mainSuite = T_MVPbkContactStoreMain::NewL();
    CleanupStack::PushL( mainSuite );

    // Loop here all contact stores to the mainSuite
    for( TInt i = 0; i < singleUriList.Count(); ++i )
        {
        mainSuite->AddL(
            T_MVPbkContactStoreSingle::NewLC( singleUriList[ i ] ) );
        CleanupStack::Pop(); // single store test
        }

    resFile.Close();
               
    // NearestLanguageFile takes only TFileName
    path.Zero();
    path.Append( KResFile() );

    if ( path.Length() != 0 )
        {
        BaflUtils::NearestLanguageFile( fs, path );
        }
	
    resFile.OpenL( fs, path );
    

    // Read resources for ContactStoreComposite test suite

    HBufC8* compositeBuffer = 
        resFile.AllocReadLC( R_COMPOSITE_STORES );
    reader.SetBuffer( compositeBuffer );

    count = reader.ReadInt16();
    RArray< TVPbkContactStoreUriPtr > compositeUriList( count );

    PopulateUriListFromResource(
        compositeUriList, reader, compositeBuffer, count );
    CleanupClosePushL( compositeUriList );

    // If only one store, there is no sense to run composite suite
    if( compositeUriList.Count() > 1 )
        {
        // Pass the uri array to the composite suite
        mainSuite->AddL(
            T_MVPbkContactStoreComposite::NewLC( compositeUriList ) );
        CleanupStack::Pop(); // composite store test
        }

    CleanupStack::PopAndDestroy( 2 );  // compositeUriList, compositeBuffer
    CleanupStack::Pop(); // mainsuite
    CleanupStack::PopAndDestroy( 4 ); // singleUriList, singleBuffer, resFile, fs

	return mainSuite;
    }

//  END OF FILE
