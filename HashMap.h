#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H
 
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
 
#include <iostream>
 
namespace aisdi
{
 
template <typename KeyType, typename ValueType>
class HashMap
{
public:
  using key_type = KeyType;
  using mapped_type = ValueType;
  using value_type = std::pair<const key_type, mapped_type>;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = const value_type&;
 
  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;
 
private:
  const size_type TABLE_SIZE;
  struct HashNode
  {
    value_type data;
    HashNode *next;
    HashNode *prev;
    HashNode(key_type key, mapped_type mapped) : data(std::make_pair( key, mapped )), next(nullptr), prev(nullptr) {}
    ~HashNode() { delete next; }
  };
  HashNode **table;
  size_type count;
 
public:
  HashMap(size_type tableSize = 1000) : TABLE_SIZE(tableSize), table(nullptr), count(0)
  {
    table = new HashNode* [TABLE_SIZE]();
    for (size_type i = 0; i < TABLE_SIZE; ++i)
      table[i] = nullptr;
  }
 
  HashMap(std::initializer_list<value_type> list) : HashMap()
  {
    for (auto it = list.begin(); it != list.end(); ++it)
      operator[]((*it).first) = (*it).second;
  }
 
  HashMap(const HashMap& other) : HashMap()
  {
    *this = other;
  }
 
  HashMap(HashMap&& other) : HashMap()
  {
    *this = std::move(other);
  }
 
  ~HashMap()
  {
    erase();
    delete[] table;
  }
 
  HashMap& operator=(const HashMap& other)
  {
    if(this != &other) {
      erase();
      for (auto it = other.begin(); it != other.end(); ++it)
        operator[]((*it).first) = (*it).second;
    }
    return *this;
  }
 
  HashMap& operator=(HashMap&& other)
  {
    if(this != &other) {
      erase();
 
      auto old = table;
 
      table = other.table;
      count = other.count;
 
      other.table = old;
      other.count = 0;
    }
    return *this;
  }
 
  bool isEmpty() const
  {
    return !count;
  }
 
  mapped_type& operator[](const key_type& key)
  {
    size_type hashKey = hashFunction(key);
    HashNode *node = findNode(key);
 
    if(node == nullptr) {
      node = new HashNode(key, mapped_type());
      ++count;
 
      if( table[ hashKey ] == nullptr ) {
        table[hashKey] = node;
      }
      else {
        HashNode *tmp = table[hashKey];
        while(tmp->next != nullptr)
          tmp = tmp->next;
        tmp->next = node;
        node->prev = tmp;
      }
    }
    return node->data.second;
  }
 
  const mapped_type& valueOf(const key_type& key) const
  {
    HashNode* node = findNode(key);
    if(node == nullptr)
      throw std::out_of_range("valueOf");
    return node->data.second;
  }
 
  mapped_type& valueOf(const key_type& key)
  {
    HashNode* node = findNode(key);
    if(node == nullptr)
      throw std::out_of_range("valueOf");
    return node->data.second;
  }
 
  const_iterator find(const key_type& key) const
  {
    return const_iterator( this, findNode(key), hashFunction(key) );
  }
 
  iterator find(const key_type& key)
  {
    return iterator( this, findNode(key), hashFunction(key) );
  }
 
  void remove(const key_type& key)
  {
    remove(find(key));
  }
 
  void remove(const const_iterator& it)
  {
    if(this != it.myMap || it == end())
      throw std::out_of_range("remove");
    remove(it.myNode, it.myNode->data.first);
  }
 
  size_type getSize() const
  {
    return count;
  }
 
  bool operator==(const HashMap& other) const
  {
    if(count != other.count)
      return false;
 
    for(auto it = begin(), it2 = other.begin() ; it!=end(); ++it, ++it2) {
      if(*it != *it2)
        return false;
    }
    return true;
  }
 
  bool operator!=(const HashMap& other) const
  {
    return !(*this == other);
  }
 
  iterator begin()
  {
    auto node_index = findFirstNode();
    return iterator (this, node_index.first, node_index.second);
  }
 
  iterator end()
  {
    iterator it(this);
    return it;
  }
 
  const_iterator cbegin() const
  {
    auto node_index = findFirstNode();
    return const_iterator (this, node_index.first, node_index.second);
  }
 
  const_iterator cend() const
  {
    const_iterator it(this);
    return it;
  }
 
  const_iterator begin() const
  {
    return cbegin();
  }
 
  const_iterator end() const
  {
    return cend();
  }
 
private:
  void erase()
  {
    if(count) {
      for(size_type i=0; i<TABLE_SIZE; ++i) {
        delete table[i];
        table[i] = nullptr;
      }
    }
    count = 0;
  }
 
