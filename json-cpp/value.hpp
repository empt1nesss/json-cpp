#ifndef SOURCE_VALUE_HPP
#define SOURCE_VALUE_HPP


#include "json.hpp"

#include <functional>


class Json::Value
{
public:

  enum ERR
  {
    NotFound,
    NotList,
    NotStruct,
    WrongType
  };


  Value(const Value &val);

  Value();
  Value(bool                                   val);
  Value(const char                            *val);
  Value(const std::string                     &val);
  Value(const wchar_t                         *val);
  Value(const std::wstring                    &val);
  Value(const ListType                        &val);
  Value(const std::initializer_list<Value>    &val);
  Value(const StructType                      &val);
  Value(const std::initializer_list<Property> &val);

  template <
    typename T,
    typename = std::enable_if_t<
      std::is_integral_v<T>       ||
      std::is_floating_point_v<T>
    >
  >
  Value(T val)
  {
    if (std::is_integral_v<T>) {
      m_type  = Int;
      m_value = new int64_t(val);
    }
    else {
      m_type  = Float;
      m_value = new double(val);
    }
  }

  template <typename It>
  Value(It it_first, It it_last);

  template <typename It, typename T>
  Value(It it_first, It it_last, const std::function<Value(T &val)> &to_value);

  template <typename It, typename T>
  Value(It it_first, It it_last, const std::function<Property(T &val)> &to_prop);

  ~Value();


  bool Contains(const std::wstring &prop_name) const;

  ValueType GetType() const { return m_type; }

  bool         GetBool   () const;
  int64_t      GetInt    () const;
  double       GetFloat  () const;
  std::string  GetString () const;
  std::wstring GetStringW() const;
  ListType     GetList   () const;
  StructType   GetStruct () const;

  void RemoveProperty(const std::string  &name);
  void RemoveProperty(const std::wstring &name);


  Value& operator=(const Value &val);

  template <typename T>
  Value& operator=(T val)
  {
    return *this = Value(val);
  }

  Value&        operator[] (const std::string  &prop_name);
  const Value&  operator[] (const std::string  &prop_name) const;
  Value&        operator[] (const std::wstring &prop_name);
  const Value&  operator[] (const std::wstring &prop_name) const;

  Value&        operator[] (size_t i);
  const Value&  operator[] (size_t i) const;
  
private:

  ValueType m_type;
  void*     m_value;


  void clear();


  friend std::wstring Json::serialize_value(
    const Value        &val
  );
  friend void         Json::deserialize_value(
    const std::wstring &json_str, Value *val
  );

};

template <typename It>
Json::Value::Value(It it_first, It it_last)
{
  using value_type = typename std::iterator_traits<It>::value_type;
  static_assert(
    std::is_same_v<value_type, Value>    ||
    std::is_same_v<value_type, Property>,
    "Iterator value type must be "
    "Json::Value or Json::Property"
  );

  if constexpr (std::is_same_v<value_type, Value>) {
    m_type  = List;
    m_value = new ListType;

    for (; it_first != it_last; ++it_first)
      ((ListType*)m_value)->push_back(*it_first);
  }
  else {
    m_type  = Struct;
    m_value = new StructType;

    for (; it_first != it_last; ++it_first)
      ((StructType*)m_value)->push_back(*it_first);
  }
}

template <typename It, typename T>
Json::Value::Value(It it_first, It it_last, const std::function<Value(T &val)> &to_value)
{
  m_type  = List;
  m_value = new ListType;
  for (; it_first != it_last; ++it_first)
    ((ListType*)m_value)->push_back(to_value(*it_first));
}


template <typename It, typename T>
Json::Value::Value(
  It it_first, It it_last, const std::function<Property(T &val)> &to_prop
)
{
  m_type  = Struct;
  m_value = new StructType;
  for (; it_first != it_last; ++it_first)
    ((StructType*)m_value)->push_back(to_prop(*it_first));
}


#endif // !SOURCE_VALUE_HPP

