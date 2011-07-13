/* Copyright (c) 2010 Stanford University
 * Copyright (c) 2011 Facebook
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR(S) DISCLAIM ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL AUTHORS BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <gtest/gtest.h>
#include <vector>

#include "Tub.h"

namespace {

using std::vector;

struct Foo {
    Foo(int x, int y, int z = 0, bool pleaseThrowAnException = false)
        : x(x) , y(y) , z(z)
    {
        if (pleaseThrowAnException)
            throw -1;
        ++liveCount;
    }
    ~Foo() {
        --liveCount;
    }
    int getX() {
        return x;
    }
    int x, y, z;
    static int liveCount;
};
int Foo::liveCount = 0;

typedef Tub<Foo> FooTub;
typedef Tub<int> IntTub;

TEST(Tub, basics) {
    Foo::liveCount = 0;
    {
        FooTub fooTub;
        EXPECT_FALSE(fooTub);
        Foo* foo = new(fooTub) Foo(1, 2, 3);
        EXPECT_TRUE(fooTub);
        EXPECT_EQ(1, foo->x);
        EXPECT_EQ(2, foo->y);
        EXPECT_EQ(3, foo->z);
        EXPECT_EQ(foo, fooTub.get());
        EXPECT_EQ(foo, &*fooTub);
        EXPECT_EQ(1, fooTub->getX());

        EXPECT_EQ(foo, new(fooTub) Foo(5, 6));
        EXPECT_EQ(5, foo->x);
        EXPECT_EQ(6, foo->y);
        EXPECT_EQ(0, foo->z);
    }
    EXPECT_EQ(0, Foo::liveCount);
}

TEST(Tub, alignment) {
    EXPECT_EQ(1, __alignof__(Tub<char>));
    EXPECT_EQ(8, __alignof__(Tub<uint64_t>));
}

TEST(Tub, copyAndAssign) {
    IntTub x;
    new(x) int(5);
    IntTub y;
    y = x;
    IntTub z(y);
    EXPECT_EQ(5, *y);
    EXPECT_EQ(5, *z);

    int p = 5;
    IntTub q(p);
    EXPECT_EQ(5, *q);
}

TEST(Tub, putInVector) {
    vector<IntTub> v;
    v.push_back(IntTub());
    IntTub eight;
    new(eight) int(8);
    v.push_back(eight);
    v.insert(v.begin(), IntTub());
    EXPECT_FALSE(v[0]);
    EXPECT_FALSE(v[1]);
    EXPECT_TRUE(v[2]);
    EXPECT_EQ(static_cast<void*>(&v[2]), static_cast<void*>(v[2].get()));
    EXPECT_EQ(8, *v[2]);
}

TEST(Tub, boolConversion) {
    IntTub x;
    EXPECT_FALSE(x);
    new(x) int(5);
    EXPECT_TRUE(x);
}

TEST(Tub, elementConstructorException) {
    Foo::liveCount = 0;
    FooTub x;
    try {
        new(x) Foo(1, 2, 3, true);
    } catch (int) {
    }
    EXPECT_FALSE(x);
    EXPECT_EQ(0, Foo::liveCount);
}

}  // anonymous namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
