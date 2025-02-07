#ifndef _TIMER_LIB
#define _TIMER_LIB

#include "freertos/FreeRTOS.h"

#include "../common.h"
#include <string>
#include <functional>
#include <atomic>
#include <M5Unified.hpp>

namespace Lib {


    struct CallbackWrapper {
        std::function<void(void)> callback = nullptr;
        TimerHandle_t  timerHandle = nullptr;
        bool singleShot = false;
        std::string name;
    };

    static void methodAdapter(TimerHandle_t xTimer) {
        CallbackWrapper* callbackWrapper = static_cast<CallbackWrapper*>(pvTimerGetTimerID(xTimer));
        //Serial.printf("methodAdapter name [%s]\n", callbackWrapper->name.c_str());
        //Serial.printf("methodAdapter callbackWrapper [%p]\n", (void*)callbackWrapper);
        //Serial.printf("methodAdapter callbackWrapper handler [%p]\n", (void*)callbackWrapper->timerHandle);
        //Serial.printf("methodAdapter callbackWrapper->singleShot [%d]\n", callbackWrapper->singleShot);
        
        std::function<void(void)> callback = static_cast<std::function<void(void)>>(callbackWrapper->callback);
        callback();
        TimerHandle_t timerHandle = callbackWrapper->timerHandle;
        if(callbackWrapper->singleShot && timerHandle) {
            xTimerDelete(timerHandle, 0);
            delete callbackWrapper;
        }

        //Serial.printf("end methodAdapter\n");
        //Serial.flush();
    }
    
    class Timer {
        public:
            Timer(const std::string& timerName, uint32_t period, std::function<void(void)> callback, bool singleShot):
            _timerName(timerName), _period(period), _callback(std::move(callback)), _singleShot(singleShot) {
            }

            Timer() {
            }

            Timer(const Timer& timer):
            _timerName(timer.GetName()), _period(timer.GetPeriod()), _callback(std::move(timer.GetCallback())), _singleShot(timer.GetSingleShot()) {
            }

            ~Timer(){
                if(!_running && nullptr != _timerHandle) {
                    xTimerDelete(_timerHandle, 0);
                    if (_callbackWrapper) {
                        delete _callbackWrapper;
                    }
                }
            }

            Result<std::string> Start() {
                Result<std::string> result;
                
                if(nullptr != _callback) {
                    _callbackWrapper = new CallbackWrapper();

                    _timerHandle = xTimerCreate(
                        _timerName.c_str(),            /* Just a text name, not used by the RTOS kernel. */
                        pdMS_TO_TICKS(_period),        /* The timer period in ticks, must be greater than 0. */
                        _singleShot? pdFALSE: pdTRUE,  /* The timers will auto-reload themselves when they expire. */
                        (void*)_callbackWrapper,       /* The ID is used to store a count of the number of times the timer has expired, which is initialised to 0. */
                        methodAdapter                  /* Each timer calls the same callback when it expires. */
                    );

                    _callbackWrapper->callback = _callback;
                    _callbackWrapper->singleShot = _singleShot;
                    _callbackWrapper->timerHandle = _timerHandle;
                    _callbackWrapper->name = _timerName;

                    if(nullptr != _timerHandle) {
                        if(xTimerStart(_timerHandle, 0) != pdPASS )
                        {
                            result.Failure("Failed to Star Timer  [" + _timerName + "]\n");
                            _running.store(false);
                        }
                        else {
                            result.Success("Created Timer [" + _timerName + "]\n");
                            _running.store(true);
                        }
                    }
                    else {
                        _running .store(false);
                        result.Failure("Failed to Create Timer  [" + _timerName + "]\n");
                    }
                }
                else {
                    result.Failure("Failed to start timer as it is uninitialized !\n");
                }
                return result;
            }

            Result<std::string> Stop() {
                Result<std::string> result;
                if(_running) {
                    xTimerStop(_timerHandle, 0);
                    _running.store(false);
                }
                return result;
            }

            std::string GetName() const {
                return _timerName;
            }

            uint32_t GetPeriod() const {
                return _period;
            }

            bool GetSingleShot() const {
                return _singleShot;
            }

            std::function<void(void)> GetCallback() const {
                return _callback;
            }

        private:
            TimerHandle_t  _timerHandle = nullptr;
            std::string _timerName;
            uint32_t _period = 0;
            std::function<void(void)> _callback;
            std::atomic<bool> _singleShot {false};
            std::atomic<bool> _running {false};
            CallbackWrapper* _callbackWrapper = nullptr;
    };

} //namespace Lib
#endif //_TIMER_LIB