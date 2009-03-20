#ifndef _BPTREE_H_
#define _BPTREE_H_


#include <iostream>
#include <ostream>
#include <assert.h>


#ifdef BTREE_DEBUG

#define TREE_PRINT(x)	do { if (debug) (std::cout <<x << endl; ); } while(0)

#define BTREE_ASSERT(x)  do { assert(x); } while(0)



#endif

using namespace std;

namespace nwt {
    //class for the btree

    template <typename _Key, typename _Datatype, int _nodeslots, int _leafslots, typename _Compare = std::less<_Key> >
class btree {
    public:
        //key type for this current instance of the btree.
        typedef _Key keytype;
        //Data type for current instance of the btree
        typedef _Datatype data_type;

        typedef std::pair<_Key, _Datatype> pair_type;

        typedef std::pair<_Key, bool> lookuppair_type;
        //Key comparison function
        typedef _Compare key_compare;

    private:

        static const unsigned short bt_nodenum;
        //Number of nodes in the tree
        short bt_numNodes;
        //Max leaf node data slots
        static const unsigned short bt_leafnodemax = _leafslots;
        //Min leaf node data slots
        static const unsigned short bt_leafnodemin = _leafslots / 2;
        //Max inner node data slots
        static const unsigned short bt_innernodemax = _nodeslots;
        //Min iiner node data slots
        static const unsigned short bt_innernodemin = _nodeslots / 2;

    private:
        struct innerngroup;
        //Private data structures

        struct node {
            //total number of slots
            unsigned short slots;
            //total number of slots in use
            unsigned short slotsinuse;
            //if its a leafnode
            bool isleafnode;
            bool isrootnode;

            inline bool isleaf() const {
                return isleafnode;
            }

            inline unsigned short keyCount() const {
                return slotsinuse;
            }
            inline bool isRoot() const {
                return isrootnode;
            }
        };
        //inherit node for inner node

        struct innerNode : public node {
            //slots for keys....inner nodes only have slots of keys and pointers
            keytype * keySlots[node::slots];
            //pointer to the first child, this is the pointer to the main segment
            innerngroup* firstChild;
            //number of child nodes present, max can be node:slots + 1
            int numChildren;

            inline void initialize() {
                node::slots = bt_innernodemax;
                node::leafnode = false;
                firstChild = NULL;
                keySlots = new keytype[node::slots];
            }

            inline bool equal(const innerNode & n) {
                if (node::keyCount() != n->keyCount())
                    return false;
                for (int i = 0; i < node::slotsinuse; i++)
                    if (keySlots[i] != n.keySlots[i])
                        return false;
                return true;
            }
            //check for underflow

            inline bool isunderflow() const {
                return (node::slotsinuse < bt_innernodemin);
            }
            //check if nodes slots are full.

            inline bool isfull() {
                return (node::slotsinuse == bt_innernodemax);
            };

            inline bool insertChild(keytype k, node * child) {
                return true;
            }

            inline bool findKey(keytype k) {
                for (int i = 0; i < node::slotsinuse; i++)
                    if (keySlots[i] == k)
                        return true;
                return false;
            }

            inline innerngroup * getChild(int i) {
                if (firstChild == NULL)
                    return NULL;
                else
                    return firstChild.group[0];

            }

        };

        //inherit node for leaf node

        struct leafNode : public node {
            //keys for leaf nodes
            keytype * keySlots[node::slot];
            data_type * dataSlots[node::slot];

            //keep track of the next and previous nodes
            //They will be helpful for range queries
            leafNode* nextLeaf;
            leafNode* prevLeaf;

            inline void initialize() {
                node::slots = bt_leafnodemax;
                node::leafnode = true;
                prevLeaf = nextLeaf = NULL;
                dataSlots = new data_type[node::slot];
                keySlots = new keytype[node::slot];
            }

