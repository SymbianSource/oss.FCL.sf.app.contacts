/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*      View node class.
*
*/


#ifndef __DigViewGraph_H__
#define __DigViewGraph_H__

// INCLUDES
#include <e32base.h>    // CBase

// FORWARD DECLARATIONS
class CDigViewNode;
class TResourceReader;

// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * View transition class.
 */
class TDigViewTransition
    {
    public:
        /// Own: ID of an event triggering this transition
        TInt iEvent;
        /// Own: target node view id
        TUid iTargetViewId;
        /// Ref: node to activate if this transition is accepted
        CDigViewNode* iNode;
    };

/**
 * @internal Only Phonebook internal use supported!
 *
 * View navigation node class.
 */
class CDigViewNode : public CBase
    {
    public: // Destructor.
        /**
         * Destructor.
         */
        IMPORT_C ~CDigViewNode();

    private:  // Constructor
        /**
         * Default constructor.
         */
        inline CDigViewNode();
        
    public:  // Interface
        /**
         * Returns the view id of this node.
         */
        inline TUid ViewId() const;

        /**
         * Returns the default previous node.
         */
        inline CDigViewNode* DefaultPreviousNode() const;

        /**
         * Returns the currently set previous node.
         */
        inline CDigViewNode* PreviousNode() const;

        /**
         * Returns ETrue if this is an exit node.
         */
        inline TBool ExitNode() const;

        /**
         * Returns additional resource id.
         */
        inline TInt AdditionalResId() const;
        
        /**
         * Sets previous node to aNode.
         */
        inline void SetPreviousNode(CDigViewNode* aNode);

        /**
         * Returns the number of transitions from this node.
         */
        inline TInt TransitionCount() const;

        /**
         * Returns aIndex:th transition from this node.
         */
        inline const TDigViewTransition& TransitionAt(TInt aIndex) const;

        /**
         * Returns ETrue if this node contains a transition equal to 
         * aTransition.
         */
        IMPORT_C TBool FindTransition
                (const TDigViewTransition& aTransition) const;

    private:
        TDigViewTransition* FindTransition(const TInt aEvent);
        void MergeL(CDigViewNode& aNewNode);        

    private:  // Data
        /// Own: view id
        TUid iViewId;
        /// Own: default previous node id
        TUid iDefaultPrevViewId;
        /// Ref: default previous node
        CDigViewNode* iDefaultPreviousNode;
        /// Ref: previous node
        CDigViewNode* iPreviousNode;
        /// Own: ETrue if this is an exit node
        TBool iExitNode;
        /// Own: transitions array
        CArrayFixFlat<TDigViewTransition> iTransitions;
        /// Own: Additional resource id
        TInt iAdditionalResId;

    private:  // Friends
        friend class CDigViewGraph;
    };


/**
 * @internal Only Phonebook internal use supported!
 *
 * View navigation graph class.
 */
class CDigViewGraph : public CBase
    {
    public:  // Constructors and destructor
        /**
         * Creates a new view graph from a DIG_VIEW_GRAPH resource.
         * @param aResReader  a DIG_VIEW_GRAPH resource reader
		 * @return created view graph
         */
        IMPORT_C static CDigViewGraph* NewL
            (TResourceReader& aResReader);

        /**
         * Destructor.
         */
        IMPORT_C ~CDigViewGraph();

    public:  // Interface
        /**
         * Returns node with resource id aViewId, NULL if not found.
         */
        IMPORT_C CDigViewNode* FindNodeWithViewId(TUid aViewId) const;

        /**
         * Modifies existing view graph. Updates transitions and
         * appends new view node according to aResReader.
         *
         * @param aResReader reads data initialized with DIG_VIEW_GRAPH
         *                   resource
         */
        IMPORT_C void ModifyViewGraphL(TResourceReader& aResReader);

        /**
         * Returns view graph node count.
         */
        inline TInt Count() const;

        /**
         * Returns view graph node at aIndex.
         */
        inline const CDigViewNode& operator[](TInt aIndex) const;

    private: // Implementation
        inline CDigViewGraph();
        void ConstructL(TResourceReader& aResReader);

        typedef CArrayPtrFlat<CDigViewNode> CDigViewNodeArray;

        CDigViewNodeArray* CreateUnlinkedNodesFromResourceL
                (TResourceReader& aResReader);        
        void LinkNodesWithTransitions();
        void MergeNodesL(CDigViewNodeArray& aChangedNodes);

    private:  // Data
        /// Own: array of nodes
        CDigViewNodeArray* iNodes;
    };


// INLINE FUNCTIONS

inline CDigViewNode::CDigViewNode()
    : iTransitions(1)  // Reallocation granularity of 1
    {
    }

inline TUid CDigViewNode::ViewId() const
    {
    return iViewId;
    }

inline CDigViewNode* CDigViewNode::DefaultPreviousNode() const
    {
    return iDefaultPreviousNode;
    }

inline CDigViewNode* CDigViewNode::PreviousNode() const
    {
    return iPreviousNode;
    }

inline TBool CDigViewNode::ExitNode() const
    {
    return iExitNode;
    }

inline TInt CDigViewNode::AdditionalResId() const
    {
    return iAdditionalResId;
    }

inline void CDigViewNode::SetPreviousNode(CDigViewNode* aNode)
    {
    iPreviousNode = aNode;
    }

inline TInt CDigViewNode::TransitionCount() const
    {
    return iTransitions.Count();
    }

inline const TDigViewTransition& CDigViewNode::TransitionAt(TInt aIndex) const
    {
    // iTransitions::operator[] will check aIndex
    return iTransitions[aIndex];
    }

inline TBool operator==(const TDigViewTransition& aLeft, const TDigViewTransition& aRight)
    {
    return (aLeft.iEvent==aRight.iEvent && aLeft.iNode==aRight.iNode);
    }

inline CDigViewGraph::CDigViewGraph() 
    {
    }

inline TInt CDigViewGraph::Count() const 
    { 
    return iNodes->Count(); 
    }

inline const CDigViewNode& CDigViewGraph::operator[](TInt aIndex) const
    {
    // iNodes::operator[] will check aIndex
    return *iNodes->At(aIndex);
    }


#endif // __DigViewGraph_H__

// End of File
