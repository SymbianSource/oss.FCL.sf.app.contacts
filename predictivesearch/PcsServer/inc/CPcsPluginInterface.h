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
*/

#ifndef CPCS_PLUGIN_INTERFACE_H
#define CPCS_PLUGIN_INTERFACE_H

// INCLUDE FILES
#include <s32mem.h>
#include <coemain.h>

#include <CPcsDefs.h>
#include "CPsClientData.h"
#include "CPsQuery.h"
#include "CPsSettings.h"
#include "CPsPattern.h"

// FORWARD DECLARATION	
class CPcsPlugin;

/**
* Class is used to instantiate the required plugins when the search has been
* asked. Acts as interface to all Plugins.
*/

class CPcsPluginInterface: public CBase 
{
	public:

		/** 
		* Symbian OS 2 phased constructor.
		* @return	A pointer to the created instance of CPcsPluginInterface.
		*/
		static CPcsPluginInterface* NewL();

		/** 
		* Symbian OS 2 phased constructor.
		* @return	A pointer to the created instance of CPcsPluginInterface.
		*/
		static CPcsPluginInterface* NewLC();

		/**
		* Destructor.
		*/      
		virtual ~CPcsPluginInterface();

		/**
		* Instantiates the required search plug-ins known by the ecom framework.
		* basically this function first calls ListAllImplementationsL()
		* and then checks them one by one to match the algorithm name
		*/
		void InstantiateAlgorithmL(TDes& aAlgorithm);

		/**
		* Unloads all plugins
		* This should internally call  REComSession::DestroyedImplementation( iDtor_ID_Key )
		* and then REComSession::FinalClose()
		*/
		void UnloadPlugIns(); 

		/**
		* Lists all implementations which satisfy this ecom interface
		*
		* @param aImplInfoArray On return, contains the list of available implementations
		* 
		*/   
		inline void ListAllImplementationsL( RImplInfoPtrArray& aImplInfoArray )
		{
			REComSession::ListImplementationsL( KPcsPluginInterfaceUid, aImplInfoArray );
		}

		/**
		* Forwards the search request to the algorithm instance
		*/
		void  PerformSearchL(CPsSettings& aSettings,
				             CPsQuery& aCondition,
				             RPointerArray<CPsClientData>& aPsData,
				             RPointerArray<CPsPattern>& aPattern);
		/**
		* Forwards the search request to the algorithm instance
		*/
		void  SearchInputL(CPsQuery& aSearchQuery,
				           TDesC& aSearchData,
				           RPointerArray<TDesC>& aMatchSet,
				           RArray<TPsMatchLocation>& aMatchLocation); 

        /**
        * Forwards the search request to the algorithm instance
        */
        void  SearchMatchStringL(CPsQuery& aSearchQuery,
                           TDesC& aSearchData,
                           TDes& aMatch ); 		
		
		/**
		* Forwards the request to check if language is supported
		* to the algorithm instance
		*/
		TBool IsLanguageSupportedL(const TUint32 aLanguage);   
		              
		/**
		* Forwards the request to recover data fields order for a URI
		* to the algorithm instance
		*/
		void GetDataOrderL(TDesC& aURI, RArray<TInt>& aDataOrder);

		/**
		* Forwards the request to recover sort order for a URI
		* to the algorithm instance
		*/
		void GetSortOrderL(TDesC& aURI, RArray<TInt>& aSortOrder);  

		/**
		* Forwards the request to set sort order for a URI
		* to the algorithm instance
		*/
		void ChangeSortOrderL(TDesC& aURI, RArray<TInt>& aSortOrder);

        /**
        * Forwards the request to get the Adaptive Grid for one or more URI
        * to the algorithm instance
        */
        void GetAdaptiveGridL( const MDesCArray& aURIs,
                               const TBool aCompanyName,
                               TDes& aAdaptiveGrid );

		/** 
		* Returns the UID of the algorithm in use
		*/
		TUid AlgorithmInUse() 
		{
			return iAlgorithmInUse;
		}

	private: 

		/** 
		* Performs the first phase of two phase construction.
		*/
		CPcsPluginInterface();  

		/** 
		* Symbian OS 2 phased constructor.
		*/
		void ConstructL();

		/**
		* Instantiates a PCS plug-in, knowing the implementation uid.
		* @param aImpUid imp uID
		*/
		CPcsPlugin* InstantiatePlugInFromImpUidL( const TUid& aImpUid );


	private: 

		TUid iDtor_ID_Key;

		// List of plugins that this interface will interact with
		RPointerArray<CPcsPlugin>  iPcsPluginInstances; 

		// Uid of the algorithm in use
		TUid iAlgorithmInUse;
};

#endif // CPCS_PLUGIN_INTERFACE_H

//End of File
