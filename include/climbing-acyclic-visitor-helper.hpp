#ifndef _CLIMBING_ACYCLIC_VISITOR_HELPER_HEADER_
#define _CLIMBING_ACYCLIC_VISITOR_HELPER_HEADER_ 1
#include "climbing-acyclic-visitor.hpp"


namespace cav
{
  template<typename DirectParent,
    typename Class,
    typename VisitableParent = DirectParent
      >
      class derive : public DirectParent, public is_visitable<Class>
      {
        public:
          using parent_visitable = VisitableParent;
          void accept(base_visitor & visitor)
          {
            is_visitable<Class>::accept(visitor);
          }
      };
}


#endif

