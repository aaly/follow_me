#ifndef _COMMON_
#define _COMMON_

enum class StatusType {
    SUCCESS = 0,
    FAILURE
};

template<typename T>
class Result {
    public:

        Result() {};
        Result(const StatusType& status, const T& data):  _status(status), _data(data) {};

        inline void SetData(const T& data) {
            _data = data;
        }

        inline void SetStatus(StatusType status) {
            _status = status;
        }

        void Success(const T& data) {
            SetData(data);
            SetStatus(StatusType::SUCCESS);
        }

        void Failure(const T& data) {
            SetData(data);
            SetStatus(StatusType::FAILURE);
        }

        T& Data() {
            return _data;
        }

        StatusType Status() const {
            return _status;
        };

        bool Succeded() const {
            return StatusType::SUCCESS == _status;
        };

    private:
        StatusType _status;
        T _data;
};

#endif //_COMMON
