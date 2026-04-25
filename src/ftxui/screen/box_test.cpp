// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include "ftxui/screen/box.hpp"

#include "gtest/gtest.h"  // for Test, EXPECT_EQ, EXPECT_TRUE, EXPECT_FALSE, TEST, TestPartResult

// NOLINTBEGIN
namespace ftxui {

TEST(BoxTest, Intersection_Overlap) {
  Box a{0, 10, 0, 10};
  Box b{5, 15, 5, 15};
  Box result = Box::Intersection(a, b);
  EXPECT_EQ(result.x_min, 5);
  EXPECT_EQ(result.x_max, 10);
  EXPECT_EQ(result.y_min, 5);
  EXPECT_EQ(result.y_max, 10);
}

TEST(BoxTest, Intersection_NoOverlap) {
  Box a{0, 5, 0, 5};
  Box b{6, 10, 6, 10};
  Box result = Box::Intersection(a, b);
  EXPECT_TRUE(result.IsEmpty());
}

TEST(BoxTest, Intersection_FullContainment) {
  Box outer{0, 10, 0, 10};
  Box inner{2, 8, 2, 8};
  Box result = Box::Intersection(outer, inner);
  EXPECT_EQ(result.x_min, 2);
  EXPECT_EQ(result.x_max, 8);
  EXPECT_EQ(result.y_min, 2);
  EXPECT_EQ(result.y_max, 8);
}

TEST(BoxTest, Intersection_SharedEdge) {
  Box a{0, 5, 0, 5};
  Box b{5, 10, 5, 10};
  Box result = Box::Intersection(a, b);
  EXPECT_EQ(result.x_min, 5);
  EXPECT_EQ(result.x_max, 5);
  EXPECT_EQ(result.y_min, 5);
  EXPECT_EQ(result.y_max, 5);
  EXPECT_FALSE(result.IsEmpty());
}

TEST(BoxTest, Union_NonOverlapping) {
  Box a{0, 5, 0, 5};
  Box b{7, 10, 7, 10};
  Box result = Box::Union(a, b);
  EXPECT_EQ(result.x_min, 0);
  EXPECT_EQ(result.x_max, 10);
  EXPECT_EQ(result.y_min, 0);
  EXPECT_EQ(result.y_max, 10);
}

TEST(BoxTest, Union_Overlapping) {
  Box a{0, 8, 0, 8};
  Box b{3, 12, 3, 12};
  Box result = Box::Union(a, b);
  EXPECT_EQ(result.x_min, 0);
  EXPECT_EQ(result.x_max, 12);
  EXPECT_EQ(result.y_min, 0);
  EXPECT_EQ(result.y_max, 12);
}

TEST(BoxTest, Shift) {
  Box box{1, 5, 2, 6};
  box.Shift(3, 4);
  EXPECT_EQ(box.x_min, 4);
  EXPECT_EQ(box.x_max, 8);
  EXPECT_EQ(box.y_min, 6);
  EXPECT_EQ(box.y_max, 10);
}

TEST(BoxTest, ShiftNegative) {
  Box box{5, 10, 5, 10};
  box.Shift(-3, -2);
  EXPECT_EQ(box.x_min, 2);
  EXPECT_EQ(box.x_max, 7);
  EXPECT_EQ(box.y_min, 3);
  EXPECT_EQ(box.y_max, 8);
}

TEST(BoxTest, ContainInside) {
  Box box{0, 10, 0, 10};
  EXPECT_TRUE(box.Contain(5, 5));
  EXPECT_TRUE(box.Contain(0, 0));
  EXPECT_TRUE(box.Contain(10, 10));
}

TEST(BoxTest, ContainOnBorder) {
  Box box{2, 8, 2, 8};
  EXPECT_TRUE(box.Contain(2, 2));
  EXPECT_TRUE(box.Contain(8, 8));
  EXPECT_TRUE(box.Contain(2, 8));
  EXPECT_TRUE(box.Contain(8, 2));
}

TEST(BoxTest, ContainOutside) {
  Box box{2, 8, 2, 8};
  EXPECT_FALSE(box.Contain(1, 5));
  EXPECT_FALSE(box.Contain(9, 5));
  EXPECT_FALSE(box.Contain(5, 1));
  EXPECT_FALSE(box.Contain(5, 9));
}

TEST(BoxTest, IsEmpty_Empty) {
  Box empty{5, 3, 0, 0};  // x_min > x_max
  EXPECT_TRUE(empty.IsEmpty());

  Box empty2{0, 0, 5, 3};  // y_min > y_max
  EXPECT_TRUE(empty2.IsEmpty());
}

TEST(BoxTest, IsEmpty_NotEmpty) {
  Box box{0, 5, 0, 5};
  EXPECT_FALSE(box.IsEmpty());

  Box single{3, 3, 3, 3};  // Single cell
  EXPECT_FALSE(single.IsEmpty());
}

TEST(BoxTest, EqualityOperator) {
  Box a{1, 2, 3, 4};
  Box b{1, 2, 3, 4};
  Box c{1, 2, 3, 5};
  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a == c);
}

TEST(BoxTest, InequalityOperator) {
  Box a{1, 2, 3, 4};
  Box b{1, 2, 3, 4};
  Box c{0, 2, 3, 4};
  EXPECT_FALSE(a != b);
  EXPECT_TRUE(a != c);
}

}  // namespace ftxui
// NOLINTEND
