/*=====================================================================
	BinTree.h - Binary tree template class

	Author: Per Nilsson

	Freeware and no copyright on my behalf. However, if you use the 
	code in some manner	I'd appreciate a notification about it
	perfnurt@hotmail.com

	The Red-Black insertion algorithm is to some extent based upon
	the example in 
	http://www.oopweb.com/Algorithms/Documents/PLDS210/Volume/red_black.html

	Classes:

		// The node class.
		template <class KeyType, class ValueType> class BinTreeNode

		// The tree class
		template <class KeyType, class ValueType> class BinTree
		{
		public:
			// Regular low->high (++) and high->low (--) iterator
		    class Iterator

			// Top to bottom iterator
			class ParentFirstIterator

			// Bottom to top iterator
			class ParentLastIterator

			// Top to bottom, level by level iterator
			class ByLevelIterator
		}

	Requirements:
		The KeyType class must support:
			1. < and == operations.
			2. Copy construction.

		The ValueType must support:
			1. Copy construction.
			2. Assignment operation (=) if BinTreeNode::SetValue is used

    Dependencies:
		No external dependencies

    #define NO_REDBLACK to deactivate the red-black functionality. 
	Tree will still work, but nothing is done in regards of balancing.

=====================================================================*/

// TAP Changes by Robin Glover 4/1/2006
//
// Renamed BinTree class to 'map'
// Removed AccessClass so that operator[] works like std::map
// changed node.GetValue() to return a reference so that values can be 
//		updated (also a la std::map)
// Taken virtual of BinTreeNode dtor - no need and it can't have its
//		vtable fixed in a tap build
#ifndef _BINTREE_H_
#define _BINTREE_H_

//------------------------------------------------------------
// class BinTreeNode is the element type of the BinTree tree. 
//------------------------------------------------------------
template <class KeyType, class ValueType> 
class BinTreeNode
{
public:
	//------------------------------
	// Public Construction
	//------------------------------

	// Constructor(Key, Value)
	BinTreeNode(const KeyType& k, const ValueType& v)
		:mKey(k),mValue(v),mLeftChild(0),mRightChild(0),mParent(0)
#ifndef NO_REDBLACK
		,mIsRed(true)
#endif
	{}

	// Destructor
	~BinTreeNode(){}

	//------------------------------
	// Public Commands
	//------------------------------
	// Set methods.
	// Set*Child also updates the the child's parent attribute.
	void SetLeftChild(BinTreeNode* p)  { mLeftChild=p; if (p) p->SetParent(this);}
	void SetRightChild(BinTreeNode* p) { mRightChild=p;if (p) p->SetParent(this);}
	void SetParent(BinTreeNode* p)	   { mParent=p; }

	void SetValue(const ValueType& v)  { mValue = v; }
	// Note: Deliberately no SetKey, that could easily screw up the tree.
	// If a key shall be changed, delete node from tree and insert a new one instead.

#ifndef NO_REDBLACK
	void SetRed()        { mIsRed = true; }
	void SetBlack()      { mIsRed = false; }
#endif
	//------------------------------
	// Public Queries
	//------------------------------

	// Get methods
	BinTreeNode* GetLeftChild() const  { return mLeftChild; }
	BinTreeNode* GetRightChild() const { return mRightChild; }
	BinTreeNode* GetParent() const     { return mParent; }

	const ValueType& GetValue() const         { return mValue; }
	ValueType& GetValue()         { return mValue; }
	KeyType GetKey() const             { return mKey; }

	// Some more or less useful queries
	bool IsRoot() const           { return mParent==0; }
	bool IsLeftChild() const      { return mParent!=0 && mParent->GetLeftChild()==this; }
	bool IsRightChild() const     { return mParent!=0 && mParent->GetRightChild()==this; }
	bool IsLeaf() const           { return mLeftChild==0 && mRightChild==0; }
	unsigned int GetLevel() const { if (IsRoot()) return 1; else return GetParent()->GetLevel() + 1; }

#ifndef NO_REDBLACK
	bool IsRed() const   { return mIsRed; };
	bool IsBlack() const { return !mIsRed; };
#endif

private:
	//------------------------------
	// Disabled Methods
	//------------------------------