            inline bool equal(const leafNode & n) {
                if (node::keyCount() != n->keyCount())
                    return false;
                for (int i = 0; i < node::slotsinuse; i++)
                    if (dataSlots[i] != n.dataSlots[i])
                        return false;
                return true;
            }
            //check if node is full

            inline bool isfull() const {
                return (node::slotsinuse == bt_leafnodemax);
            }
            //check if there are node is less than full

            inline bool isfew() const {
                return (node::slotsinuse <= bt_leafnodemin);
            }
            //check if there is an underflow

            inline bool isunderflow()const {
                return (node::slotsinuse < bt_leafnodemin);
            }

            inline bool insertData(data_type * data) {
                for (int i = 0; i < node::slotsinuse; i++) {
                    //dosomething
                }
                return false;
            }
        };

        struct innerngroup {
            innerNode * group[bt_innernodemax];

            inline void initialize() {
                group = new innerNode[bt_innernodemax];
            }
        };

        struct leafngroup {
        };


    //public methods
private:
        node* root;
        //D-value of the tree, leaf node will always can hold 2d keys.
        leafNode* tailleaf;
        leafNode* headleaf;
public:
    inline node* getRoot() {
        return btree::root;
    }
    //size, this will return the number of data values in the treee
    int size();
    //returns true if there is a least 1 data/key value in the tree
    bool empty();
    //get leaf slot size
    int keyslotsize();
    //get node slot size
    int nodeslotsize();
    //erase
    void erase();
    /**
     * Delete key, data pair from index
     */
    void delete_pair(keytype k, data_type data);

    /**
     *Find Key in tree.
     **/
    inline iterator find(keytype k) {
        int i = 0;
        node* curNode = getRoot;
        keytype* ki = NULL;

        if (btree::empty())
            return false;

        while (!curNode->isleaf()) {
            for (i = 0; i < curNode.keyCount(); i++) {
                TREE_PRINT("looking for key " << curNode << "and at index" << i);
                if (key_compare(curNode->keySlots[i], k) > 0) {
                    //found a key that is greater than the look being searched
                    ki = curNode->keySlots[i];
                    break;
                }
            }
            //this key is greater than all keys in node
            if (ki == NULL) {
                if (curNode->numChildren > 0)
                    curNode = curNode->getChild(curNode->numChildren - 1);
                TREE_PRINT("returning node child " << curNode->numChildren - 1);
            } else {
                curNode = curNode->getChild(i);

            }
        }
        //if we are out of the loop it means we have reached a leaf node, return false
        for (i = 0; i < curNode.keyCount(); i++) {
            if (key_compare(curNode->keySlots[i], k) == 0) {
                return iterator(curNode, i);
            }
        }


        return end();
    }

    /**
     *  just calls find and returns a bool the key exists
     **/
    inline bool exists(keytype k) {
        if (find(k) != NULL)
            return true;

        return false;

    }

    inline bool insert(keytype k, data_type data) {

        return true;
    }
    private:

    void insert_in_parent(node* N, keytype k, node* Nprime);

    private:
        // *** Template Magic to Convert a pair or key/data types to a value_type

        /// \internal For sets the second pair_type is an empty struct, so the
        /// value_type should only be the first.

        template <typename value_type, typename pair_type>
        struct btree_pair_to_value {
            /// Convert a fake pair type to just the first component

            inline value_type operator()(pair_type & p) const {
                return p.first;
            }
            /// Convert a fake pair type to just the first component

            inline value_type operator()(const pair_type & p) const {
                return p.first;
            }
        };

        /// \internal For maps value_type is the same as the pair_type

        template <typename value_type>
        struct btree_pair_to_value<value_type, value_type> {
            /// Identity "convert" a real pair type to just the first component

            inline value_type operator()(pair_type & p) const {
                return p;
            }
            /// Identity "convert" a real pair type to just the first component

            inline value_type operator()(const pair_type & p) const {
                return p;
            }
        };

