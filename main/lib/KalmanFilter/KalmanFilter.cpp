/*
 * Kalman.cpp - Arduino Library for simple Kalman filter
 *
 * Created by Max Lunin http://max.lunin.info : <nut.code.monkey@gmail.com> on 27.09.13.
  * as part of Arduino automatization described at http://exotic-garden-at-home.blogspot.com
 * Released into the public domain.
 */

#include "KalmanFilter.h"

namespace Lib {
    KalmanFilter::KalmanFilter(double q, double r, double f, double h) {
        F = f;
        Q = q;
        H = h;
        R = r;
    }

    void KalmanFilter::Reset(double q, double r, double f, double h) {
        F = f;
        Q = q;
        H = h;
        R = r;
        _init = false;
    }

    bool KalmanFilter::IsInitialized(){
        return _init;
    }

    double KalmanFilter::GetX0() const {
        return x0;
    }

    double KalmanFilter::GetP0() const {
        return p0;
    }

    double KalmanFilter::GetF() const {
        return F;
    }

    double KalmanFilter::GetQ() const {
        return Q;
    }

    double KalmanFilter::GetH() const {
        return H;
    }

    double KalmanFilter::GetR() const {
        return R;
    }

    double KalmanFilter::GetState() const {
        return state;
    }

    void KalmanFilter::SetState(double state) {
        state = state;
        _init = true;
    }

    double KalmanFilter::GetCovariance() const {
        return covariance;
    }

    void KalmanFilter::SetCovariance(double covariance) {
        covariance = covariance;
    }

    double KalmanFilter::CorrectAndGet(double data) {
        Correct(data);
        return GetState();
    }
    void KalmanFilter::Correct(double data) {
        x0 = F*state;
        p0 = F*F*covariance + Q;
        
        //measurement update - correction
        double K = (H*p0)/(H*p0*H + R);
        state = x0 + K*(data - H*x0);
        covariance = (1 - K*H)*p0;
    }
} //namespace Lib