	// Default constructor - deliberately not implemented
	BinTreeNode();

	//------------------------------
	// Private Members
	//------------------------------
	BinTreeNode*	mLeftChild;
	BinTreeNode*	mRightChild;

	BinTreeNode*	mParent;

	KeyType			mKey;
	ValueType		mValue;

#ifndef NO_REDBLACK
	bool mIsRed;
#endif

};

//------------------------------------------------------------
// class BinTree. Holder of the tree structure. 
//------------------------------------------------------------
template <class KeyType, class ValueType> 
class map
{
public:
	//------------------------------
	// Public Definitions
	//------------------------------
	typedef BinTreeNode<KeyType,ValueType> Node;

	//------------------------------
	// Public Classes
	//------------------------------

	//--------------------------------------------------------
	// class BinTree::Iterator.
	// Regular low->high (++) and high->low (--) iterator
	//--------------------------------------------------------
	class Iterator
	{
	public:
		//------------------------------
		// Public Construction
		//------------------------------
		// Default Constructor
		Iterator():mRoot(0),mCur(0){}

		// Constructor(Node*)
		Iterator(Node* root):mRoot(root){ Reset();}

		// Copy constructor
		Iterator(const Iterator& src):mRoot(src.mRoot),mCur(src.mCur){}


		//------------------------------
		// Public Commands
		//------------------------------
		// Reset the iterator
		// atLowest : true  - Reset at lowest key value (and then ++ your way through)
		//            false - Reset at highest key value (and then -- your way through)
		void Reset(bool atLowest=true) 
		{ 
			if (atLowest) 
				mCur = Min(mRoot); 
			else 
				mCur = Max(mRoot);
		}

		//------------------------------
		// Public Queries
		//------------------------------

		// Has the iterator reached the end?
		bool atEnd() const { return mCur==0; }
		Node* GetNode() { return mCur;	}

		//------------------------------
		// Public Operators
		//------------------------------

		// Assignment operator
		Iterator& operator=(const Iterator& src) 
		{ 
			mRoot = src.mRoot; 
			mCur = src.mCur; 
			return (*this);
		}

		// Increment operator
		void operator++(int) { Inc(); }

		// Decrement operator
		void operator--(int)
		{
			Dec();
		}

		// Access operators
		Node* operator -> () { return GetNode();	}
		Node& operator*   () 
		{ 
			if (atEnd())
			{
				throw "Iterator at end";			
			}
			return *mCur; 
		}

	private:
		//------------------------------
		// Private Helper Methods
		//------------------------------

		// Min: Returns node with lowest key from n and down.
		//      Ie the node all down to the left
		Node* Min(Node* n)
		{
			while(n && n->GetLeftChild())
				n = n->GetLeftChild();
			return n;
		}
		// Min: Returns node with highest key from n and down.
		//      Ie the node all down to the right
		Node* Max(Node* n)
		{
			while(n && n->GetRightChild())
				n = n->GetRightChild();
			return n;
		}

		//------------------------------
		// Private Commands
		//------------------------------
		// ++
		void Inc()
		{
			// Already at end?
			if (mCur==0)
				return;

			if (mCur->GetRightChild())
			{
				// If current node has a right child, the next higher node is the 
				// node with lowest key beneath the right child.
				mCur =  Min(mCur->GetRightChild());
			}
			else if (mCur->IsLeftChild())
			{
				// No right child? Well if current node is a left child then
				// the next higher node is the parent
				mCur = mCur->GetParent();
			}
			else
			{
				// Current node neither is left child nor has a right child.
				// Ie it is either right child or root
				// The next higher node is the parent of the first non-right
				// child (ie either a left child or the root) up in the
				// hierarchy. Root's parent is 0.
				while(mCur->IsRightChild())
					mCur = mCur->GetParent();
				mCur =  mCur->GetParent();
			}
		}

