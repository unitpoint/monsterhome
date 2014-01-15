#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__

/******************************************************************************
* Copyright (C) 2012 Evgeniy Golovin (evgeniy.golovin@unitpoint.ru)
*
* Latest source code: https://github.com/unitpoint/monsterhome
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

#include <ExtCommon.h>
#include <MathLib.h>
#include <xString.h>

template <class T>
struct HashTableKey
{
  T value;

  HashTableKey(const T& k): value(k){}
  int Cmp(const HashTableKey& b) const
  {
    return value < b.value ? -1 : (value > b.value ? 1 : 0);
  }
  int Hash() const { return (int)value; }
};

typedef HashTableKey<int> HashIntKey;
typedef HashTableKey<float> HashFloatKey;
typedef HashTableKey<void*> HashPtrKey;

struct HashLabel
{
  int head;
  int node;

  HashLabel(){ head = node = 0; }
};

template < class KeyType, class ValueType >
class HashTable
{
public:

  enum
  {
    ITERATE_CONTINUE     = 0,
    ITERATE_BREAK        = 1,
    ITERATE_DELETE_ITEM  = 2,
  };

  typedef int (*IterateFunc)(const KeyType& k, ValueType& v, void * params);

  HashTable(int newTableSize = 8);
  HashTable(const HashTable<KeyType, ValueType> &map);
  ~HashTable();

  HashTable& operator=(const HashTable<KeyType, ValueType> &map);

  int Count() const;

  int TableSize() const;
  void SetTableSize(int);

  int AllocatedSize() const;

  ValueType * Get(const KeyType& key) const;
  ValueType * Set(const KeyType& key, const ValueType& value);
  void Add(const KeyType& key, const ValueType& value);
  bool Remove(const KeyType& key);

  const ValueType& GetValue(const KeyType& key, const ValueType& defValue) const;
  const ValueType& GetValue(const KeyType& key) const;

  ValueType * GetAt(int index, KeyType ** pkey = NULL) const;
  const ValueType & GetValueAt(int index, KeyType ** pkey = NULL) const;

  void Grow();
  void Clear();
  void DeleteContents();

  ValueType * ForEach(IterateFunc f, void * params);

  bool First(const KeyType& key, HashLabel& label, const KeyType *& foundKey, const ValueType *& foundValue) const;
  bool Next(HashLabel& label, const KeyType *& foundKey, const ValueType *& foundValue) const;

  bool FirstValue(HashLabel& label, const KeyType *& foundKey, const ValueType *& foundValue) const;
  bool NextValue(HashLabel& label, const KeyType *& foundKey, const ValueType *& foundValue) const;
  bool NextHeadValue(HashLabel& label, const KeyType *& foundKey, const ValueType *& foundValue) const;

  bool Remove(HashLabel& label);

  void ToKeys(Vector<KeyType*>& out);
  void ToValues(Vector<ValueType*>& out);
  void ToValues(Vector<ValueType>& out);

protected:

  struct Node
  {
    KeyType key;
    ValueType value;
    Node *next;

    Node(const KeyType& k, const ValueType& v, Node * n) : key(k), value(v), next(n) {};
  };

  Node **	heads;

  int tableSize;
  int count;
};

template < class KeyType, class ValueType >
inline HashTable<KeyType, ValueType>::HashTable(int newTableSize)
{
  tableSize = newTableSize > 0 ? MathLib::CeilPowerOfTwo(newTableSize) : 8;
  ASSERT(tableSize > 0);

  heads = DNEW Node *[ tableSize ];
  __MEMSET(heads, 0, sizeof(*heads) * tableSize);

  count = 0;
}

template < class KeyType, class ValueType >
inline HashTable<KeyType, ValueType>::HashTable(const HashTable<KeyType, ValueType> &map)
{
  ASSERT(map.tableSize > 0);

  tableSize = map.tableSize;
  heads = DNEW Node* [ tableSize ];
  count = map.count;

  for(int i = 0; i < tableSize; i++)
  {
    if(!map.heads[ i ])
    {
      heads[ i ] = NULL;
      continue;
    }

    Node ** prev = &heads[ i ];
    for(Node * node = map.heads[ i ]; node; node = node->next)
    {
      *prev = DNEW Node(node->key, node->value, NULL);
      prev = &(*prev)->next;
    }
  }
}

template < class KeyType, class ValueType >
HashTable<KeyType, ValueType>& HashTable<KeyType, ValueType>::operator=(const HashTable<KeyType, ValueType> &map)
{
  ASSERT(map.tableSize > 0);

  Clear();
  
  if(tableSize != map.tableSize)
  {
    DDELETEARR(heads);
    tableSize = map.tableSize;
    heads = DNEW Node* [ tableSize ];
  }

  count = map.count;

  for(int i = 0; i < tableSize; i++)
  {
    if(!map.heads[ i ])
    {
      heads[ i ] = NULL;
      continue;
    }

    Node ** cur = &heads[ i ];
    for(Node * node = map.heads[ i ]; node; node = node->next)
    {
      *cur = DNEW Node(node->key, node->value, NULL);
      cur = &(*cur)->next;
    }
  }

  return *this;
}

template < class KeyType, class ValueType >
inline HashTable<KeyType, ValueType>::~HashTable()
{
  Clear();
  DDELETEARR(heads);
}

template < class KeyType, class ValueType >
inline int HashTable<KeyType, ValueType>::AllocatedSize() const
{
  return sizeof(heads) * tableSize + sizeof(*heads) * count;
}

template < class KeyType, class ValueType >
inline int HashTable<KeyType, ValueType>::Count() const
{
  return count;
}

template < class KeyType, class ValueType >
inline int HashTable<KeyType, ValueType>::TableSize() const
{
  return tableSize;
}

template < class KeyType, class ValueType >
inline void HashTable<KeyType, ValueType>::SetTableSize(int newTableSize)
{
  newTableSize = newTableSize > 0 ? MathLib::CeilPowerOfTwo(newTableSize) : 8;
  if(newTableSize == tableSize)
  {
    return;
  }

  Node ** oldheads = heads;
  int oldTableSize = tableSize;

  tableSize = newTableSize;
  count = 0;

  heads = DNEW Node *[ tableSize ];
  __MEMSET(heads, 0, sizeof(*heads) * tableSize);

  for(int i = 0; i < oldTableSize; i++)
  {
    Node * next = oldheads[ i ];
    while(next)
    {
      Node * node = next;
      next = next->next;

      Add(node->key, node->value);

      DDELETE(node);
    }
  }
  DDELETEARR(oldheads);
}

template < class KeyType, class ValueType >
inline ValueType * HashTable<KeyType, ValueType>::Set(const KeyType& key, const ValueType& value)
{
  if((count>>1) > tableSize)
  {
    Grow();
  }

  int hash = key.Hash() & (tableSize - 1);
  Node ** nextPtr = &heads[hash];
  Node * node = *nextPtr;
  for(; node; nextPtr = &node->next, node = *nextPtr)
  {
    int s = node->key.Cmp(key);
    if(s == 0)
    {
      node->value = value;
      return &node->value;
    }
    if(s > 0)
    {
      break;
    }
  }

  count++;

  Node * newNode = DNEW Node(key, value, heads[ hash ]);
  *nextPtr = newNode;
  newNode->next = node;
  return &newNode->value;
}

template < class KeyType, class ValueType >
inline void HashTable<KeyType, ValueType>::Add(const KeyType& key, const ValueType& value)
{
  if((count>>1) > tableSize)
  {
    Grow();
  }
  
  int hash = key.Hash() & (tableSize - 1);
  Node ** nextPtr = &heads[hash];
  Node * node = *nextPtr;
  for(; node; nextPtr = &node->next, node = *nextPtr)
  {
    int s = node->key.Cmp(key);
    if(s >= 0)
    {
      break;
    }
  }

  count++;

  *nextPtr = DNEW Node(key, value, heads[ hash ]);
  (*nextPtr)->next = node;
}

template < class KeyType, class ValueType >
inline const ValueType& HashTable<KeyType, ValueType>::GetValue(const KeyType& key, const ValueType& defValue) const
{
  const ValueType * pval = Get(key);
  return pval ? *pval : defValue;
}

template < class KeyType, class ValueType >
inline const ValueType& HashTable<KeyType, ValueType>::GetValue(const KeyType& key) const
{
  const ValueType * pval = Get(key);
  ASSERT(pval);
  return *pval;
}

template < class KeyType, class ValueType >
inline ValueType * HashTable<KeyType, ValueType>::Get(const KeyType& key) const
{
  int hash = key.Hash() & (tableSize - 1);
  for(Node * node = heads[ hash ]; node; node = node->next)
  {
    int s = node->key.Cmp(key);
    if(s == 0)
    {
      return &node->value;
    }
    if(s > 0)
    {
      break;
    }
  }
  return NULL;
}

template < class KeyType, class ValueType >
inline bool HashTable<KeyType, ValueType>::First(const KeyType& key, HashLabel& label, const KeyType *& foundKey, const ValueType *& foundValue) const 
{
  int hash = key.Hash() & (tableSize - 1);
  for(Node * node = heads[hash]; node; node = node->next)
  {
    if(node->key.Cmp(key) == 0)
    {
      label.head = hash;
      label.node = (int)node;
      foundKey = &node->key;
      foundValue = &node->value;
      return true;
    }
  }
  foundKey = NULL;
  foundValue = NULL;
  return false;
}

template < class KeyType, class ValueType >
inline bool HashTable<KeyType, ValueType>::Next(HashLabel& label, const KeyType *& foundKey, const ValueType *& foundValue) const 
{
  if(label.node)
  {
    Node * node = ((Node*)label.node)->next;
    for(; node; node = node->next)
    {
      if(node->key.Cmp(*foundKey) == 0)
      {
        label.node = (int)node;
        foundKey = &node->key;
        foundValue = &node->value;
        return true;
      }
    }
  }
  return false;
}

template < class KeyType, class ValueType >
inline bool HashTable<KeyType, ValueType>::FirstValue(HashLabel& label, const KeyType *& foundKey, const ValueType *& foundValue) const 
{
  for(int i = 0; i < tableSize; i++)
  {
    if(heads[i])
    {
      label.head = i;
      label.node = (int)heads[i];
      foundKey = &heads[i]->key;
      foundValue = &heads[i]->value;
      return true;
    }
  }
  foundKey = NULL;
  foundValue = NULL;
  return false;
}

template < class KeyType, class ValueType >
inline bool HashTable<KeyType, ValueType>::NextValue(HashLabel& label, const KeyType *& foundKey, const ValueType *& foundValue) const
{
  if(!label.node)
  {
    return FirstValue(label, foundKey, foundValue);
  }
  Node * node = ((Node*)label.node)->next;
  if(node)
  {
    label.node = (int)node;
    foundKey = &node->key;
    foundValue = &node->value;
    return true;
  }
  for(int i = label.head+1; i < tableSize; i++)
  {
    if(heads[i])
    {
      label.head = i;
      label.node = (int)heads[i];
      foundKey = &heads[i]->key;
      foundValue = &heads[i]->value;
      return true;
    }
  }
  return false;
}

template < class KeyType, class ValueType >
inline bool HashTable<KeyType, ValueType>::NextHeadValue(HashLabel& label, const KeyType *& foundKey, const ValueType *& foundValue) const
{
  for(int i = label.node ? label.head+1 : 0; i < tableSize; i++)
  {
    if(heads[i])
    {
      label.head = i;
      label.node = (int)heads[i];
      foundKey = &heads[i]->key;
      foundValue = &heads[i]->value;
      return true;
    }
  }
  return false;
}

template < class KeyType, class ValueType >
inline ValueType * HashTable<KeyType, ValueType>::GetAt(int index, KeyType ** pkey) const
{
  if(index < 0 || index >= count)
  {
    // ASSERT(false);
    if(pkey) *pkey = NULL;
    return NULL;
  }

  int num = 0;
  for(int i = 0; i < tableSize; i++)
  {
    for(Node * node = heads[ i ]; node; node = node->next)
    {
      if(num == index)
      {
        if(pkey) *pkey = &node->key;
        return &node->value;
      }
      num++;
    }
  }
  // ASSERT(false);
  if(pkey) *pkey = NULL;
  return NULL;
}

template < class KeyType, class ValueType >
inline const ValueType & HashTable<KeyType, ValueType>::GetValueAt(int index, KeyType ** pkey) const
{
    const ValueType * pval = GetAt(index, pkey);
    ASSERT(pval);
    return *pval;
}

template < class KeyType, class ValueType >
inline ValueType * HashTable<KeyType, ValueType>::ForEach(IterateFunc f, void * params)
{
  Node * node, * prev, * next;
  for(int i = 0; i < tableSize; i++)
  {
    prev = NULL;
    for(node = heads[i]; node; prev = node, node = next)
    {
      next = node->next;
      int flags = f(node->key, node->value, params);
      if(flags & ITERATE_DELETE_ITEM)
      {
        if(prev)
          prev->next = node->next;
        else
          heads[i] = node->next; 

        count--;
        DDELETE(node);

        if(flags & ITERATE_BREAK)
          return NULL;

        continue;
      }
      if(flags & ITERATE_BREAK)
        return &node->value;
    }
  }
  return NULL;
}

template < class KeyType, class ValueType >
inline void HashTable<KeyType, ValueType>::ToKeys(Vector<KeyType*>& out)
{
  for(int i = 0; i < tableSize; i++)
  {
    for(Node * node = heads[i]; node; node = node->next)
    {
      out.addElement(node->key);
    }
  }
}

template < class KeyType, class ValueType >
inline void HashTable<KeyType, ValueType>::ToValues(Vector<ValueType*>& out)
{
  for(int i = 0; i < tableSize; i++)
  {
    for(Node * node = heads[i]; node; node = node->next)
    {
      out.addElement(&node->value);
    }
  }
}

template < class KeyType, class ValueType >
inline void HashTable<KeyType, ValueType>::ToValues(Vector<ValueType>& out)
{
  for(int i = 0; i < tableSize; i++)
  {
    for(Node * node = heads[i]; node; node = node->next)
    {
      out.addElement(node->value);
    }
  }
}

template < class KeyType, class ValueType >
inline bool HashTable<KeyType, ValueType>::Remove(const KeyType& key)
{
  Node *node, *prev;

  int hash = key.Hash() & (tableSize - 1);
  Node ** head = &heads[ hash ];
  if(*head)
  {
    for(prev = NULL, node = *head; node; prev = node, node = node->next)
    {
      if(node->key.Cmp(key) == 0)
      {
        if(prev)
        {
          prev->next = node->next;
        }
        else
        {
          *head = node->next;
        }

        count--;
        DDELETE(node);
        return true;
      }
    }
  }

  return false;
}

template < class KeyType, class ValueType >
inline bool HashTable<KeyType, ValueType>::Remove(HashLabel& label)
{
  if(label.node)
  {
    Node ** head = &heads[label.head];
    if(*head)
    {
      Node * prev = NULL;
      Node * node = *head;
      for(; node ; prev = node, node = node->next)
      {
        if(node == ((Node*)label.node))
        {
          if(prev)
          {
            prev->next = node->next;
          }
          else
          {
            *head = node->next;
          }
          count--;
          DDELETE(node);
          label.head = 0;
          label.node = 0;
          return true;
        }
      }
    }
  }
  return false;
}

template < class KeyType, class ValueType >
inline void HashTable<KeyType, ValueType>::Grow()
{
  Node ** oldheads = heads;
  int oldTableSize = tableSize;

  tableSize <<= 1;
  count = 0;

  heads = DNEW Node *[ tableSize ];
  __MEMSET(heads, 0, sizeof(*heads) * tableSize);

  for(int i = 0; i < oldTableSize; i++)
  {
    Node * next = oldheads[ i ];
    while(next)
    {
      Node * node = next;
      next = next->next;

      Add(node->key, node->value);

      DDELETE(node);
    }
  }
  DDELETEARR(oldheads);
}

template < class KeyType, class ValueType >
inline void HashTable<KeyType, ValueType>::Clear()
{
  if(count > 0)
  {
    for(int i = 0; i < tableSize; i++)
    {
      Node * next = heads[ i ];
      while(next) {
        Node * node = next;
        next = next->next;
        DDELETE(node);
      }
      heads[ i ] = NULL;
    }
    count = 0;
  }
}

template < class KeyType, class ValueType >
inline void HashTable<KeyType, ValueType>::DeleteContents()
{
  for(int i = 0; i < tableSize; i++)
  {
    Node * next = heads[ i ];
    heads[ i ] = NULL; // allow access over delete process in destructor of value
    while(next)
    {
      Node * node = next;
      next = next->next;
      DDELETE(node->value);
      DDELETE(node);
    }		
  }
  count = 0;
}

#endif /* __HASH_TABLE_H__ */
