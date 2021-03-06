/*
 
      ___           ___           ___     
     /\__\         /\  \         /\__\    
    /::|  |       /::\  \       /:/  /    
   /:|:|  |      /:/\:\  \     /:/  /     
  /:/|:|  |__   /::\~\:\  \   /:/  /  ___ 
 /:/ |:| /\__\ /:/\:\ \:\__\ /:/__/  /\__\
 \/__|:|/:/  / \:\~\:\ \/__/ \:\  \ /:/  /
     |:/:/  /   \:\ \:\__\    \:\  /:/  / 
     |::/  /     \:\ \/__/     \:\/:/  /  
     /:/  /       \:\__\        \::/  /   
     \/__/         \/__/         \/__/    
 
 
Neu, Copyright (c) 2013-2014, Andrometa LLC
All rights reserved.

neu@andrometa.net
http://neu.andrometa.net

Neu can be used freely for commercial purposes. We hope you will find
Neu powerful, useful to make money or otherwise, and fun! If so,
please consider making a donation via PayPal to: neu@andrometa.net

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
 
1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
 
2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
 
3. Neither the name of the copyright holder nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
*/

#include <map>
#include <ostream>
#include <cassert>

#ifndef NEU_N_MAP_H
#define NEU_N_MAP_H

#include <neu/NError.h>

namespace neu{
  
  template<class Key, class T, class Compare = std::less<Key>,
  class Allocator = std::allocator<std::pair<const Key,T> > >
  class NMap{
  public:
    typedef std::map<Key, T, Compare, Allocator> Map_;
    
    typedef typename Map_::iterator iterator;
    typedef typename Map_::const_iterator const_iterator;
    typedef typename Map_::key_type key_type;
    typedef typename Map_::allocator_type allocator_type;
    typedef typename Map_::value_type value_type;
    typedef typename Map_::key_compare key_compare;
    typedef typename Map_::reverse_iterator reverse_iterator;
    typedef typename Map_::const_reverse_iterator const_reverse_iterator;
    typedef typename Map_::value_compare value_compare;
    
    explicit NMap(const Compare& comp=Compare(),
                  const Allocator& allocator=Allocator())
    : map_(comp, allocator){
      
    }
    
    template<class InputIterator>
    NMap(InputIterator first,
         InputIterator last,
         const Compare& comp=Compare(),
         const Allocator& allocator=Allocator())
    : map_(first, last, comp, allocator){
      
    }
    
    NMap(const NMap<Key, T, Compare, Allocator>& x)
    : map_(x.map_){
    }
    
    NMap(NMap&& x)
    : map_(std::move(x.map_)){
      
    }
    
    NMap(NMap&& x, const allocator_type& a)
    : map_(std::move(x.map_), a){
      
    }
    
    NMap(std::initializer_list<value_type> il,
         const key_compare& comp = key_compare())
    : map_(il, comp){
      
    }
    
    NMap(std::initializer_list<value_type> il,
         const key_compare& comp,
         const allocator_type& a)
    : map_(il, comp, a){
      
    }
    
    virtual ~NMap(){
      
    }
    
    const Map_ map() const{
      return map_;
    }
    
    Map_ map(){
      return map_;
    }
    
    iterator begin(){
      return map_.begin();
    }
    
    const_iterator begin() const{
      return map_.begin();
    }
    
    void clear(){
      map_.clear();
    }
    
    size_t count(const key_type& x) const{
      return map_.count(x);
    }
    
    bool empty() const{
      return map_.empty();
    }
    
    iterator end(){
      return map_.end();
    }
    
    const_iterator end() const{
      return map_.end();
    }
    
    std::pair<iterator,iterator>
    equal_range(const key_type& x){
      return map_.equal_range(x);
    }
    
    std::pair<const_iterator,const_iterator>
    equal_range(const key_type& x) const{
      return map_.equal_range(x);
    }
    
    void erase(iterator position){
      map_.erase(position);
    }
    
    size_t erase(const key_type& x){
      return map_.erase(x);
    }
    
    void erase(iterator first, iterator last){
      map_.erase(first, last);
    }
    
    iterator find(const key_type& x){
      return map_.find(x);
    }
    
    bool hasKey(const key_type& x) const{
      return map_.find(x) != map_.end();
    }
    
    const_iterator find(const key_type& x) const{
      return map_.find(x);
    }
    
    allocator_type get_allocator() const{
      return map_.get_allocator();
    }
    
    std::pair<iterator, bool> insert(const value_type& x){
      return map_.insert(x);
    }
    
    iterator insert(iterator position, const value_type& x){
      return map_.insert(position, x);
    }
    
    template <class... Args>
    std::pair<iterator, bool> emplace(Args&&... args){
      return map_.emplace(std::forward<Args>(args)...);
    }
    
    void merge(const NMap<Key,T,Compare,Allocator>& m){
      map_.insert(m.begin(), m.end());
    }
    
