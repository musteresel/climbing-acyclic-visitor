/** An acyclic visitor that climbs up a class hierarchy to find a matching
 * method to call.
 *
 * https://github.com/musteresel/climbing-acyclic-visitor
 * */
#ifndef _CLIMBING_ACYCLIC_VISITOR_HEADER_
#define _CLIMBING_ACYCLIC_VISITOR_HEADER_ 1
namespace cav
{
  // Forward declaration
  class base_visitable;

  
  /** Marker class to mark everything that can be a visitor.
   *
   * */
  class base_visitor
  {
    public:
      virtual ~base_visitor() {};
      virtual void unknown_visitable(base_visitable *) = 0;
  };


  /** Marker class to mark everything that is able to accept a visitor.
   *
   * */
  class base_visitable
  {
    public:
      virtual ~base_visitable() {};
      virtual void accept(base_visitor &) = 0;
  };


  /** Template class to declare a class as being able to visit a specific
   * class.
   *
   * Inheriting from this class is neccessary to state that the inheriting class
   * is able to visit objects of type Visitable (or any subtype). By deriving
   * from this class, the inheriting class is also virtual inheriting from
   * base_visitor, because everything that can visit something is, by
   * definition, a visitor.
   *
   * The inheriting class must then provide an implementation of the visit
   * method.
   * */
  template<typename Visitable> class can_visit : virtual public base_visitor
  {
    public:
      virtual void visit(Visitable &) = 0;
  };


  // Forward declaration
  template<typename Visitable> class is_visitable;


  namespace backend
  {
    /** Dispatcher class that implements the (static) dispatching to the parent
     * class.
     *
     * This class provides the method to cast something visitable to its
     * parent (which should be visitable, too). This is done using a static_cast
     * to avoid runtime costs and provide compile time error detection.
     * */
    template<typename Parent> class dispatcher
    {
      public:
        template<typename Visitable>
          static void dispatch(Visitable * visitable, base_visitor & visitor)
          {
            using parent_is_visitable = is_visitable<Parent>;
            parent_is_visitable * parent = 
              static_cast<parent_is_visitable *>(visitable);
            parent->is_visitable<Parent>::accept(visitor);
          }
    };


    /** Special implementation of the dispatch class for a base visitable.
     *
     * When dispatching reaches this class, then the given visitor was unable
     * to handle any class in the visitables hierarchy. The visitor must take
     * care of that in its unknown_visitable method (good luck then...).
     * */
    template<> class dispatcher<base_visitable>
    {
      public:
        template<typename Visitable>
          static void dispatch(Visitable * visitable, base_visitor & visitor)
          {
            visitor.unknown_visitable(visitable);
          }
    };
  }


  /** Mark a class as being visitable.
   *
   * Inheriting from this class makes the inheriting class visitable. The
   * template parameter must be set to the inheriting class. This class
   * introduces the accept method which does the first dispatch based on the
   * type of the template parameter.
   * */
  template<typename Visitable>
    class is_visitable : virtual public base_visitable
  {
    public:
      void accept(base_visitor & visitor)
      {
        using can_visit_current = can_visit<Visitable>;
        can_visit_current * interface =
          dynamic_cast<can_visit_current *>(&visitor);
        if (interface)
        {
          interface->visit(static_cast<Visitable &>(*this));
        }
        else
        {
          using parent_visitable = typename Visitable::parent_visitable;
          backend::dispatcher<parent_visitable>::dispatch(
              static_cast<Visitable *>(this), visitor);
        }
      }
  };


  /** "Delete" special implementation of is_visitable for the base_visitable.
   *
   * A base_visitable is - contrary to its name - never visitable.
   * */
  template<> class is_visitable<base_visitable>
  {
  };
}
#endif

