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
      Json::Property("x", x),
      Json::Property("y", y)
    };
  }
};


int main()
{
  Coords c { 2, 5 };

  Json j;

  j.FromProperties({
    { "coords", c.Serialize() }
  });
  j.GetData()["five"] = 5;

  std::cout << j.Serialize();

  return 0;
}
```