        /// Using template specialization select the correct converter used by the
        /// iterators
        typedef btree_pair_to_value<pair_type, pair_type> pair_to_value_type;


    public:
        // *** Iterators and Reverse Iterators

        class iterator;
        class const_iterator;
        class reverse_iterator;
        class const_reverse_iterator;

        /// STL-like iterator object for B+ tree items. The iterator points to a
        /// specific slot number in a leaf.

        class iterator {
        public:
            // *** Types

            /// The key type of the btree. Returned by key().
            typedef typename btree::keytype key_type;

            /// The data type of the btree. Returned by data().
            typedef typename btree::data_type data_type;

            /// The value type of the btree. Returned by operator*().

            /// The pair type of the btree.
            typedef typename btree::pair_type pair_type;

            /// Reference to the value_type. STL required.
            typedef pair_type& reference;

            /// Pointer to the value_type. STL required.
            typedef pair_type* pointer;

            /// STL-magic iterator category
            //typedef std::bidirectional_iterator_tag iterator_category;

            /// STL-magic
            //typedef ptrdiff_t               difference_type;

            /// Our own type
            typedef iterator self;

        private:
            // *** Members

            /// The currently referenced leaf node of the tree
            typename btree::leafNode* currnode;

            /// Current key/data slot referenced
            unsigned short currslot;

            /// Friendly to the const_iterator, so it may access the two data items directly.
            friend class const_iterator;

            /// Also friendly to the reverse_iterator, so it may access the two data items directly.
            friend class reverse_iterator;

            /// Also friendly to the const_reverse_iterator, so it may access the two data items directly.
            friend class const_reverse_iterator;

            /// Evil! A temporary value_type to STL-correctly deliver operator* and
            /// operator->
            mutable pair_type temp_value;

            // The macro BTREE_FRIENDS can be used by outside class to access the B+
            // tree internals. This was added for wxBTreeDemo to be able to draw the
            // tree.

        public:
            // *** Methods

            /// Default-Constructor of a mutable iterator

            inline iterator()
            : currnode(NULL), currslot(0) {
            }

            /// Initializing-Constructor of a mutable iterator

            inline iterator(typename btree::leafNode *l, unsigned short s)
            : currnode(l), currslot(s) {
            }

            /// Copy-constructor from a reverse iterator

            inline iterator(const reverse_iterator &it)
            : currnode(it.currnode), currslot(it.currslot) {
            }

            /// Dereference the iterator, this is not a value_type& because key and
            /// value are not stored together

            inline reference operator*() const {
                temp_value = pair_to_value_type()(pair_type(currnode->keySlots[currslot],
                        currnode->dataSlots[currslot]));
                return temp_value;
            }

            /// Dereference the iterator. Do not use this if possible, use key()
            /// and data() instead. The B+ tree does not stored key and data
            /// together.

            inline pointer operator->() const {
                temp_value = pair_to_value_type()(pair_type(currnode->keySlots[currslot],
                        currnode->dataSlots[currslot]));
                return &temp_value;
            }

            /// Key of the current slot

            inline const key_type& key() const {
                return currnode->keySlots[currslot];
            }

            /// Writable reference to the current data object

            inline data_type& data() const {
                return currnode->dataSlots[currslot];
            }

            /// Prefix++ advance the iterator to the next slot

            inline self & operator++() {
                if (currslot + 1 < currnode->slotsinuse) {
                    ++currslot;
                } else if (currnode->nextLeaf != NULL) {
                    currnode = currnode->nextLeaf;
                    currslot = 0;
                } else {
                    // this is end()
                    currslot = currnode->slotsinuse;
                }

                return *this;
            }

            /// Postfix++ advance the iterator to the next slot

