#include "stdafx.h"
#include "CFuncLog.h"

//////////////////////////////////////////////////////////////////////////
// 

CFuncLog::CFuncLog( CLog &pLog, const std::string &FunctionName )
{
  m_pLog = &pLog;

  m_strFuncName = FunctionName.c_str();
  
  if( m_pLog != NULL )
    m_pLog->LogString( 2, std::string( m_strFuncName + " enter" ) );
}

//////////////////////////////////////////////////////////////////////////
// 

CFuncLog::CFuncLog( CLog *pLog, const std::string &FunctionName )
{
  m_pLog = pLog;
  
  m_strFuncName = FunctionName.c_str();

  if( m_pLog != NULL )
    m_pLog->LogString( 2, std::string( m_strFuncName + " enter" ) );
};

//////////////////////////////////////////////////////////////////////////
// 

CFuncLog::CFuncLog( CLog *pLog, char *szFunctionName )
{
  m_pLog = pLog;

  m_strFuncName = szFunctionName;

  if( m_pLog != NULL )
    m_pLog->LogString( 2, std::string( m_strFuncName + " enter" ) );
}

//////////////////////////////////////////////////////////////////////////
// 

CFuncLog::~CFuncLog(  )
{
  if( m_pLog != NULL )
    m_pLog->LogString( 2, std::string( m_strFuncName + " leave" ) );

  m_pLog = NULL;
};

//////////////////////////////////////////////////////////////////////////
// 

int CFuncLog::LogString( long Level, const std::string &Message )
{
  int retValue = 0;
  
  if( m_pLog != NULL )
    retValue = m_pLog->LogString( Level, ( m_strFuncName + Message.c_str() ) );

  return retValue;
};

//////////////////////////////////////////////////////////////////////////
// 

std::string CFuncLog::FormatString( const std::string Format, ... )
{
  va_list args;
  va_start( args, Format );

  m_tmpBuf[0] = 0;
  _vsnprintf_s( m_tmpBuf, _countof(m_tmpBuf), sizeof( m_tmpBuf ), Format.c_str(), args );
  va_end( args );
  
  return m_tmpBuf;
}

//////////////////////////////////////////////////////////////////////////
// 

CFuncLog &CFuncLog::operator<< ( int iValue )
{
  m_tmpBuf[0] = 0;
  _snprintf_s( m_tmpBuf, _countof(m_tmpBuf), sizeof( m_tmpBuf ), "%i", iValue );

  if( m_pLog != NULL )
    m_pLog->LogRawString( m_tmpBuf );

  return *this;
};

//////////////////////////////////////////////////////////////////////////
// 

CFuncLog &CFuncLog::operator<< ( long lValue )
{
  m_tmpBuf[0] = 0;
  _snprintf_s( m_tmpBuf, _countof(m_tmpBuf), sizeof( m_tmpBuf ), "%d", lValue );
  
  if( m_pLog != NULL )
    m_pLog->LogRawString( m_tmpBuf );
  
  return *this;
};

//////////////////////////////////////////////////////////////////////////
// 

CFuncLog &CFuncLog::operator<< ( double dValue )
{
  m_tmpBuf[0] = 0;
  _snprintf_s( m_tmpBuf, _countof(m_tmpBuf), sizeof( m_tmpBuf ), "%f", dValue );
  
  if( m_pLog != NULL )
    m_pLog->LogRawString( m_tmpBuf );

  return *this;
};

//////////////////////////////////////////////////////////////////////////
// 

CFuncLog &CFuncLog::operator<< ( const std::string &strValue )
{
  if( m_pLog != NULL )
    m_pLog->LogRawString( strValue );
  
  return *this;
};

//////////////////////////////////////////////////////////////////////////
//

CFuncLog &CFuncLog::operator<< ( char *cszValue )
{
  if( m_pLog != NULL )
    m_pLog->LogRawString( std::string( cszValue ) );
  
  return *this;
};

