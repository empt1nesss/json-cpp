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
    NotStruct
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
      type  = Int;
      value = new int64_t(val);
    }
    else {
      type  = Float;
      value = new double(val);
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

  ValueType GetType() { return type; } const

  bool         GetBool   () const { return *(bool*)value; }
  int64_t      GetInt    () const { return *(int64_t*)value; }
  double       GetFloat  () const { return *(double*)value; }
  std::string  GetString () const { return Json::to_str(*(std::wstring*)value); }
  std::wstring GetStringW() const { return *(std::wstring*)value; }
  ListType     GetList   () const { return *(ListType*)value; }
  StructType   GetStruct () const { return *(StructType*)value; }

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

  ValueType type;
  void*     value;


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
    type  = List;
    value = new ListType;

    for (; it_first != it_last; ++it_first)
      ((ListType*)value)->push_back(*it_first);
  }
  else {
    type  = Struct;
    value = new StructType;

    for (; it_first != it_last; ++it_first)
      ((StructType*)value)->push_back(*it_first);
  }
}

template <typename It, typename T>
Json::Value::Value(It it_first, It it_last, const std::function<Value(T &val)> &to_value)
{
  type  = List;
  value = new ListType;
  for (; it_first != it_last; ++it_first)
    ((ListType*)value)->push_back(to_value(*it_first));
}


template <typename It, typename T>
Json::Value::Value(
  It it_first, It it_last, const std::function<Property(T &val)> &to_prop
)
{
  type  = Struct;
  value = new StructType;
  for (; it_first != it_last; ++it_first)
    ((StructType*)value)->push_back(to_prop(*it_first));
}


#endif // !SOURCE_VALUE_HPP