		// --
		void Dec()
		{
			// Already at end?
			if (mCur==0)
				return;

			if (mCur->GetLeftChild())
			{
				// If current node has a left child, the next lower node is the 
				// node with highest key beneath the left child.
				mCur =  Max(mCur->GetLeftChild());
			}
			else if (mCur->IsRightChild())
			{
				// No left child? Well if current node is a right child then
				// the next lower node is the parent
				mCur =  mCur->GetParent();
			}
			else
			{
				// Current node neither is right child nor has a left child.
				// Ie it is either left child or root
				// The next higher node is the parent of the first non-left
				// child (ie either a right child or the root) up in the
				// hierarchy. Root's parent is 0.

				while(mCur->IsLeftChild())
					mCur = mCur->GetParent();
				mCur =  mCur->GetParent();
			}
		}

		//------------------------------
		// Private Members
		//------------------------------
		Node* mRoot;
		Node* mCur;
	}; // Iterator

	//--------------------------------------------------------
	// class BinTree::ParentFirstIterator. 
	// Traverses the tree from top to bottom. Typical usage is 
	// when storing the tree structure, because when reading it 
	// later (and inserting elements) the tree structure will
	// be the same.
	//--------------------------------------------------------
	class ParentFirstIterator
	{
	public:

		//------------------------------
		// Public Construction
		//------------------------------
		// Default constructor
		ParentFirstIterator():mRoot(0),mCur(0){}

		// Constructor(Node*)
		explicit ParentFirstIterator(Node* root):mRoot(root),mCur(0){ Reset(); }

		//------------------------------
		// Public Commands
		//------------------------------
		void Reset() { mCur = mRoot; };

		//------------------------------
		// Public Queries
		//------------------------------

		// Has the iterator reached the end?
		bool atEnd() const { return mCur==0; }
		Node* GetNode() { return mCur;	}

		//------------------------------
		// Public Operators
		//------------------------------

		// Assignment operator
		ParentFirstIterator& operator=(const ParentFirstIterator& src) 
		{ 
			mRoot = src.mRoot; mCur = src.mCur; return (*this);
		}

		// Increment operator
		void operator++(int) { Inc(); }

		// Access operators
		Node* operator -> () { return GetNode();	}
		Node& operator*   () 
		{ 
			if (atEnd())
				throw "ParentFirstIterator at end";			
			return *GetNode(); 
		}
	private:

		//------------------------------
		// Private Commands
		//------------------------------

		void Inc()
		{
			// Already at end?
			if (mCur==0)
				return;

			// First we try down to the left
			if (mCur->GetLeftChild())
			{
				mCur =  mCur->GetLeftChild();
			}
			else if (mCur->GetRightChild())
			{
				// No left child? The we go down to the right.
				mCur = mCur->GetRightChild();
			}
			else
			{
				// No children? Move up in the hierarcy until
				// we either reach 0 (and are finished) or
				// find a right uncle.
				while (mCur!=0)
				{
					// But if parent is left child and has a right "uncle" the parent
					// has already been processed but the uncle hasn't. Move to 
					// the uncle.
					if (mCur->IsLeftChild() && mCur->GetParent()->GetRightChild())
					{
						mCur = mCur->GetParent()->GetRightChild();
						return;
					}
					mCur = mCur->GetParent();
				}
			}
		}

		//------------------------------
		// Private Members
		//------------------------------
		Node* mRoot;
		Node* mCur;

	}; // ParentFirstIterator

	//--------------------------------------------------------
	// class BinTree::ParentLastIterator. 
	// Traverse the tree from bottom to top.
	// Typical usage is when deleting all elements in the tree
	// because you must delete the children before you delete
	// their parent.
	//--------------------------------------------------------
	class ParentLastIterator
	{
	public:

