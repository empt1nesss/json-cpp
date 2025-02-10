# json-cpp

Example:
```c++
#include <json.hpp>

#include <iostream>


struct Coords
{
  int x;
  int y;

  Json::Value Serialize()
  {
    return {
      Json::Property(L"x", (int64_t)x),
      Json::Property(L"x", (int64_t)y)
    };
  }
};


int main()
{
  Coords c { 2, 5 };

  Json j;

  j.FromProperties({
    { L"c", c.Serialize() }
  });

  std::wcout << j.Serialize();

  return 0;
}
```

