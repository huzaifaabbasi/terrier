//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// deleter_proxy.cpp
//
// Identification: src/execution/proxy/deleter_proxy.cpp
//
// Copyright (c) 2015-2018, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/proxy/deleter_proxy.h"

#include "execution/proxy/data_table_proxy.h"
#include "execution/proxy/executor_context_proxy.h"
#include "execution/proxy/transaction_context_proxy.h"
#include "execution/proxy/executor_context_proxy.h"

namespace peloton {
namespace codegen {

DEFINE_TYPE(Deleter, "codegen::Deleter", opaque);

DEFINE_METHOD(peloton::codegen, Deleter, Init);
DEFINE_METHOD(peloton::codegen, Deleter, Delete);

}  // namespace codegen
}  // namespace peloton