		//------------------------------
		// Public Construction
		//------------------------------
		// Default constructor
		ParentLastIterator():mRoot(0),mCur(0){}

		// Constructor(Node*)
		explicit ParentLastIterator(Node* root):mRoot(root),mCur(0){ Reset();}

		//------------------------------
		// Public Commands
		//------------------------------
		void Reset() { mCur = Min(mRoot); }

		//------------------------------
		// Public Queries
		//------------------------------

		// Has the iterator reached the end?
		bool atEnd() const { return mCur==0; }
		Node* GetNode() { return mCur;	}

		//------------------------------
		// Public Operators
		//------------------------------

		// Assignment operator
		ParentLastIterator& operator=(const ParentLastIterator& src) { mRoot = src.mRoot; mCur = src.mCur; return (*this);}

		// Increment operator
		void operator++(int) { Inc(); }

		// Access operators
		Node* operator -> () { return GetNode();	}
		Node& operator*   () 
		{ 
			if (atEnd())
				throw "ParentLastIterator at end";			
			return *GetNode(); 
		}
	private:
		//------------------------------
		// Private Helper Methods
		//------------------------------

		// Min: Returns the node as far down to the left as possible.
		Node* Min(Node* n)
		{
			while(n!=0 && (n->GetLeftChild()!=0 || n->GetRightChild()!=0))
			{
				if (n->GetLeftChild())
					n = n->GetLeftChild();
				else
					n = n->GetRightChild();
			}
			return n;
		}

		//------------------------------
		// Private Commands
		//------------------------------
		void Inc()
		{
			// Already at end?
			if (mCur==0)
				return;

			// Note: Starting point is the node as far down to the left as possible.

			// If current node has an uncle to the right, go to the
			// node as far down to the left from the uncle as possible
			// else just go up a level to the parent.
			if (mCur->IsLeftChild() && mCur->GetParent()->GetRightChild())
			{
				mCur = Min(mCur->GetParent()->GetRightChild());
			}
			else
				mCur = mCur->GetParent();
		}

		//------------------------------
		// Private Members
		//------------------------------
		Node* mRoot;
		Node* mCur;
	}; // ParentLastIterator

	// ByLevelIterator. Traverse tree top to bottom, level by level left to right.
	// Typical usage : I don't know. Perhaps if the tree should be displayed               
	// in some fancy way.
	// It is the most memory consuming of all iterators as it will allocate an 
	// array of (Size()+1)/2 Node*s.
	// Impl. desc:
	//   mArray[0] is the current node we're looking at, initially set to mRoot.
	//   When ++:ing the children of mArray[0] (if any) are put last in the 
	//   array and the array is shifted down 1 step
	class ByLevelIterator
	{
	public:
		//------------------------------
		// Public Construction
		//------------------------------

		// Default constructor
		ByLevelIterator():mRoot(0),mArray(0),mSize(0){}

		// Constructor(treeRoot, elementsInTree)
		ByLevelIterator(Node* root, unsigned int size):mRoot(root),mSize(size),mArray(0){ Reset();}

		// Copy constructor
		ByLevelIterator(const ByLevelIterator& src):mRoot(src.mRoot),mSize(src.mSize),mEndPos(src.mEndPos)
		{
			if (src.mArray!=0)
			{
				mArray = new Node*[(mSize+1)/2];
				memcpy(mArray,src.mArray,sizeof(Node*)*(mSize+1)/2);
			}
			else
				mArray = 0;
		}

		// Destructor
		~ByLevelIterator() 
		{ 
			if (mArray!=0)
			{
				delete [] mArray;
				mArray = 0;
			}
		}

