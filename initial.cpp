#include <iostream>
#include <typeinfo>

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
			void accept(base_visitor & b) = delete;/*
			{
				std::cerr << "Must not accept a visitable!" << std::endl;
				throw std::bad_cast();
			}*/
	};



	template<typename Node>
		class is_root : public accepting<Node>
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
	class root : public is_root<root>
	{
	};
	class sub : public derive<root,sub>
	{
	};
	class subcomp : public derive<root,subcomp>
	{
	};
	class subsub : public sub
	{
	};
/*	class subsubsub : public subsub, public derive<sub,subsubsub>
	{
	};*/
	class subsubcomp : public derive<subcomp,subsubcomp>
	{
	};

	class printer : public can_visit<subsubcomp>, public can_visit<subcomp>
	{
		public:
			void visit(subsubcomp & r)
			{
				std::cout << "visit as root" << std::endl;
			}
			void visit(subcomp & c)
			{
				std::cout << "visit as subcomp" << std::endl;
			}
	};
}


int main(int argc, char** argv)
{
	using namespace tree2;
	printer p;
	subsubcomp ssc;
	root r;
	sub s;
	subsub ss;
//	subsubsub sss;
	subcomp sc;

	root & node = ss;
	node.accept(p);

	return 0;
}

