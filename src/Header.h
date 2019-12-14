#pragma once

// CRT
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cstdarg>
#include <cmath>
#include <clocale>
#include <malloc.h>
#include <mbstring.h>
//#include <string.h>
#include <tchar.h>
#include <cstdint>
#include <cctype>

// STL containers
#include <vector>
#include <map>
#include <string>
#include <set>
#include <list>
#include <deque>
#include <queue>
#include <bitset>
#include <unordered_set>
#include <unordered_map>
#include <stack>

// STL algorithms & functions
#include <algorithm>
#include <functional>
#include <limits>
#include <future>

#include "vtkSmartPointer.h"
#include <vtkObjectFactory.h>
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleImage.h"
#include <vtkActor.h>
#include "vtkImageActor.h"
#include "vtkImageMapper3D.h"
#include "vtkMapper.h"
#include "vtkImageMapper.h"
#include <vtkCommand.h>
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkMatrix4x4.h>
#include <vtkImageData.h>
#include <vtkVolume.h>
#include <vtkMath.h>
#include <vtkTransform.h>
#include <vtkAssembly.h>
#include <vtkCamera.h>
#include <vtkLookupTable.h>

//Qt
#include <QDebug>
#include <QString>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QWidget>
#include <QDialog>

#include "UObject.h"
#include "glm.hpp"
#include "ext.hpp"
#include "glog/logging.h"


// type
typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned int		uint32;
#ifdef PLATFORM_WINDOWS
typedef unsigned __int64	uint64;
#else
typedef unsigned long long	uint64;
#endif
typedef signed char			int8;
typedef signed short		int16;
typedef signed int			int32;
#ifdef PLATFORM_WINDOWS
typedef signed __int64		int64;
#else
typedef long long			int64;
#endif
typedef unsigned char		uchar;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef unsigned long		ulong;



typedef std::function<void()> VoidCallback;
typedef std::function<bool()> BoolCallback;


#define MAX(a, b) (a) > (b) ? (a) : (b)
#define MIN(a, b) (a) < (b) ? (a) : (b)