    void outerMerge(const NMap<Key,T,Compare,Allocator>& m){
      typename NMap<Key,T,Compare,Allocator>::const_iterator i = m.begin();
      while(i != m.end()){
        typename NMap<Key,T,Compare,Allocator>::iterator j = map_.find(i->first);
        if(j != map_.end()){
          j->second = i->second;
        }
        else{
          map_.insert(std::pair<Key,T>(i->first, i->second));
        }
        ++i;
      }
    }
    
    template<class InputIterator>
    void insert(InputIterator first, InputIterator last){
      map_.insert(first, last);
    }
    
    NMap<Key,T,Compare,Allocator>&
    add(const Key& k, const T& t){
      map_.insert({k, t});
      return *this;
    }
    
    key_compare key_comp() const{
      return map_.key_comp();
    }
    
    iterator lower_bound(const key_type& x){
      return map_.lower_bound(x);
    }
    
    const_iterator lower_bound(const key_type& x) const{
      return map_.lower_bound(x);
    }
    
    size_t max_size() const{
      return map_.max_size();
    }
    
    NMap<Key,T,Compare,Allocator>&
    operator=(const NMap<Key,T,Compare,Allocator>& x){
      map_ = x.map_;
      return *this;
    }
    
    NMap<Key,T,Compare,Allocator>&
    operator=(NMap<Key,T,Compare,Allocator>&& x){
      map_ = std::move(x.map_);
      return *this;
    }
    
    NMap<Key,T,Compare,Allocator>&
    operator=(std::initializer_list<value_type> il){
      map_ = il;
      
      return *this;
    }
    
    T& operator[](const key_type& x){
      return map_[x];
    }
    
    const T& get(const key_type& x, const T& def) const{
      auto itr = map_.find(x);
      return itr == map_.end() ? def : itr->second;
    }
    
    T& get(const key_type& x, T& def){
      auto itr = map_.find(x);
      return itr == map_.end() ? def : itr->second;
    }
    
    reverse_iterator rbegin(){
      return map_.rbegin();
    }
    
    const_reverse_iterator rbegin() const{
      return map_.rbegin();
    }
    
    reverse_iterator rend(){
      return map_.rend();
    }
    
    const_reverse_iterator rend() const{
      return map_.rend();
    }
    
    size_t size() const{
      return map_.size();
    }
    
    void swap(NMap<Key,T,Compare,Allocator>& mp){
      map_.swap(mp);
    }
    
    iterator upper_bound(const key_type& x){
      return map_.upper_bound(x);
    }
    
    const_iterator upper_bound(const key_type& x) const{
      return map_.upper_bound(x);
    }
    
    value_compare value_comp() const{
      return map_.value_comp();
    }
    
    value_compare valueCompare() const{
      return map_.value_comp();
    }
    
    Key keyForValue(const T& value){
      for(iterator itr = map_.begin(), itrEnd = map_.end();
          itr != itrEnd; ++itr){
        if(itr->second == value){
          return itr->first;
        }
      }
      
      NERROR("failed to find key for value");
    }
    
  private:
    Map_ map_;
  };
  
  template<class Key, class T, class Compare, class Allocator>
  bool operator==(const NMap<Key,T,Compare,Allocator>& x,
                  const NMap<Key,T,Compare,Allocator>& y){
    return x.map() == y.map();
  }
  
  template<class Key, class T, class Compare, class Allocator>
  bool operator!=(const NMap<Key,T,Compare,Allocator>& x,
                  const NMap<Key,T,Compare,Allocator>& y){
    return x.map() != y.map();
  }
  
  template<class Key, class T, class Compare, class Allocator>
  bool operator<(const NMap<Key,T,Compare,Allocator>& x,
                 const NMap<Key,T,Compare,Allocator>& y){
    return x.map() < y.map();
  }
  
  template<class Key, class T, class Compare, class Allocator>
  bool operator>(const NMap<Key,T,Compare,Allocator>& x,
                 const NMap<Key,T,Compare,Allocator>& y){
    return x.map() > y.map();
  }
  
  template<class Key, class T, class Compare, class Allocator>
  bool operator<=(const NMap<Key,T,Compare,Allocator>& x,
                  const NMap<Key,T,Compare,Allocator>& y){
    return x.map() <= y.map();
  }
  
  template<class Key, class T, class Compare, class Allocator>
  bool operator>=(const NMap<Key,T,Compare,Allocator>& x,
                  const NMap<Key,T,Compare,Allocator>& y){
    return x.map() >= y.map();
  }
  
  template<typename K, typename T, class C>
  std::ostream& operator<<(std::ostream& ostr, const NMap<K,T,C>& m){
    typename NMap<K,T,C>::const_iterator itr = m.begin();
    ostr << "[";
    while(itr != m.end()){
      if(itr != m.begin()){
        ostr << ", ";
      }
      ostr << itr->first << ":";
      ostr << itr->second;
      ++itr;
    }
    ostr << "]";
    return ostr;
  }
  
} // end namespace neu

#endif // NEU_N_MAP_H
