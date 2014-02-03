#include <iostream>
#include "climbing-acyclic-visitor.hpp"
using namespace cav;

class root : /* possibly other superclasses */ public is_visitable<root>
{
  public:
    using parent_visitable = base_visitable;
    // ...
};

class sub : public root, public is_visitable<sub>
{
  public:
    using parent_visitable = root;
    void accept(base_visitor & visitor)
    {
      is_visitable<sub>::accept(visitor);
    }
};

class regular_sub : public root
{
  // ... anything ...
};

class sub_regular_sub : public regular_sub, public is_visitable<sub_regular_sub>
{
  public:
    using parent_visitable = root; // Important to choose the right class here!
    void accept(base_visitor & visitor)
    {
      is_visitable<sub_regular_sub>::accept(visitor);
    }
};

class subsub : public sub, public is_visitable<subsub>
{
  public:
    using parent_visitable = root; // Skip sub, i.e. don't let a visitor threat this as a sub.
    void accept(base_visitor & visitor)
    {
      is_visitable<subsub>::accept(visitor);
    }
};
class subsub_new_hierarchy : public sub, public is_visitable<subsub_new_hierarchy>
{
  public:
    using parent_visitable = base_visitable; // Threat this as the root of a visitable hierarchy.
    void accept(base_visitor & visitor)
    {
      // But must resolve ambiguities!
      is_visitable<subsub_new_hierarchy>::accept(visitor);
    }
};

class some_visitor : public can_visit<root>,
                     public can_visit<sub_regular_sub>,
                     public can_visit<sub>
{
  public:
    void unknown_visitable(base_visitable * bv)
    {
      // handle it somehow
      std::cout << "unknown" << std::endl;
    }
    void visit(root & r)
    {
      // do something ...
      std::cout << "Is root" << std::endl;
    }
    void visit(sub & s)
    {
      // do something ...
      std::cout << "Is sub" << std::endl;
    }
    void visit(sub_regular_sub & srs)
    {
      // do something different ...
      std::cout << "Is sub_regular_sub" << std::endl;
    }
};

int main(int arg, char** argv)
{
  some_visitor my_visitor;

  root r;
  sub s;
  subsub subs;
  regular_sub rs;
  sub_regular_sub subrs;
  subsub_new_hierarchy subs_nh;


  r.accept(my_visitor);        // Is root
  s.accept(my_visitor);        // Is sub
  subs.accept(my_visitor);     // Is root
  rs.accept(my_visitor);       // Is root
  subrs.accept(my_visitor);    // Is sub_regular_sub
  subs_nh.accept(my_visitor);  // unknown
  return 0;
}
