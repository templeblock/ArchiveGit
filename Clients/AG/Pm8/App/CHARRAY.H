
#include "dynarray.h"

typedef void FAR* LPARRAYELEMENT;
typedef const void FAR* LPCARRAYELEMENT;

extern void HMemSet(LPVOID pDst, int nValue, long lSize);

class CHugeArray : public CDynamicArray
{
public:
   CHugeArray(DWORD dwElementSize=0, DWORD dwIncrement=100, DWORD dwMaximum=1048576);
   virtual ~CHugeArray(void)        {};
// Attributes
	DWORD GetSize();        // const
	DWORD GetUpperBound();  // const
	void  SetSize(DWORD dwNewSize, DWORD dwGrowBy = -1);
	void  SetElementSize(DWORD dwElementSize);
	DWORD GetElementSize(void);   // const

// Operations
	// Clean up
	void RemoveAll();
   void FreeExtra();

	// Accessing elements
	LPCARRAYELEMENT GetAt(DWORD dwIndex);     // const
//	void SetAt(int nIndex, LPCARRAYELEMENT pNewElement);
	LPARRAYELEMENT ElementAt(DWORD dwIndex);
	LPARRAYELEMENT GetData();

	// Potentially growing the array
//	void SetAtGrow(int nIndex, LPCARRAYELEMENT pNewElement);
	int Add(LPCARRAYELEMENT pNewElement);

	// overloaded operator helpers
//	LPCARRAYELEMENT operator[](DWORD dwIndex);   // const
	LPARRAYELEMENT operator[](DWORD dwIndex);

	// Operations that move elements around
	void InsertAt(DWORD dwIndex, LPCARRAYELEMENT pNewElement, DWORD dwCount = 1);
//	void RemoveAt(int nIndex, int nCount = 1);
//	void InsertAt(int nStartIndex, CVarArray* pNewArray);

	// Set all bytes in the array to a value.
	void WipeData(BYTE bValue);
};

// Inline Section
inline CHugeArray::CHugeArray(DWORD dwElementSize, DWORD dwIncrement, DWORD dwMaximum) :
   CDynamicArray(dwElementSize, dwIncrement, dwMaximum)
   {
   }

inline DWORD CHugeArray::GetSize()
   {
     return CDynamicArray::GetElementCount();
   }

inline DWORD CHugeArray::GetUpperBound()
   {
     return GetSize() - 1;
   }

inline void CHugeArray::SetSize(DWORD dwNewSize, DWORD dwGrowBy)
   {
      // Need to add grow by logic
      CDynamicArray::Allocate(dwNewSize);
      CDynamicArray::SetElementCount(dwNewSize);
   }

inline void CHugeArray::SetElementSize(DWORD dwElementSize)
   {
      m_dwElementSize = dwElementSize;
   }

inline DWORD CHugeArray::GetElementSize()
   {
      return CDynamicArray::GetElementSize();
   }

inline void CHugeArray::RemoveAll()
   {
      CDynamicArray::Free();
   }

inline LPCARRAYELEMENT CHugeArray::GetAt(DWORD dwIndex)
   {
      return CDynamicArray::GetElement(dwIndex);
   }

inline LPARRAYELEMENT CHugeArray::ElementAt(DWORD dwIndex)
   {
      return CDynamicArray::GetElement(dwIndex);
   }

inline LPARRAYELEMENT CHugeArray::GetData()
   {
      return CDynamicArray::GetElement(0);
   }

inline int CHugeArray::Add(LPCARRAYELEMENT pNewElement)
   {
      return CDynamicArray::AddElement((LPARRAYELEMENT)pNewElement);
   }

inline void CHugeArray::InsertAt(DWORD dwIndex, LPCARRAYELEMENT pNewElement,
   DWORD dwCount)
   {
      // Parent returns BOOL
      CDynamicArray::InsertElement(dwIndex, (LPARRAYELEMENT )pNewElement, dwCount);
   }

inline void CHugeArray::FreeExtra()
   {
      CDynamicArray::Pack();
   }

//inline LPCARRAYELEMENT CHugeArray::operator[](DWORD dwIndex)
//	{ return GetAt(dwIndex); }
inline LPARRAYELEMENT CHugeArray::operator[](DWORD dwIndex)
	{ return (LPARRAYELEMENT)GetAt(dwIndex); }

inline void CHugeArray::WipeData(BYTE bValue)
	{ HMemSet(GetData(), bValue, GetSize()*GetElementSize()); }