		//------------------------------
		// Public Commands
		//------------------------------
		void Reset() 
		{ 
			if (mSize>0)
			{
				// Only allocate the first time Reset is called
				if (mArray==0)
				{
					// mArray must be able to hold the maximum "width" of the tree which
					// at most can be (NumberOfNodesInTree + 1 ) / 2
					mArray = new Node*[(mSize+1)/2];
				}
				// Initialize the array with 1 element, the mRoot.
				mArray[0] = mRoot;
				mEndPos = 1;
			}
			else 
				mEndPos=0;
		} // Reset

		//------------------------------
		// Public Queries
		//------------------------------

		// Has the iterator reached the end?
		bool atEnd() const { return mEndPos == 0; }
		Node* GetNode() { return mArray[0];	}

		//------------------------------
		// Public Operators
		//------------------------------

		// Assignment Operator
		ByLevelIterator& operator=(const ByLevelIterator& src) 
		{ 
			mRoot = src.mRoot; 
			mSize = src.mSize;
			if (src.mArray!=0)
			{
				mArray = new Node*[(mSize+1)/2];
				memcpy(mArray,src.mArray,sizeof(Node*)*(mSize+1)/2);
			}
			else
				mArray = 0;

			return (*this);
		}

		// Increment operator
		void operator++(int) { Inc(); }

		// Access operators
		Node* operator -> () { return GetNode(); }
		Node& operator*   () 
		{ 
			if (atEnd())
				throw "ParentLastIterator at end";			
			return *GetNode(); 
		}
	private:

		//------------------------------
		// Private Commands
		//------------------------------

		void Inc()
		{
			if (mEndPos == 0)
				return;

			// Current node is mArray[0]
			Node* pNode = mArray[0];

			// Move the array down one notch, ie we have a new current node 
			// (the one than just was mArray[1])
			for (unsigned int i=0;i<mEndPos;i++)
			{
				mArray[i] = mArray[i+1];
			}
			mEndPos--;

			Node* pChild=pNode->GetLeftChild();
			if (pChild) // Append the left child of the former current node
			{ 
				mArray[mEndPos] = pChild;
				mEndPos++;
			}

			pChild = pNode->GetRightChild();
			if (pChild) // Append the right child of the former current node
			{ 
				mArray[mEndPos] = pChild;
				mEndPos++;
			}

		}

		//------------------------------
		// Private Members
		//------------------------------
		Node** mArray;
		Node* mRoot;
		unsigned int mSize;
		unsigned int mEndPos;
	}; // ByLevelIterator

	// ==== Enough of that, lets get back to the BinTree class itself ====

	//------------------------------
	// Public Construction
	//------------------------------
	// Constructor.
	map():mRoot(0),mSize(0){}

	// Destructor
	~map(){ DeleteAll(); }

	//------------------------------
	// Public Commands
	//------------------------------

