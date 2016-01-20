#ifndef _INETCONNSTAT_INC_
#define _INETCONNSTAT_INC_
// CPMWInternetConnectStatus provides info. about the CInternetConnect object
class CInternetConnectStatus
   {
   public:
      // Construction
      CInternetConnectStatus()
         {
            SetStatus(statusWaitingForConnect);
         }

      // Operations
      enum Status
         {
            statusSuccess,
            statusConnected,
            statusWaitingForConnect,
            statusConnectFailed,
            statusUserAbort,
            statusWinsockError,
            statusTimedOut, 
            statusInitFailed,
            statusTimerFailed,
            statusInvalidURL,
            statusOtherError,
            statusInvalidSettings,
            statusEventCreationFailed,
         };
      const Status GetStatus() const
         {
            return m_enStatus;
         }
      void SetStatus(Status enStatus)
         {
            m_enStatus = enStatus;
         }
      void Reset()
         {
            SetStatus(statusWaitingForConnect);
         }
   protected:
      // Data members
      Status      m_enStatus;
   };

#endif