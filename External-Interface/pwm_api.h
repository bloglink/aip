#ifndef __ASM_ARCH_PWM_API_H
#define __ASM_ARCH_PWM_API_H

#ifdef  __cplusplus
extern "C" {
#endif

#define	PWM_POLARITY_NORMAL    (0 << 0)
#define PWM_POLARITY_INVERTED  (1 << 0)


int Beep_PWM_Start(double duty);
int Beep_PWM_Stop();

int GPIO_OutEnable(unsigned int dwEnBits);
int GPIO_OutDisable(unsigned int dwDisBits);
int GPIO_OutSet(unsigned int dwSetBits);
int GPIO_OutClear(unsigned int dwClearBits);

int Led_PWM_Start(int duty);
int Led_PWM_Stop();

#ifdef __cplusplus
}
#endif

#endif