	bool Insert(const KeyType& keyNew, const ValueType& v)
#ifndef NO_REDBLACK
	// RED / BLACK insertion
	{
		// First insert node the "usual" way (no fancy balance logic yet)
		Node* newNode = new Node(keyNew,v);
		if (!Insert(newNode))
		{
			delete newNode;
			return false;
		}

		// Then attend a balancing party
		while  (!newNode->IsRoot() && (newNode->GetParent()->IsRed()))
		{
			if ( newNode->GetParent()->IsLeftChild()) 
			{
				// If newNode is a left child, get its right 'uncle'
				Node* newNodesUncle = newNode->GetParent()->GetParent()->GetRightChild();
				if ( newNodesUncle!=0 && newNodesUncle->IsRed())
				{
					// case 1 - change the colours
					newNode->GetParent()->SetBlack();
					newNodesUncle->SetBlack();
					newNode->GetParent()->GetParent()->SetRed();
					// Move newNode up the tree
					newNode = newNode->GetParent()->GetParent();
				}
				else 
				{
					// newNodesUncle is a black node
					if ( newNode->IsRightChild()) 
					{
					// and newNode is to the right
					// case 2 - move newNode up and rotate
					newNode = newNode->GetParent();
					RotateLeft(newNode);
					}
					// case 3
					newNode->GetParent()->SetBlack();
					newNode->GetParent()->GetParent()->SetRed();
					RotateRight(newNode->GetParent()->GetParent());
				}
			}
			else 
			{
				// If newNode is a right child, get its left 'uncle'
				Node* newNodesUncle = newNode->GetParent()->GetParent()->GetLeftChild();
				if ( newNodesUncle!=0 && newNodesUncle->IsRed())
				{
					// case 1 - change the colours
					newNode->GetParent()->SetBlack();
					newNodesUncle->SetBlack();
					newNode->GetParent()->GetParent()->SetRed();
					// Move newNode up the tree
					newNode = newNode->GetParent()->GetParent();
				}
				else 
				{
					// newNodesUncle is a black node
					if ( newNode->IsLeftChild()) 
					{
						// and newNode is to the left
						// case 2 - move newNode up and rotate
						newNode = newNode->GetParent();
						RotateRight(newNode);
					}
					// case 3
					newNode->GetParent()->SetBlack();
					newNode->GetParent()->GetParent()->SetRed();
					RotateLeft(newNode->GetParent()->GetParent());
				}

			}
		}
		// Color the root black
		mRoot->SetBlack();
		return true;
	}
#else
	// No balance logic insertion
	{
		Node* newNode = new Node(keyNew,v);
		if (!Insert(newNode))
		{
			delete newNode;
			return false;
		}
		return true;
	}
#endif // NO_REDBLACK

	// Set. If the key already exist just replace the value
	// else insert a new element.
	void Set(const KeyType& k, const ValueType& v)
	{
		Node* p = Find(k);
		if (p)
		{
			p->SetValue(v);
		}
		else
			Insert(k,v);
	}

	// Remove a node.Return true if the node could
	// be found (and was removed) in the tree.
	bool Delete(const KeyType& k)
	{
		Node* p = Find(k);
		if (p == 0) return false;

		// Rotate p down to the left until it has no right child, will get there
		// sooner or later.
		while(p->GetRightChild())
		{
			// "Pull up my right child and let it knock me down to the left"
			RotateLeft(p);
		}
		// p now has no right child but might have a left child
		Node* left = p->GetLeftChild(); 

		// Let p's parent point to p's child instead of point to p
		if (p->IsLeftChild())
		{
			p->GetParent()->SetLeftChild(left);
		}
		else if (p->IsRightChild())
		{
			p->GetParent()->SetRightChild(left);
		}
		else
		{
			// p has no parent => p is the root. 
			// Let the left child be the new root.
			SetRoot(left);
		}

		// p is now gone from the tree in the sense that 
		// no one is pointing at it. Let's get rid of it.
		delete p;
			
		mSize--;
		return true;
	}

	// Wipe out the entire tree.
	void DeleteAll()
	{
		ParentLastIterator i(GetParentLastIterator());

		while(!i.atEnd())
		{
			Node* p = i.GetNode();
			i++; // Increment it before it is deleted
			     // else iterator will get quite confused.
			delete p;
		}
		mRoot = 0;
		mSize= 0;
	}

	//------------------------------
	// Public Queries
	//------------------------------

	// Is the tree empty?
	bool IsEmpty() const { return mRoot == 0; }

	// Search for the node.
	// Returns 0 if node couldn't be found.
	Node* Find(const KeyType& keyToFind) const
	{
		Node* pNode = mRoot;

		while(pNode!=0)
		{
			KeyType key(pNode->GetKey());

			if (keyToFind == key)
			{
				// Found it! Return it! Wheee!
				return pNode;
			}			
			else if (keyToFind < key)
			{
				pNode = pNode->GetLeftChild();
			}
			else //keyToFind > key
			{
				pNode = pNode->GetRightChild();
			}
		}

		return 0;
	}
	
