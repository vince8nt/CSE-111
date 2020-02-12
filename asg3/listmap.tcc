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
}

//----------------------------------------------- insert in order
// iterator listmap::insert (const value_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::insert (const value_type& pair) {
   DEBUGF ('l', &pair << "->" << pair);
   return iterator();
}

//----------------------------------------------- find the pair
// listmap::find(const key_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::find (const key_type& that) {
   DEBUGF ('l', that);
   return iterator();
}

//--------------------------------------------- delete the node
// iterator listmap::erase (iterator position)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::erase (iterator position) {
   DEBUGF ('l', &*position);
   return iterator();
}


//
/////////////////////////////////////////////////////////////////
// Operations on listmap iterator
/////////////////////////////////////////////////////////////////
//

//---------------------------------------------- '*' derefrence
//
//
template <typename key_t, typename mapped_t, class less_t>

//-------------------------------------- '->' dereference and get
//
//
template <typename key_t, typename mapped_t, class less_t>

//------------------------------------------ '++'
//
// - completed in the header file

//------------------------------------------ '--'
//
// - completed in the header file

//----------------------------------------------------erase
//
// - already done in the listmap operations

//------------------------------------------ '=='
//
//
template <typename key_t, typename mapped_t, class less_t>

//------------------------------------------ '!='
//
//
template <typename key_t, typename mapped_t, class less_t>





