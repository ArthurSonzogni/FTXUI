// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include "ftxui/component/terminal_input_parser.hpp"

extern "C" int LLVMFuzzerTestOneInput(const char* data, size_t size) {
  using namespace ftxui;
  auto event_receiver = MakeReceiver<Task>();
  {
    auto parser = TerminalInputParser(event_receiver->MakeSender());
    for (size_t i = 0; i < size; ++i) {
      parser.Add(data[i]);
    }
  }

  Task received;
  while (event_receiver->Receive(&received)) {
    // Do nothing.
  }
  return 0;  // Non-zero return values are reserved for future use.
}
