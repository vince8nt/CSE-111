// $Id: listmap.tcc,v 1.15 2019-10-30 12:44:53-07 - - $

#include "listmap.h"
#include "debug.h"


//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//---------------------------------------------- delete everything
// listmap::~listmap()
// 
template <typename key_t, typename mapped_t, class less_t>
listmap<key_t,mapped_t,less_t>::~listmap() {
   DEBUGF ('l', reinterpret_cast<const void*> (this));
   while (begin() != end()) erase(begin());
   anchor_.next = anchor_.prev = NULL;
}

//----------------------------------------------- insert in order
// iterator listmap::insert (const value_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::insert (const value_type& pair) {
   DEBUGF ('l', &pair << "->" << pair);
   listmap<key_t,mapped_t,less_t>::iterator locate = begin();
   while (locate != end()) {
   	if (less(pair.first, locate->first)) break;
   	++locate;
   }
   node* toInsert = new node(locate.where, locate.where->prev, pair);
   toInsert->next->prev = toInsert;
   toInsert->prev->next = toInsert;
   return locate;
}

//----------------------------------------------- find the pair
// listmap::find(const key_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::find (const key_type& that) {
   DEBUGF ('l', that);
   listmap<key_t,mapped_t,less_t>::iterator locate = begin();
   while (locate != end()) {
   	if (!less(that, locate->first))
   		if (!less(locate->first, that)) break;
   	++locate;
   }
   return locate;
}

//--------------------------------------------- delete the node
// iterator listmap::erase (iterator position)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::erase (iterator position) {
   DEBUGF ('l', &*position);
   position.where->next->prev = position.where->prev;
   position.where->prev->next = position.where->next;
   delete position.where;
   position.where = NULL;
   return position;
}









