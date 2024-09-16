
#ifndef VEC2_H
#define VEC2_H

class Vec2 {
public:
  float x = 0, y = 0;
  Vec2();
  Vec2(float x, float y) : x(x), y(y) {};

  bool operator == (const Vec2 &other) const;
  bool operator != (const Vec2 &other) const;

  Vec2 operator + (const Vec2 &other) const;
  Vec2 operator - (const Vec2 &other) const;
  Vec2 operator * (const Vec2 &other) const;
  Vec2 operator / (const Vec2 &other) const;


  void operator += (const Vec2 &other);
  void operator -= (const Vec2 &other);
  void operator *= (const Vec2 &other);
  void operator /= (const Vec2 &other);

  float dist(const Vec2 &other) const;

  void print() const;


};

#endif //VEC2_H