  void remove(HashNode* node, const key_type& key)
  {
    if(node->prev == nullptr)
      table[hashFunction(key)] = node->next;
    else
      node->prev->next = node->next;
 
    if(node->next != nullptr)
      node->next->prev = node->prev;
 
    node->next = nullptr;
    delete node;
    count--;
  }
 
  size_type hashFunction(const key_type& key) const
  {
    return std::hash<key_type>()(key) % TABLE_SIZE;
  }
 
  HashNode* findNode(const key_type& key) const
  {
    HashNode *node = table[ hashFunction(key) ];
    while(node != nullptr) {
      if(node->data.first == key) {
        return node;
      }
      node = node->next;
    }
    return node;
  }
 
  std::pair<HashNode*, size_type> findFirstNode() const
  {
    size_type index = 0;
 
    while(index < TABLE_SIZE && table[index] == nullptr)
      ++index;
 
    HashNode *node = nullptr;
    if(index < TABLE_SIZE)
      node = table[index];
 
    return std::make_pair(node, index);
  }
 
};
 
template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename HashMap::value_type;
  using pointer = const typename HashMap::value_type*;
 
private:
  const HashMap *myMap;
  HashNode *myNode;
  size_type index;
  friend void HashMap<KeyType, ValueType>::remove(const const_iterator&);
 
public:
  explicit ConstIterator(const HashMap *myMap = nullptr, HashNode *myNode = nullptr, size_type index = 0) : myMap(myMap), myNode(myNode), index(index)
  {
    if(myNode == nullptr && myMap != nullptr)
      this->index = myMap->TABLE_SIZE;
  }
 
  ConstIterator(const ConstIterator& other) : ConstIterator(other.myMap, other.myNode, other.index)
  {}
 
  ConstIterator& operator++()
  {
    if(myMap == nullptr || myNode == nullptr)
      throw std::out_of_range("operator++");
    else if(myNode->next != nullptr){
      myNode = myNode->next;
    }
    else {
      index++;
      while( index < myMap->TABLE_SIZE && myMap->table[index] == nullptr )
        index++;
 
      if(index >= myMap->TABLE_SIZE )
        myNode = nullptr;
      else
        myNode = myMap->table[index];
    }
    return *this;
  }
 
  ConstIterator operator++(int)
  {
    auto result = *this;
    ConstIterator::operator++();
    return result;
  }
 
  ConstIterator& operator--()
  {
    if(myMap == nullptr)
      throw std::out_of_range("operator--");
    else if(myNode == nullptr || myNode == myMap->table[index]) {
      index--;
      while( index >0 && myMap->table[index] == nullptr )
        index--;
 
      if(index == 0 && myMap->table[index] == nullptr)
        throw std::out_of_range("operator--");
 
      myNode = myMap->table[index];
      while (myNode->next != nullptr)
        myNode = myNode->next;
    }
    else{
      myNode = myNode->prev;
    }
 
    return *this;
  }
 
  ConstIterator operator--(int)
  {
    auto result = *this;
    ConstIterator::operator--();
    return result;
  }
 
  reference operator*() const
  {
    if(myNode == nullptr)
      throw std::out_of_range("operator*");
    return myNode->data;
  }
 
  pointer operator->() const
  {
    return &this->operator*();
  }
 
  bool operator==(const ConstIterator& other) const
  {
    return myMap == other.myMap && myNode == other.myNode && index == other.index;
  }
 
  bool operator!=(const ConstIterator& other) const
  {
    return !(*this == other);
  }
};
 
template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::reference;
  using pointer = typename HashMap::value_type*;
 
  explicit Iterator(HashMap *myMap = nullptr, HashNode *myNode = nullptr, size_type index = 0) : ConstIterator(myMap, myNode, index)
  {}
 
  Iterator(const ConstIterator& other)
    : ConstIterator(other)
  {}
 
  Iterator& operator++()
  {
    ConstIterator::operator++();
    return *this;
  }
 
  Iterator operator++(int)
  {
    auto result = *this;
    ConstIterator::operator++();
    return result;
  }
 
  Iterator& operator--()
  {
    ConstIterator::operator--();
    return *this;
  }
 
  Iterator operator--(int)
  {
    auto result = *this;
    ConstIterator::operator--();
    return result;
  }
 
  pointer operator->() const
  {
    return &this->operator*();
  }
 
  reference operator*() const
  {
    // ugly cast, yet reduces code duplication.
    return const_cast<reference>(ConstIterator::operator*());
  }
};
 
}
 
#endif /* AISDI_MAPS_HASHMAP_H */
