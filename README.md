climbing-acyclic-visitor
========================

An acyclic visitor that climbs up a class hierarchy to find a matching method to call.

Usage
------

The code is written such that you only have to include the header file found in the
`include` directory. The relevant classes are in the `cav` namespace, so for demonstration
purposes it's probably the best to just use that entire namespace:
```{c++}
#include "climbing-acyclic-visitor.hpp"
using namespace cav;
```

### Making classes visitable
For a class to be visitable, it must declare a "parent visitable type" `parent_visitable` 
(to which it must be castable, too) and inherit from the `is_visitable` template class,
providing itself as template parameter.

So to declare the root of a hierarchy as being visitable, the following is sufficient:
```{c++}
class root : /* possibly other superclasses */ public is_visitable<root>
{
  public:
    using parent_visitable = base_visitable;
    // ...
};
```
In this case `root` implicitely inherits from its "parent visitable type" through the
`is_visitable` template class, so there is no need to directly derive from `base_visitable`.
The class `root` now has an method `accept` through which it can be fed to a visitor class.

To declare a sub class as being visitable, there is the additional "burden" of explicitely
declaring the accept method to use the correct accept method from the `is_visitable` parent
(and not the one from the "real" parent class):
```{c++}
class sub : public root, public is_visitable<sub>
{
  public:
    using parent_visitable = root;
    void accept(base_visitor & visitor)
    {
      is_visitable<sub>::accept(visitor);
    }
};
```
Now, a visitor may define a special behaviour for the `sub` class. The advantage compared to
a regular acyclic visitor is, that the visitor doesn't NEED to define a special behaviour. In case
no special behaviour is defined, an object of type `sub` is handled the same way as an object of
type `root`.

Further subclassing works analogous.

Of course, one can also define a "regular" sub class of `root`:
```{c++}
class regular_sub : public root
{
  // ... anything ...
};
```
`regular_sub` now inherits the `accept` method from its parent, but a visitor may not define special
behaviour on `regular_sub`.
Further subclassing `regular_sub` is a bit special: One must not declare `regular_sub` as `parent_visitable`
(because it's not a "real" visitable) but instead use the next parent that really is a "visitable":
```{c++}
class sub_regular_sub : public regular_sub, public is_visitable<sub_regular_sub>
{
  public:
    using parent_visitable = root; // Important to choose the right class here!
    void accept(base_visitor & visitor)
    {
      is_visitable<sub_regular_sub>::accept(visitor);
    }
};
```

One additional note here: One can also declare a sub class as being visitable, but "skipping" some
parent visitable classes or start a complete new visitable hierarchy:
```{c++}
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
```

### The other side: Defining a visitor
To define a visitor, one must derive from the template class `can_visit`, giving the class
the visitor is able to visit as template parameter. One must then provide an implementation
of the `visit` method with one parameter (whose type is the same as the template parameter):
```{c++}
class some_visitor : public can_visit<root>,
                     public can_visit<sub_regular_sub>,
                     public can_visit<sub>
{
  public:
    void unknown_visitable(base_visitable & bv)
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
```

### Putting it together
The let a visitor visit a visitable, the visitable must accept the visitor:
```{c++}
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
```