            inline self operator++(int) {
                self tmp = *this; // copy ourselves

                if (currslot + 1 < currnode->slotsinuse) {
                    ++currslot;
                } else if (currnode->nextLeaf != NULL) {
                    currnode = currnode->nextLeaf;
                    currslot = 0;
                } else {
                    // this is end()
                    currslot = currnode->slotsinuse;
                }

                return tmp;
            }

            /// Prefix-- backstep the iterator to the last slot

            inline self & operator--() {
                if (currslot > 0) {
                    --currslot;
                } else if (currnode->prevLeaf != NULL) {
                    currnode = currnode->prevLeaf;
                    currslot = currnode->slotsinuse - 1;
                } else {
                    // this is begin()
                    currslot = 0;
                }

                return *this;
            }

            /// Postfix-- backstep the iterator to the last slot

            inline self operator--(int) {
                self tmp = *this; // copy ourselves

                if (currslot > 0) {
                    --currslot;
                } else if (currnode->prevLeaf != NULL) {
                    currnode = currnode->prevLeaf;
                    currslot = currnode->slotsinuse - 1;
                } else {
                    // this is begin()
                    currslot = 0;
                }

                return tmp;
            }

            /// Equality of iterators

            inline bool operator==(const self& x) const {
                return (x.currnode == currnode) && (x.currslot == currslot);
            }

            /// Inequality of iterators

            inline bool operator!=(const self& x) const {
                return (x.currnode != currnode) || (x.currslot != currslot);
            }
        };

        /// STL-like read-only iterator object for B+ tree items. The iterator
        /// points to a specific slot number in a leaf.
    public:
        // *** STL Iterator Construction Functions

        /// Constructs a read/data-write iterator that points to the first slot in
        /// the first leaf of the B+ tree.

        inline iterator begin() {
            return iterator(headleaf, 0);
        }

        /// Constructs a read/data-write iterator that points to the first invalid
        /// slot in the last leaf of the B+ tree.

        inline iterator end() {
            return iterator(tailleaf, tailleaf ? tailleaf->slotsinuse : 0);
        }

        /// Constructs a read-only constant iterator that points to the first slot
        /// in the first leaf of the B+ tree.

        inline const_iterator begin() const {
            return const_iterator(headleaf, 0);
        }

        /// Constructs a read-only constant iterator that points to the first
        /// invalid slot in the last leaf of the B+ tree.

        inline const_iterator end() const {
            return const_iterator(tailleaf, tailleaf ? tailleaf->slotsinuse : 0);
        }

        /// Constructs a read/data-write reverse iterator that points to the first
        /// invalid slot in the last leaf of the B+ tree. Uses STL magic.

        inline reverse_iterator rbegin() {
            return reverse_iterator(end());
        }

        /// Constructs a read/data-write reverse iterator that points to the first
        /// slot in the first leaf of the B+ tree. Uses STL magic.

        inline reverse_iterator rend() {
            return reverse_iterator(begin());
        }

        /// Constructs a read-only reverse iterator that points to the first
        /// invalid slot in the last leaf of the B+ tree. Uses STL magic.

        inline const_reverse_iterator rbegin() const {
            return const_reverse_iterator(end());
        }

        /// Constructs a read-only reverse iterator that points to the first slot
        /// in the first leaf of the B+ tree. Uses STL magic.

        inline const_reverse_iterator rend() const {
            return const_reverse_iterator(begin());
        }
   

        /// The pair type of the btree.
    
        /// Reference to the value_type. STL required.
        typedef pair_type& reference;

        /// Pointer to the value_type. STL required.
        typedef pair_type* pointer;

        /// STL-magic iterator category
        typedef std::bidirectional_iterator_tag iterator_category;

        /// STL-magic
        typedef ptrdiff_t difference_type;

        /// Our own type
        typedef reverse_iterator self;

    private:
        // *** Members

        /// The currently referenced leaf node of the tree
        typename btree::leafNode* currnode;

        /// One slot past the current key/data slot referenced.
        unsigned short currslot;

