/*
 * Copyright 2013 The Flutter Authors. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of Google Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "flutter/shell/platform/ohos/platform_view_ohos_delegate.h"
#include <utility>

namespace flutter {

void putStringAttributesIntoBuffer(
    const StringAttributes& attributes,
    int32_t* buffer_int32,
    size_t& position,
    std::vector<std::vector<uint8_t>>& string_attribute_args) {
  if (attributes.empty()) {
    buffer_int32[position++] = -1;
    return;
  }
  buffer_int32[position++] = attributes.size();
  for (const auto& attribute : attributes) {
    buffer_int32[position++] = attribute->start;
    buffer_int32[position++] = attribute->end;
    buffer_int32[position++] = static_cast<int32_t>(attribute->type);
    switch (attribute->type) {
      case StringAttributeType::kSpellOut:
        buffer_int32[position++] = -1;
        break;
      case StringAttributeType::kLocale:
        buffer_int32[position++] = string_attribute_args.size();
        std::shared_ptr<LocaleStringAttribute> locale_attribute =
            std::static_pointer_cast<LocaleStringAttribute>(attribute);
        string_attribute_args.push_back(
            {locale_attribute->locale.begin(), locale_attribute->locale.end()});
        break;
    }
  }
}
// interaction between java and native, encoding the semantics info to bytebuffer
PlatformViewOHOSDelegate::PlatformViewOHOSDelegate(
    std::shared_ptr<PlatformViewOHOSNapi> napi_facade)
    : napi_facade_(std::move(napi_facade)) {};

void PlatformViewOHOSDelegate::UpdateSemantics(
    const flutter::SemanticsNodeUpdates& update,
    const flutter::CustomAccessibilityActionUpdates& actions) {
  constexpr size_t kBytesPerNode = 47 * sizeof(int32_t);
  constexpr size_t kBytesPerChild = sizeof(int32_t);
  constexpr size_t kBytesPerCustomAction = sizeof(int32_t);
  constexpr size_t kBytesPerAction = 4 * sizeof(int32_t);
  constexpr size_t kBytesPerStringAttribute = 4 * sizeof(int32_t);

  {
    size_t num_bytes = 0;
    for (const auto& value : update) {
      num_bytes += kBytesPerNode;
      num_bytes +=
          value.second.childrenInTraversalOrder.size() * kBytesPerChild;
      num_bytes += value.second.childrenInHitTestOrder.size() * kBytesPerChild;
      num_bytes += value.second.customAccessibilityActions.size() *
                   kBytesPerCustomAction;
      num_bytes +=
          value.second.labelAttributes.size() * kBytesPerStringAttribute;
      num_bytes +=
          value.second.valueAttributes.size() * kBytesPerStringAttribute;
      num_bytes += value.second.increasedValueAttributes.size() *
                   kBytesPerStringAttribute;
      num_bytes += value.second.decreasedValueAttributes.size() *
                   kBytesPerStringAttribute;
      num_bytes +=
          value.second.hintAttributes.size() * kBytesPerStringAttribute;
    }
    // encode the updated nodes/actions into bytebuffer/strings
    std::vector<uint8_t> buffer(num_bytes);
    std::vector<std::string> strings;
    std::vector<std::vector<uint8_t>> string_attribute_args;

    int32_t* buffer_int32 = reinterpret_cast<int32_t*>(&buffer[0]);
    float* buffer_float32 = reinterpret_cast<float*>(&buffer[0]);

    size_t position = 0;
    for (const auto& value : update) {
      // make sure you update kBytesPerNode and/or kBytesPerChild above to
      // match the number of values you are sending.
      const flutter::SemanticsNode& node = value.second;
      buffer_int32[position++] = node.id;
      buffer_int32[position++] = node.flags;
      buffer_int32[position++] = node.actions;
      buffer_int32[position++] = node.maxValueLength;
      buffer_int32[position++] = node.currentValueLength;
      buffer_int32[position++] = node.textSelectionBase;
      buffer_int32[position++] = node.textSelectionExtent;
      buffer_int32[position++] = node.platformViewId;
      buffer_int32[position++] = node.scrollChildren;
      buffer_int32[position++] = node.scrollIndex;
      buffer_float32[position++] = static_cast<float>(node.scrollPosition);
      buffer_float32[position++] = static_cast<float>(node.scrollExtentMax);
      buffer_float32[position++] = static_cast<float>(node.scrollExtentMin);
      if (node.label.empty()) {
        buffer_int32[position++] = -1;
      } else {
        buffer_int32[position++] = strings.size();
        strings.push_back(node.label);
      }

      putStringAttributesIntoBuffer(node.labelAttributes, buffer_int32,
                                    position, string_attribute_args);
      if (node.value.empty()) {
        buffer_int32[position++] = -1;
      } else {
        buffer_int32[position++] = strings.size();
        strings.push_back(node.value);
      }

      putStringAttributesIntoBuffer(node.valueAttributes, buffer_int32,
                                    position, string_attribute_args);
      if (node.increasedValue.empty()) {
        buffer_int32[position++] = -1;
      } else {
        buffer_int32[position++] = strings.size();
        strings.push_back(node.increasedValue);
      }

      putStringAttributesIntoBuffer(node.increasedValueAttributes, buffer_int32,
                                    position, string_attribute_args);
      if (node.decreasedValue.empty()) {
        buffer_int32[position++] = -1;
      } else {
        buffer_int32[position++] = strings.size();
        strings.push_back(node.decreasedValue);
      }

      putStringAttributesIntoBuffer(node.decreasedValueAttributes, buffer_int32,
                                    position, string_attribute_args);

      if (node.hint.empty()) {
        buffer_int32[position++] = -1;
      } else {
        buffer_int32[position++] = strings.size();
        strings.push_back(node.hint);
      }

      putStringAttributesIntoBuffer(node.hintAttributes, buffer_int32, position,
                                    string_attribute_args);

      if (node.tooltip.empty()) {
        buffer_int32[position++] = -1;
      } else {
        buffer_int32[position++] = strings.size();
        strings.push_back(node.tooltip);
      }

      buffer_int32[position++] = node.textDirection;
      buffer_float32[position++] = node.rect.left();
      buffer_float32[position++] = node.rect.top();
      buffer_float32[position++] = node.rect.right();
      buffer_float32[position++] = node.rect.bottom();
      node.transform.getColMajor(&buffer_float32[position]);
      position += 16;

      buffer_int32[position++] = node.childrenInTraversalOrder.size();
      for (int32_t child : node.childrenInTraversalOrder) {
        buffer_int32[position++] = child;
      }

      for (int32_t child : node.childrenInHitTestOrder) {
        buffer_int32[position++] = child;
      }

      buffer_int32[position++] = node.customAccessibilityActions.size();
      for (int32_t child : node.customAccessibilityActions) {
        buffer_int32[position++] = child;
      }
    }

    // custom accessibility actions.
    size_t num_action_bytes = actions.size() * kBytesPerAction;
    std::vector<uint8_t> actions_buffer(num_action_bytes);
    int32_t* actions_buffer_int32 =
        reinterpret_cast<int32_t*>(&actions_buffer[0]);

    std::vector<std::string> action_strings;
    size_t actions_position = 0;
    for (const auto& value : actions) {
      // If you edit this code, make sure you update kBytesPerAction
      // to match the number of values you are sending.
      const flutter::CustomAccessibilityAction& action = value.second;
      actions_buffer_int32[actions_position++] = action.id;
      actions_buffer_int32[actions_position++] = action.overrideId;
      if (action.label.empty()) {
        actions_buffer_int32[actions_position++] = -1;
      } else {
        actions_buffer_int32[actions_position++] = action_strings.size();
        action_strings.push_back(action.label);
      }
      if (action.hint.empty()) {
        actions_buffer_int32[actions_position++] = -1;
      } else {
        actions_buffer_int32[actions_position++] = action_strings.size();
        action_strings.push_back(action.hint);
      }
    }

    if (!actions_buffer.empty()) {
      FML_DLOG(INFO) << "PlatformViewOHOSDelegate::"
                        "updateCustomAccessibilityActions is called";
    }

    if (!buffer.empty()) {
      FML_DLOG(INFO) << "PlatformViewOHOSDelegate::UpdateSemantics is called";
    }
  }
}

}  // namespace flutter