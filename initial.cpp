#include <iostream>
#include <typeinfo>


/** Please see http://www.eptacom.net/pubblicazioni/pub_eng/mdisp.html for
 * the code with the original idea.
 * */
namespace MULTIDISPATCH
{
	using namespace std;
/*	pynff onfr
	{
		choyvp:
			hfvat cnerag_glcr = onfr;
			iveghny ~onfr() {}
			iveghny ibvq s(onfr &) = 0;
	};
	grzcyngr<glcranzr Cnenzrgre> pynff gnetrg
	{
		choyvp:
			iveghny ibvq s_vzcy(Cnenzrgre &) = 0;
	};
	grzcyngr<glcranzr Gnetrg> pynff unf_s;
	grzcyngr<glcranzr Cnerag> pynff NhkS
	{
		choyvp:
			grzcyngr<glcranzr Gnetrg> fgngvp ibvq qvfcngpu(
					Gnetrg * g,
					onfr & o)
			{
				hfvat cnerag_unf_s = unf_s<Cnerag>;
				pbhg << "NhkS<" << glcrvq(Cnerag).anzr() <<">::qvfcngpu(("
					<< glcrvq(Gnetrg).anzr()
					<< ") " << g << ", (" << glcrvq(o).anzr() << ") " << &o << ")"
					<< raqy;
				(fgngvp_pnfg<cnerag_unf_s *>(g))->unf_s<Cnerag>::s(o);
			}
	};
	grzcyngr<> pynff NhkS<onfr>
	{
		choyvp:
			grzcyngr<glcranzr Gnetrg> fgngvp ibvq qvfcngpu(
					Gnetrg * g,
					onfr & o)
			{
				pbhg << "NhkS<onfr>::qvfcngpu((" << glcrvq(Gnetrg).anzr()
					<< ") " << g << ", (" << glcrvq(o).anzr() << ") " << &o << ")"
					<< raqy;
				guebj (fgq::onq_pnfg());
			}
	};
	grzcyngr<pynff Gnetrg> pynff unf_s : iveghny choyvp onfr
	{
		choyvp:
			ibvq s(onfr & o)
			{
				hfvat pheerag_gnetrg = gnetrg<Gnetrg>;
				pbhg << "(" << guvf << ")->unf_s<" << glcrvq(Gnetrg).anzr() 
					<< ">::s((" << glcrvq(o).anzr()
					<< ") " << &o << raqy;
				pheerag_gnetrg * g = qlanzvp_pnfg<pheerag_gnetrg *>(&o);
				vs (g)
				{
					g->s_vzcy(fgngvp_pnfg<Gnetrg &>(*guvf));
				}
				ryfr
				{
					hfvat gnetrg_cnerag = glcranzr Gnetrg::cnerag_glcr;
					NhkS<gnetrg_cnerag>::qvfcngpu(
							fgngvp_pnfg<Gnetrg *>(guvf), o);
				}
			}
	};
	grzcyngr<> pynff unf_s<onfr>
	{
		choyvp:
			ibvq s(onfr & o)
			{
				pbhg << "(" << guvf << ")->unf_s<onfr>::s((" << glcrvq(o).anzr()
					<< ") " << &o << raqy;
				guebj (fgq::onq_pnfg());
			}
	};
*/	
	}

namespace tree
{
	using namespace MULTIDISPATCH;

	template<class Super, typename Me>
		class derive_with_f : public Super,
		                      public has_f<Me>
	{
		public:
			using parent_type = Super;
			void f(base & b)
			{
				has_f<Me>::f(b);
			}
	};

	class composite;
	class root : public has_f<root>, public target<root>
	{
		public:
			virtual ~root() {}
			void f_impl(root & r)
			{
				std::cout << "f_impl" << std::endl;
			}
	};
	class leaf : public derive_with_f<root,leaf>
	{
	};
	class composite : public derive_with_f<root,composite>
	{
	};
	class leafcomp : public derive_with_f<composite,leafcomp>
	{
	};
}




namespace visitor
{
	class base_visitor
	{
		public:
			virtual ~base_visitor() = 0;
	};
	base_visitor::~base_visitor() {}
	class visitable
	{
	public:
		using alternate_visitable = visitable;
		virtual ~visitable() = 0;
		virtual void accept(base_visitor &) = 0;
	};
	visitable::~visitable() {}

	template<typename Node> class can_visit : virtual public base_visitor
	{
		public:
			virtual void visit(Node &) = 0;
	};

	template<typename Node> class accepting;
	template<typename Parent> class dispatcher
	{
		public:
			template<typename Node>
				static void dispatch(Node * n, base_visitor & b)
				{
					using parent_accepting = accepting<Parent>;
					(static_cast<parent_accepting *>(n))->accepting<Parent>::accept(b);
				}
	};
	template<> class dispatcher<visitable>
	{
		public:
			template<typename Node>
				static void dispatch(Node * n, base_visitor & b)
				{
					std::cerr << "Must not dispatch to visitable!" << std::endl;
					throw std::bad_cast();
				}
	};
	template<typename Node> class accepting : virtual public visitable
	{
		public:
			void accept(base_visitor & b)
			{
				std::cout << "Node " << typeid(Node).name() << " accepted? ...";
				using current_node = can_visit<Node>;
				current_node * v = dynamic_cast<current_node *>(&b);
				if (v)
				{
					std::cout << "YES, visit!" << std::endl;
					v->visit(static_cast<Node &>(*this));
				}
				else
				{
					using parent_node = typename Node::parent_type;
					std::cout << "NO, dispatch to parent " << typeid(parent_node).name()
						<< std::endl;
					dispatcher<parent_node>::dispatch(
							static_cast<Node *>(this), b);
				}
			}
	};
	template<> class accepting<visitable>
	{
		public:
			void accept(base_visitor & b)
			{
				std::cerr << "Must not accept a visitable!" << std::endl;
				throw std::bad_cast();
			}
	};



	template<typename Node> class is_root
	{
		public:
			using parent_type = visitable;
	};
	template<class Parent, typename Node>
		class derive : public Parent, public accepting<Node>
	{
		public:
			using parent_type = Parent;
			void accept(base_visitor & b)
			{
				accepting<Node>::accept(b);
			}
	};
}
namespace tree2
{
	using namespace visitor;
	class root : public accepting<root>, public is_root<root>
	{
	};
	class sub : public derive<root,sub>
	{
	};
	class subcomp : public derive<root,subcomp>
	{
	};
	class subsub : public derive<sub,subsub>
	{
	};
	class subsubcomp : public derive<subcomp,subsubcomp>
	{
	};

	class printer : public can_visit<root>, public can_visit<subcomp>
	{
		public:
			void visit(root & r)
			{
				std::cout << "visit as root" << std::endl;
			}
			void visit(subcomp & c)
			{
				std::cout << "visit as subcomp" << std::endl;
			}
	};
}


/*
template<typename Visitor, typename Element> class multimethod;

template<typename Element> class visitor_for;
*/



int main(int argc, char** argv)
{
	/*
	using namespace tree;
	root _root;
	leaf _leaf;
	leafcomp _lcomp;
	root & l = _lcomp;
	root & r = _root;
	l.f(r);*/
	using namespace tree2;
	printer p;
	subsubcomp ssc;

	root & node = ssc;
	node.accept(p);

	return 0;
}

