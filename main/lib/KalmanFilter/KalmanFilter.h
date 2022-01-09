/*
 * kalmanFilter.h - Library for simple Kalman filter
 *
 * Created by Max Lunin http://max.lunin.info : <nut.code.monkey@gmail.com> on 27.09.13.
 * as part of Arduino automatization described at http://exotic-garden-at-home.blogspot.com
 * Released into the public domain.
 */

/*
    // Usage :
    KalmanFilter kalmanFilter; // create with default parameters

    kalmanFilter.setState(startValue);
    kalmanFilter.setCovariance(0.1);

    while (true)
    {
        double value = getValueFromSensor();
        kalmanFilter.correct(value);
        double correctedValue = kalmanFilter.getState();
    }
*/

#ifndef Kalman_Kalman_h
#define Kalman_Kalman_h

namespace Lib {
    class KalmanFilter {
        public:
            
            KalmanFilter(double q = 1, double r = 1, double f = 1, double h = 1);
            void Reset(double q = 1, double r = 1, double f = 1, double h = 1);
            bool IsInitialized();

            double GetState() const;
            void SetState(double state);
            void SetCovariance(double covariance);    
            void Correct(double data);
            double CorrectAndGet(double data);

        public:
            double GetCovariance() const;
            double GetX0() const;
            double GetP0() const;
            double GetF() const;
            double GetQ() const;
            double GetH() const;
            double GetR() const;
            
        private:
            double x0; // predicted state
            double p0; // predicted covariance
            double F; // factor of real value to previous real value
            double Q; // measurement noise
            double H; // factor of measured value to real value
            double R; // environment noise
            double state;
            double covariance;
            bool _init = false;
    };
} //namespace Lib

#endif
