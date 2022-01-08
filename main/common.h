#ifndef _COMMON_
#define _COMMON_


enum class StatusType {
    SUCCESS,
    FAILURE
};

template<typename T>
class Result {
    public:

        Result() {};
        Result(const StatusType& status, const T& data):  _status(status), _data(data) {};

        void SetData(const T& data) {
            _data = data;
        }

        void SetStatus(StatusType status) {
            _status = status;
        }

        T& Data() const {
            return _data;
        }

        StatusType Status() const {
            return _status;
        };

    private:
        StatusType _status;
        T _data;
};

#endif //_COMMON
