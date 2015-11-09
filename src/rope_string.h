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
		: _p_top(NULL)
	{
	}

	explicit
	rope_string(
		const string& s)
		: _p_top(NULL)
	{
		_p_top = new rope_node_leaf(s);
	}

	rope_string(
		const rope_string& rhs)
		: _p_top(rhs._p_top)
	{
		if (NULL != _p_top) { _p_top->set(); }
	}

	~rope_string()
	{
		if (NULL != _p_top) { _p_top->release() };
	}

	rope_string&
	operator=(
		const rope_string& rhs)
	{
		if (_p_top == rhs._p_top) { return *this; }

		rhs._p_top->set();
		if (NULL != _p_top) { _p_top->release(); }

		_p_top = rhs._p_top;

		return *this;
	}

	rope_string&
	operator=(
		const string& rhs)
	{
		if (NULL != _p_top) { _p_top->release(); }

		_p_top = new rope_node_leaf(rhs);

		return *this;
	}

	// =

	string
	linearized() const
	{
		if (NULL == _p_top){ return string(); }
		if (LEAF == _p_top->_type) { return ((rope_node_leaf*)_p_top)->m_string; }

		string result;
		result.reserve(size());

		if (CONCAT == _p_top->_type) { _linearized_concat((rope_node_concat*)_p_top, result); }

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
		if (LEAF == p->_p_lhs->_type)
		{
			result.insert(result.end(), ((rope_node_leaf*)p->_p_lhs)->m_string.begin(), ((rope_node_leaf*)p->_p_lhs)->m_string.end());
		}
		else if (CONCAT == p->_p_lhs->_type)
		{
			_linearized_concat((rope_node_concat*)p->_p_lhs, result);
		}

		// rhs
		if (LEAF == p->_p_rhs->_type)
		{
			result.insert(result.end(), ((rope_node_leaf*)p->_p_rhs)->m_string.begin(), ((rope_node_leaf*)p->_p_rhs)->m_string.end());
		}
		else if (CONCAT == p->_p_rhs->_type)
		{
			_linearized_concat((rope_node_concat*)p->_p_rhs, result);
		}

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
		if (NULL == _p_top) { return 0 };

		return _p_top->size();
	}

	// =

	rope_string&
	concat(
		const rope_string& rhs)
	{
		rope_node_base* p = new rope_node_concat(_p_top, rhs._p_top);
		if (p == NULL) { return *this; }

		_p_top = p;

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
			: _count(1), _type(NODE_BASE)
		{}

		virtual size_type size() const = 0;

		void set() const{ ++_count; }
		void release() const
		{
			--_count;
			if (0 >= _count){ delete this; }
		}

	protected:

		virtual ~rope_node_base(){}

	private:

		mutable size_type _count;

	public:

		NODE_TYPE _type;

	};

	class rope_node_leaf : public rope_node_base
	{
	public:

		rope_node_leaf( const string& rhs )
			: rope_node_base()
			, m_string(rhs)
		{
			_type = LEAF;
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
			: _size(0)
			, _p_lhs(lhs)
			, _lhs_size(0)
			, _p_rhs(rhs)
			, _rhs_size(0)
		{
			_type = CONCAT;

			if (NULL != _p_lhs) { _p_lhs->set(); _lhs_size = _p_lhs->size(); }
			if (NULL != _p_rhs) { _p_rhs->set(); _rhs_size = _p_rhs->size(); }

			_size = _lhs_size + _rhs_size;
		}

		size_type size() const { return _size; }

	protected:

		~rope_node_concat()
		{
			if (NULL != _p_lhs) { _p_lhs->release(); }
			if (NULL != _p_rhs) { _p_rhs->release(); }
		}

	public:

		size_type _size;

		const rope_node_base* _p_lhs;
		size_type _lhs_size;

		const rope_node_base* _p_rhs;
		size_type _rhs_size;

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

	rope_node_base* _p_top;

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