	// Get the root element. 0 if tree is empty.
	Node* GetRoot() const { return mRoot; }

	// Number of nodes in the tree.
	unsigned int Size() const { return mSize; }

	//------------------------------
	// Public Iterators
	//------------------------------
	Iterator GetIterator()			 
	{ 
		Iterator it(GetRoot());
		return it; 
	}
	ParentFirstIterator GetParentFirstIterator() 
	{
		ParentFirstIterator it(GetRoot());
		return it; 
	}
	ParentLastIterator GetParentLastIterator()
	{   
		ParentLastIterator it(GetRoot());
		return it;	
	}
	ByLevelIterator GetByLevelIterator()	 
	{ 
		ByLevelIterator it(GetRoot(),Size());
		return it;	
	}
	
	//------------------------------
	// Public Operators
	//------------------------------

	// operator [] for accesss to elements
	ValueType& operator[](const KeyType& k) 
	{
		Node* pNode = Find(k);
		if (pNode != NULL)
			return pNode->GetValue();

		Insert(k, ValueType());
		return Find(k)->GetValue();
	}
private:

	//------------------------------
	// Disabled methods
	//------------------------------
	// Copy constructor and assignment operator deliberately 
	// defined but not implemented. The tree should never be 
	// copied, pass along references to it instead (or use auto_ptr to it).
	explicit map(const map& src); 
	map& operator = (const map& src);


	//------------------------------
	// Private Commands
	//------------------------------
	void SetRoot(Node* newRoot)
	{
		mRoot = newRoot;
		if (mRoot!=0)
			mRoot->SetParent(0);
	}

	// Insert a node into the tree without using any fancy balancing logic.
	// Returns false if that key already exist in the tree.
	bool Insert(Node* newNode)
	{
		bool result=true; // Assume success

		if (mRoot==0)
		{
			SetRoot(newNode);
			mSize = 1;
		}
		else
		{
			Node* pNode = mRoot;
			KeyType keyNew = newNode->GetKey();
			while (pNode)
			{
				KeyType key(pNode->GetKey());

				if (keyNew == key)
				{
					result = false;
					pNode = 0;
				} 
				else if (keyNew < key)
				{
					if (pNode->GetLeftChild()==0)
					{
						pNode->SetLeftChild(newNode);
						pNode = 0;
					}
					else
					{
						pNode = pNode->GetLeftChild();
					}
				} 
				else 
				{
					// keyNew > key
					if (pNode->GetRightChild()==0)
					{
						pNode->SetRightChild(newNode);
						pNode = 0;
					}
					else
					{
						pNode = pNode->GetRightChild();
					}
				}
			}

			if (result)
			{
				mSize++;
			}
		}
		
		return result;
	}

	// Rotate left.
	// Pull up node's right child and let it knock node down to the left
	void RotateLeft(Node* p)
	{		
		Node* right = p->GetRightChild();

		p->SetRightChild(right->GetLeftChild());
		
		if (p->IsLeftChild())
			p->GetParent()->SetLeftChild(right);
		else if (p->IsRightChild())
			p->GetParent()->SetRightChild(right);
		else
		{
			SetRoot(right);
		}
		right->SetLeftChild(p);
	}

	// Rotate right.
	// Pull up node's left child and let it knock node down to the right
	void RotateRight(Node* p)
	{		

		Node* left = p->GetLeftChild();

		p->SetLeftChild(left->GetRightChild());
		
		if (p->IsLeftChild())
			p->GetParent()->SetLeftChild(left);
		else if (p->IsRightChild())
			p->GetParent()->SetRightChild(left);
		else
		{
			SetRoot(left);
		}
		left->SetRightChild(p);
	}

	//------------------------------
	// Private Members
	//------------------------------
	Node* mRoot; // The top node. 0 if empty.	
	unsigned int mSize; // Number of nodes in the tree
};
#endif // _BINTREE_H_

