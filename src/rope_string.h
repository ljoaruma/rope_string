#ifndef CXX_ROPE_STRING_H
#define CXX_ROPE_STRING_H

#include <iostream>
#include <string>

template< class CharType, class Traits = std::char_traits< CharType >, class Allocator = std::allocator< CharType > >
class rope_string
{
public:

	typedef CharType value_type;

	typedef Allocator allocator_type;
	typedef Traits char_traits_type;

	typedef CharType* pointer;
	typedef const CharType* const_pointer;

	typedef value_type& reference;
	typedef const value_type& const_reference;

	typedef ptrdiff_t difference_type;
	typedef size_t size_type;

	typedef
	std::basic_string< value_type, char_traits_type, allocator_type >
	string;

private:

	class rope_node_base;
	class rope_node_leaf;
	class rope_node_substr;
	class rope_node_concat;

public:

	class iterator;
	class const_iterator;

	rope_string()
		: m_ptop(NULL)
	{
	}

	explicit
	rope_string(
		const string& s)
		: m_ptop(NULL)
	{
		m_ptop = new rope_node_leaf(s);
	}

	rope_string(
		const rope_string& rhs)
		: m_ptop(rhs.m_ptop)
	{
		if (NULL != m_ptop) m_ptop->set();
	}

	~rope_string()
	{
		if (NULL != m_ptop) m_ptop->release();
	}

	rope_string&
	operator=(
		const rope_string& rhs)
	{
		if (m_ptop == rhs.m_ptop) { return *this; }

		rhs.m_ptop->set();
		if (NULL != m_ptop) m_ptop->release();

		m_ptop = rhs.m_ptop;

		return *this;
	}

	rope_string&
	operator=(
		const string& rhs)
	{
		if (NULL != m_ptop) { m_ptop->release(); }

		m_ptop = new rope_node_leaf(rhs);

		return *this;
	}

	// =

	string
	linearized() const
	{
		if (NULL == m_ptop){ return string(); }
		if (LEAF == m_ptop->m_type) { return ((rope_node_leaf*)m_ptop)->m_string; }

		string result;
		result.reserve(size());

		if (CONCAT == m_ptop->m_type) { _linearized_concat((rope_node_concat*)m_ptop, result); }

		return result;
	}

private:

	static
	void
	_linearized_concat(
		rope_node_concat* p,
		string& result)
	{
		// lhs
		if (LEAF == p->m_plhs->m_type)
		{ result.insert(result.end(), ((rope_node_leaf*)p->m_plhs)->m_string.begin(), ((rope_node_leaf*)p->m_plhs)->m_string.end()); }
		else if (CONCAT == p->m_plhs->m_type)
		{ _linearized_concat((rope_node_concat*)p->m_plhs, result); }

		// rhs
		if (LEAF == p->m_prhs->m_type)
		{ result.insert(result.end(), ((rope_node_leaf*)p->m_prhs)->m_string.begin(), ((rope_node_leaf*)p->m_prhs)->m_string.end()); }
		else if (CONCAT == p->m_prhs->m_type)
		{ _linearized_concat((rope_node_concat*)p->m_prhs, result); }

	}

public:

	const_reference
	at(
		size_t i) const
	{
		return value_type();
	}

	const_reference
	operator[](
		const size_type i) const
	{
		return value_type();
	}

	reference
	at(
		size_t i)
	{
		return value_type();
	}

	reference
	operator[](
		const size_type i)
	{
		return value_type();
	}

	size_type
	size() const
	{
		if (NULL == m_ptop) return 0;

		return m_ptop->size();
	}

	// =

	rope_string&
	concat(
		const rope_string& rhs)
	{
		rope_node_base* p = new rope_node_concat(m_ptop, rhs.m_ptop);
		if (p == NULL) { return *this; }

		m_ptop = p;

		return *this;
	}

	// =

	const_iterator
	begin() const
	{
		return const_iterator();
	}

	iterator
	begin()
	{
		return iterator();
	}

	const_iterator
	end() const
	{
		return const_iterator();
	}

	iterator
	end()
	{
		return iterator();
	}

	// =

	enum NODE_TYPE
	{
		NODE_BASE,
		LEAF,
		SUBSTR,
		CONCAT,
	};

