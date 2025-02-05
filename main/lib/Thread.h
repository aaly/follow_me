#ifndef _THREAD_LIB
#define _THREAD_LIB

#include "freertos/FreeRTOS.h"

#include "../common.h"
#include <string>
#include <functional>

namespace Lib {
    
    class Thread {
        public:

            Thread(const std::string& taskName, std::function<void(void)> callback, uint32_t taskSize):
            _taskName(taskName), _callback(std::move(callback)), _taskSize(taskSize) {

            }

            ~Thread(){
                if(nullptr != _taskHandle) {
                    vTaskDelete(_taskHandle);
                }
            }

            /**
             * @brief Run the task/thread, return result with a status message containing task name
             */
            Result<std::string> Run() {
                Result<std::string> result;
                /* Create the task, storing the handle. */
                const auto xReturned = xTaskCreate(
                                    methodAdapter,       /* Function that implements the task. */
                                    _taskName.c_str(),   /* Text name for the task. */
                                    _taskSize,            /* Stack size in words, not bytes. */
                                    (void*)&_callback,    /* Parameter passed into the task. */
                                    tskIDLE_PRIORITY,    /* Priority at which the task is created. */
                                    &_taskHandle);       /* Used to pass out the created task's handle. */

                if( xReturned == pdPASS ) {
                    result.Success("Created Task for Service [" + _taskName + "]\n");
                }
                else {
                    result.Failure("Failed to Create Task for Service [" + _taskName + "]\n");
                    /* The task was created.  Use the task's handle to delete the task. */
                    vTaskDelete(_taskHandle);
                }
                return result;
            }

            /**
             * @brief abort running task
             */
            Result<std::string> Abort() {
                Result<std::string> result;
                
                result.Success("");

                if(_taskHandle != nullptr)
                {
                    vTaskDelete(_taskHandle);
                }
                return result;
            }

            /**
             * @brief method adapter to be called by Run() and accomodate the xTaskCreate C interface
             */
            static void methodAdapter(void *pvParameters) {
                std::function<void(void)> callback = *static_cast<std::function<void(void)>*>(pvParameters);
                callback();
            }

        private:
            TaskHandle_t _taskHandle = nullptr;
            std::string _taskName;
            std::function<void(void)> _callback;
            uint32_t _taskSize;
    };

} //namespace Lib
#endif //_THREAD_LIB