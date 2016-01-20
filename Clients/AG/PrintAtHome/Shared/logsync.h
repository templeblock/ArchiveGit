#ifndef _LOGGER_SYNC_SIMPLE_OBJECTS_H_
#define _LOGGER_SYNC_SIMPLE_OBJECTS_H_

#pragma once
#pragma warning( push, 3 )

#pragma warning( disable: 4786 ) // disable STL warnings

//////////////////////////////////////////////////////////////////////////
//  

namespace LOGGER
{

  class CAutoCritic : public CRITICAL_SECTION
  {
  public:
    CAutoCritic( void )
    {
      ::InitializeCriticalSection( this );
    }
    
    ~CAutoCritic( void )
    {
      ::DeleteCriticalSection( this );
    }
  };
  
  class CLogSimpleLock
  {
    public:
      CLogSimpleLock( CAutoCritic *pSection ) : m_pSection( pSection )
      {
        if( m_pSection != NULL )
          EnterCriticalSection( m_pSection );
      }
      
      ~CLogSimpleLock(  )
      {
        if( m_pSection != NULL )
          LeaveCriticalSection( m_pSection );
        
        m_pSection = NULL;
      }
      
    private:
      CAutoCritic *m_pSection;
  };
  
};

#pragma warning( pop )


#endif // _LOGGER_SYNC_SIMPLE_OBJECTS_H_