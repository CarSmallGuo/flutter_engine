/*
 * Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef OHOS_TYPES_H
#define OHOS_TYPES_H
namespace flutter {
constexpr int PIXEL_FMT_RGBA_8888 = 12;

enum Locales {
  LANGUAGE_INDEX = 0,
  REGION_INDEX,
  SCRIPT_INDEX,
};

enum Number {
  NEGATIVE_FIVE = -5,
  NEGATIVE_FOUR = -4,
  NEGATIVE_THREE = -3,
  NEGATIVE_TWO = -2,
  NEGATIVE_ONE = -1,
  ZERO = 0,
  ONE,
  TWO,
  THREE,
  FOUR,
  FIVE,
  SIX,
  SEVEN,
  EIGHT,
  NINE,
  TEN,
};

enum Range {
  ZEROTH = 0,
  FIRST = 1,
  SECOND = 2,
  THIRD = 3,
  FOURTH = 4,
  FIFTH = 5,
  SIXTH = 6,
  SEVENTH = 7,
  EIGHTH = 8,
  NINTH = 9,
  TENTH = 10,
  ELEVENTH = 11,
  TWELFTH = 12,
  THIRTEENTH = 13,
  FOURTEENTH = 14,
  FIFTEENTH = 15,
  SIXTEENTH = 16,
  SEVENTEENTH = 17,
  EIGHTEENTH = 18,
  NINETEENTH = 19,
};
}  // namespace flutter
#endif