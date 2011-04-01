#ifndef QANY_H
#define QANY_H
#define TEST_MYOPT 0
#if TEST_MYOPT
#define QANY_STDCPP 1
#if QANY_STDCPP
#include <typeinfo>
#else
#include <qvariant.h>
#endif

class QAny
{
public:
	QAny() :content(0) {}

	template<typename ValueType> QAny(const ValueType& value)
		:content(new QHolder<ValueType>(value))
	{}

	QAny(const QAny& other)
		:content(other.content ?other.content->clone():0)
	{}

	~QAny() {
		delete content;
	}

	template<typename ValueType>
	QAny& operator=(const ValueType& other)
	{
		content=QAny(other).content;
		return *this;
	}

	QAny& operator=(QAny that)
	{
		content=that.content;
		return *this;
	}

	bool empty() const { return !content;}
	const std::type_info & type() const {
		return content ? content->type() : typeid(void);
	}


	class QHolderBase {
	public:
		virtual ~QHolderBase(){}

		virtual const std::type_info & type() const=0;
		virtual QHolderBase* clone() const=0;

	};

	template<typename ValueType> class QHolder :public QHolderBase {
	public:
		QHolder(const ValueType& value)
			:held(value)
		{}
		~QHolder(){}

		const std::type_info & type() const
		{
			return typeid(ValueType);
		}

		virtual QHolder* clone() const
		{
			return new QHolder(held);
		}

		ValueType held;

	private:
		QHolder& operator=(const QHolder&);

	};

private:
	template<typename ValueType> friend ValueType* qany_cast(QAny *);
	QHolderBase *content;

};

template<typename ValueType>
ValueType* qany_cast(QAny *that) {
	return that->type()==typeid(ValueType) ? &static_cast<QAny::QHolder<ValueType> *>(that->content)->held :0;
}

template<typename ValueType>
inline const ValueType* qany_cast(const QAny* that) {
	return qany_cast<ValueType>(const_cast<QAny*>(that));
}

template<typename ValueType>
ValueType qany_cast(QAny& that) {
	return *qany_cast<ValueType>(&that);
}

template<typename ValueType>
inline ValueType qany_cast(const QAny& that) {
	return qany_cast<const ValueType&>(const_cast<QAny&>(that));
}
#endif //TEST_MYOPT
#endif // QANY_H
