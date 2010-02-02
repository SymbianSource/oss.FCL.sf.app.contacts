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
* Description:  ECom search interface definition
*
*/


#ifndef CPCS_PLUGIN_H
#define CPCS_PLUGIN_H

//  INCLUDES
#include <e32base.h>
#include <s32mem.h>
#include <ecom/ecom.h>
#include <CPsData.h>
#include <CPsClientData.h>
#include <CPsQuery.h>
#include <CPsSettings.h>
#include <CPsPattern.h>

/**
* Search plug-ins (Ecom) interface definition.
*/
class CPcsPlugin: public CBase
{

	public: 

		/**
		* Ecom interface static factory method implementation.
		* @param aImpUid Ecom's implementation uid
		* @return A pointer to the created instance of CSearchPlugin
		*/
		static inline CPcsPlugin* NewL( TUid aImpUid );

		/**
		* Ecom interface static factory method implementation.
		* @param aImpUid Ecom's implementation uid
		* @return A pointer to the created instance of CSearchPlugin
		*/
		static inline CPcsPlugin* NewLC( TUid aImpUid );

		/**
		* Destructor
		*/
		virtual ~CPcsPlugin();

	public: 

		/**
		* Gets the plugin id.
		* @return Id of the plugin.
		*/   
		inline TUid PluginId() const;

		/**
		* Performs search
		* Implementation needs to be provided by the algorithm classes.
		* Search results are packed in the stream.
		*/
		virtual void  PerformSearchL( const CPsSettings& aPcsSettings,
				                      CPsQuery& aCondition,
				                      RPointerArray<CPsClientData>& aData,
				                      RPointerArray<CPsPattern>& aPattern) = 0;				                      	                                                                          

		/**
		* Performs search on a input string
		* Implementation needs to be provided by the algorithm classes 
		*/
		virtual void  SearchInputL( CPsQuery& aSearchQuery,
			                        TDesC& aSearchData,
			                        RPointerArray<TDesC>& aMatchSet,
			                        RArray<TPsMatchLocation>& aMatchLocation ) = 0;

        /**
        * Performs search on a input string, and return result also as a string
        * Implementation needs to be provided by the algorithm classes 
        */
        virtual void  SearchMatchStringL( CPsQuery& aSearchQuery,
                                    TDesC& aSearchData,
                                    TDes& aMatch ) = 0;		
		
		/**
		* Returns ETrue if the predictive search is supported for
		*              the local language
		* Returns EFalse otherwise
		*/
		virtual TBool IsLanguageSupportedL(const TUint32 aLanguage) = 0;                                                                 	                                                      

		/**
		* Get the data order (mapped to VPbk ids) supported for a URI
		*/
		virtual void GetDataOrderL(TDesC& aURI, RArray<TInt>& aDataOrder) = 0;                                                            	                                                       

		/**
		* Get the sort order (mapped to VPbk ids) supported for a URI
		*/
		virtual void GetSortOrderL(TDesC& aURI, RArray<TInt>& aDataOrder) = 0;

		/**
		* Set the sort order (mapped to VPbk ids) supported for a URI
		*/
		virtual void ChangeSortOrderL(TDesC& aURI, RArray<TInt>& aDataOrder) = 0;
	                          
	private:    

		TUid iPluginId;
		TUid iDtor_ID_Key;

	};

#include "CPcsPlugin.inl"

#endif		// CPCS_PLUGIN_H

// End of File