        /// Friendly to the const_iterator, so it may access the two data items directly
        friend class iterator;

        /// Also friendly to the const_iterator, so it may access the two data items directly
        friend class const_iterator;

        /// Also friendly to the const_iterator, so it may access the two data items directly
        friend class const_reverse_iterator;

        /// Evil! A temporary value_type to STL-correctly deliver operator* and
        /// operator->
        mutable pair_type temp_value;

    public:
        // *** Methods

        /// Default-Constructor of a reverse iterator
//
//        inline reverse_iterator()
//        : currnode(NULL), currslot(0) {
//        }
//
//        /// Initializing-Constructor of a mutable reverse iterator
//
//        inline reverse_iterator(typename btree::leafNode *l, unsigned short s)
//        : currnode(l), currslot(s) {
//        }
//
//        /// Copy-constructor from a mutable iterator
//
//        inline reverse_iterator(const iterator &it)
//        : currnode(it.currnode), currslot(it.currslot) {
//        }

        /// Dereference the iterator, this is not a value_type& because key and
        /// value are not stored together

        inline reference operator*() const {
            assert(currslot > 0);
            temp_value = pair_to_value_type()(pair_type(currnode->keySlots[currslot - 1],
                    currnode->dataSlots[currslot - 1]));
            return temp_value;
        }

        /// Dereference the iterator. Do not use this if possible, use key()
        /// and data() instead. The B+ tree does not stored key and data
        /// together.

        inline pointer operator->() const {
            assert(currslot > 0);
            temp_value = pair_to_value_type()(pair_type(currnode->keySlots[currslot - 1],
                    currnode->dataSlots[currslot - 1]));
            return &temp_value;
        }

        /// Key of the current slot

        inline const keytype& key() const {
            assert(currslot > 0);
            return currnode->keySlots[currslot - 1];
        }

        /// Writable reference to the current data object

        inline data_type& data() const {
            assert(currslot > 0);
            return currnode->dataSlots[currslot - 1];
        }

        /// Prefix++ advance the iterator to the next slot

        inline self & operator++() {
            if (currslot > 1) {
                --currslot;
            } else if (currnode->prevLeaf != NULL) {
                currnode = currnode->prevLeaf;
                currslot = currnode->slotsinuse;
            } else {
                // this is begin() == rend()
                currslot = 0;
            }

            return *this;
        }

        /// Postfix++ advance the iterator to the next slot

        inline self operator++(int) {
            self tmp = *this; // copy ourselves

            if (currslot > 1) {
                --currslot;
            } else if (currnode->prevLeaf != NULL) {
                currnode = currnode->prevLeaf;
                currslot = currnode->slotsinuse;
            } else {
                // this is begin() == rend()
                currslot = 0;
            }

            return tmp;
        }

        /// Prefix-- backstep the iterator to the last slot

        inline self & operator--() {
            if (currslot < currnode->slotsinuse) {
                ++currslot;
            } else if (currnode->nextLeaf != NULL) {
                currnode = currnode->nextLeaf;
                currslot = 1;
            } else {
                // this is end() == rbegin()
                currslot = currnode->slotsinuse;
            }

            return *this;
        }

        /// Postfix-- backstep the iterator to the last slot

        inline self operator--(int) {
            self tmp = *this; // copy ourselves

            if (currslot < currnode->slotsinuse) {
                ++currslot;
            } else if (currnode->nextLeaf != NULL) {
                currnode = currnode->nextLeaf;
                currslot = 1;
            } else {
                // this is end() == rbegin()
                currslot = currnode->slotsinuse;
            }

            return tmp;
        }

        /// Equality of iterators

        inline bool operator==(const self& x) const {
            return (x.currnode == currnode) && (x.currslot == currslot);
        }

        /// Inequality of iterators

        inline bool operator!=(const self& x) const {
            return (x.currnode != currnode) || (x.currslot != currslot);
        }


};
};
#endif
