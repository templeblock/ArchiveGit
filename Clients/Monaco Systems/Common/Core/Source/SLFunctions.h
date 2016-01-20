#ifdef __cplusplus
extern "C" {
#endif

void SLDisable(void);
void SLEnable(void);

void SLEnterInterrupt(void);
void SLLeaveInterrupt(void);

void SLResetLeaks(void);

#ifdef __cplusplus
}
#endif
