#ifndef __MPU9250_H__
#define __MPU9250_H__

typedef int16 int16_t;

void mpu9250_init(void);
int mpu9250_accel(float *x, float *y, float *z);
int mpu9250_gyro(int16_t *x, int16_t *y, int16_t *z);
int mpu9250_mga(int16_t *x, int16_t *y, int16_t *z);

#endif