	// string = char string
	// rope_string = [LEAF | SUBSTR | CONCAT]
	// LEAF = string
	// SUBSTR = LEAF
	// CONCAT = [LEAF | SUBSTR | CONCAT]
	//
	// e.g
	//
	// OK rope_string -> LEAF
	// OK rope_string -> SUBSTR -> LEAF
	// OK rope_string -> CONCAT -> LEAF
	// OK rope_string -> CONCAT -> SUBSTR -> LEAF
	// NG rope_string -> CONCAT -> SUBSTR -> CONCAT

	class rope_node_base
	{
	public:

		rope_node_base()
			: m_count(1), m_type(NODE_BASE)
		{}

		virtual size_type size() const = 0;

		void set() const{ ++m_count; }
		void release() const{
			--m_count;
			if (0 >= m_count){delete this;}
		}

	protected:

		virtual ~rope_node_base(){}

	private:

		mutable size_type m_count;

	public:

		NODE_TYPE m_type;

	};

	class rope_node_leaf : public rope_node_base
	{
	public:

		rope_node_leaf( const string& rhs )
			: rope_node_base()
			, m_string(rhs)
		{
			m_type = LEAF;
		}

		size_type size() const { return m_string.size(); }

	private:

		virtual ~rope_node_leaf(){}

	public:

		const string m_string;

	};

	class rope_node_substr : public rope_node_base
	{
	};

	class rope_node_concat : public rope_node_base
	{
	public:

		rope_node_concat(
			const rope_node_base* lhs,
			const rope_node_base* rhs)
			: m_size(0)
			, m_plhs(lhs)
			, lhs_size(0)
			, m_prhs(rhs)
			, rhs_size(0)
		{
			m_type = CONCAT;

			if (NULL != m_plhs) { m_plhs->set(); lhs_size = m_plhs->size(); }
			if (NULL != m_prhs) { m_prhs->set(); rhs_size = m_prhs->size(); }

			m_size = lhs_size + rhs_size;
		}

		size_type size() const { return m_size; }

	protected:

		~rope_node_concat()
		{
			if (NULL != m_plhs) { m_plhs->release(); }
			if (NULL != m_prhs) { m_prhs->release(); }
		}

	public:

		size_type m_size;

		const rope_node_base* m_plhs;
		size_type lhs_size;

		const rope_node_base* m_prhs;
		size_type rhs_size;

	};

public:

	class const_iterator
	{
	public:

		const_iterator()
		{
		}

		const_iterator(
			const const_iterator& rhs)
		{
		}

		~const_iterator()
		{
		}

		const_iterator&
		operator=(
			const const_iterator& rhs)
		{
			return *this;
		}

		// = operator

		const_iterator&
		operator++()
		{
			return *this;
		}

		const_iterator
		operator++(int)
		{
			return *this;
		}

		const_iterator&
		operator--()
		{
			return *this;
		}

		const_iterator
		operator--(int)
		{
			return *this;
		}

		// = reference

		const_reference
		operator*() const
		{
			return *this;
		}

	};

	class iterator
		: public const_iterator
	{

		iterator()
		{
		}

		iterator(
			const iterator& rhs)
		{
		}

		~iterator()
		{
		}

		iterator&
		operator=(
			const iterator& rhs)
		{
			return *this;
		}

		// = operator

		iterator&
		operator++()
		{
			return *this;
		}

		iterator
		operator++(int)
		{
			return *this;
		}

		iterator&
		operator--()
		{
			return *this;
		}

		iterator
		operator--(int)
		{
			return *this;
		}

		// = reference

		reference
		operator*() const
		{
			return *this;
		}

	};

private:

	rope_node_base* m_ptop;

};

template< class CharType, class Traits, class Allocator >
rope_string< CharType, Traits, Allocator >
operator+(
	const rope_string< CharType, Traits, Allocator >& lhs,
	const rope_string< CharType, Traits, Allocator >& rhs);

template< class CharType, class Traits, class Allocator >
rope_string< CharType, Traits, Allocator >
sub_string(
	const rope_string< CharType, Traits, Allocator >& s,
	const typename rope_string< CharType, Traits, Allocator >::size_type begin,
	const typename rope_string< CharType, Traits, Allocator >::size_type end);

template< class CharType, class Traits, class Allocator >
rope_string< CharType, Traits, Allocator >
concat(
	const typename rope_string< CharType, Traits, Allocator >& lhs,
	const typename rope_string< CharType, Traits, Allocator >& rhs)
{
	typedef rope_string< CharType, Traits, Allocator > S_;

	return S_();
}


#endif // CXX_ROPE_STRING_H

