/********************************************************************************************
*
* AUTHOR        : Martin Ziacek, Martin.Ziacek@swh.sk, http://www.swh.sk
* COPYRIGHT     : 1999 Martin Ziacek
* DEPARTMENT    : SWH s.r.o
* TELEPHONE     : +421 7 59684147
* CREATION-DATE : 1.5.1999 8:27:23
* SP-NO         : 
* FUNCTION      : Implementation of QuickSort algorithm as template for array class
*                 and template for simple function QuickSort()
* 
*********************************************************************************************/

#ifndef _QuickSortCArrayAndFunctionTemplate_7EB8E364_1A47_11d3_AFD1_0080ADB99E81_
#define _QuickSortCArrayAndFunctionTemplate_7EB8E364_1A47_11d3_AFD1_0080ADB99E81_

//////////////////////////////////////////////////////////////////////////
// QuickSort functions
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// QuickSortRecursive - core of algorithm, do not call it, use QuickSort,
// see below
template <class T, class Comparitor> void QuickSortRecursive(T *pArr, int d, int h, Comparitor& comp )
{
	int i,j;
	T str;

	i = h;
	j = d;

	str = pArr[((int) ((d+h) / 2))];

	do {

		while (comp.less(pArr[j],str)) j++;
		while (!(comp.less(pArr[i],str) || (comp.equal(pArr[i], str)))) i--; // should be <=

		if ( i >= j ) {

			if ( i != j ) {
				T zal;

				zal = pArr[i];
				pArr[i] = pArr[j];
				pArr[j] = zal;

			}

			i--;
			j++;
		}
	} while (j <= i);

	if (d < i) QuickSortRecursive(pArr,d,i,comp);
	if (j < h) QuickSortRecursive(pArr,j,h,comp);
}

//////////////////////////////////////////////////////////////////////////
// QuickSort - entry to algorithm
//
// T *pArr			... pointer of array to sort
// int iSize		... size of array T *pArr
// bool bAscending	... if bAscending == TRUE, then sort ascending,
//						otherwise descending
//
// return TRUE if no error, error can be bad parameter, call ::GetLastError()
// if QuickSort returned FALSE
template <class T, class Comparitor> bool QuickSort(T *pArr, int iSize, Comparitor& comp)
{
	bool rc = true;

	if (iSize > 1) 
	{
		int	low = 0,
			high = iSize - 1;

		QuickSortRecursive(pArr,low,high,comp);
	}
	else 
	{
		rc = false;
	}

	return rc;
}

template <class T> class SimpleComparitor
{
public:
	bool less(const T& left, const T& right)
	{
		return left<right;
	}
	bool equal(const T& left, const T& right)
	{
		return left == right;
	}
};

template <class T> class SimpleComparitor<T*>
{
public:
	bool less(const T* left, const T* right)
	{
		return (*left)<(*right);
	}
	bool equal(const T* left, const T* right)
	{
		return (*left) == (*right);
	}
};

#endif /* _QuickSortCArrayAndFunctionTemplate_7EB8E364_1A47_11d3_AFD1_0080ADB99E81_ */
