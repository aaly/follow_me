#include "PanicService.h"

namespace Services {
  Result<std::string> PanicService::Init(const ParametersPack& config) {
    Result<std::string> result;
    result.SetStatus(StatusType::SUCCESS);
    return result;
  }

  Result<std::string> PanicService::Run() {
    Result<std::string> result;
    result.SetStatus(StatusType::FAILURE);
    return result;
  }

} //namespace Services