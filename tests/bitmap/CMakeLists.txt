K3D_TEST(bitmap.source.JPEGBitmapReader
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.source.JPEGBitmapReader.py
	REQUIRES K3D_BUILD_JPEG_IO_MODULE 
	LABELS bitmap source reader JPEGBitmapReader)

K3D_TEST(bitmap.source.PNGBitmapReader 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.source.PNGBitmapReader.py
	REQUIRES K3D_BUILD_PNG_IO_MODULE 
	LABELS bitmap source reader PNGBitmapReader)

K3D_TEST(bitmap.source.TIFFBitmapReader 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.source.TIFFBitmapReader.py
	REQUIRES K3D_BUILD_TIFF_IO_MODULE 
	LABELS bitmap source reader TIFFBitmapReader)

K3D_TEST(bitmap.source.BitmapReader.bmp 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.source.BitmapReader.bmp.py
	REQUIRES K3D_BUILD_IMAGEMAGICK_IO_MODULE K3D_BUILD_BITMAP_MODULE 
	LABELS bitmap source reader BitmapReader)

K3D_TEST(bitmap.source.BitmapReader.jpeg 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.source.BitmapReader.jpeg.py
	REQUIRES K3D_BUILD_JPEG_IO_MODULE K3D_BUILD_BITMAP_MODULE 
	LABELS bitmap source reader BitmapReader)

K3D_TEST(bitmap.source.BitmapReader.png 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.source.BitmapReader.png.py
	REQUIRES K3D_BUILD_PNG_IO_MODULE K3D_BUILD_BITMAP_MODULE 
	LABELS bitmap source reader BitmapReader)

K3D_TEST(bitmap.source.BitmapReader.sun 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.source.BitmapReader.sun.py
	REQUIRES K3D_BUILD_IMAGEMAGICK_IO_MODULE K3D_BUILD_BITMAP_MODULE 
	LABELS bitmap source reader BitmapReader)

K3D_TEST(bitmap.source.BitmapReader.tiff 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.source.BitmapReader.tiff.py
	REQUIRES K3D_BUILD_TIFF_IO_MODULE K3D_BUILD_BITMAP_MODULE 
	LABELS bitmap source reader BitmapReader)

K3D_TEST(bitmap.source.BitmapChecker 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.source.BitmapChecker.py
	REQUIRES K3D_BUILD_BITMAP_MODULE 
	LABELS bitmap source BitmapChecker)

K3D_TEST(bitmap.source.BitmapSolid 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.source.BitmapSolid.py
	REQUIRES K3D_BUILD_BITMAP_MODULE 
	LABELS bitmap source BitmapSolid)

K3D_TEST(bitmap.source.BitmapSourceScript 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.source.BitmapSourceScript.py
	REQUIRES K3D_BUILD_SCRIPTING_MODULE 
	LABELS bitmap source BitmapSourceScript)





K3D_TEST(bitmap.modifier.BitmapAdd 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.modifier.BitmapAdd.py
	REQUIRES K3D_BUILD_BITMAP_MODULE
	LABELS bitmap modifier BitmapAdd)

K3D_TEST(bitmap.modifier.BitmapMultiply 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.modifier.BitmapMultiply.py
	REQUIRES K3D_BUILD_BITMAP_MODULE
	LABELS bitmap modifier BitmapMultiply)

K3D_TEST(bitmap.modifier.BitmapSubtract 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.modifier.BitmapSubtract.py
	REQUIRES K3D_BUILD_BITMAP_MODULE
	LABELS bitmap modifier BitmapSubtract)

K3D_TEST(bitmap.modifier.BitmapColorMonochrome 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.modifier.BitmapColorMonochrome.py
	REQUIRES K3D_BUILD_BITMAP_MODULE
	LABELS bitmap modifier BitmapColorMonochrome)

K3D_TEST(bitmap.modifier.BitmapGamma 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.modifier.BitmapGamma.py
	REQUIRES K3D_BUILD_BITMAP_MODULE
	LABELS bitmap modifier BitmapGamma)

K3D_TEST(bitmap.modifier.BitmapInvert 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.modifier.BitmapInvert.py
	REQUIRES K3D_BUILD_BITMAP_MODULE
	LABELS bitmap modifier BitmapInvert)

K3D_TEST(bitmap.modifier.BitmapMatteColorDiff 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.modifier.BitmapMatteColorDiff.py
	REQUIRES K3D_BUILD_BITMAP_MODULE
	LABELS bitmap modifier BitmapMatteColorDiff)

K3D_TEST(bitmap.modifier.BitmapMatteInvert 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.modifier.BitmapMatteInvert.py
	REQUIRES K3D_BUILD_BITMAP_MODULE
	LABELS bitmap modifier BitmapMatteInvert)

K3D_TEST(bitmap.modifier.BitmapThreshold 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.modifier.BitmapThreshold.py
	REQUIRES K3D_BUILD_BITMAP_MODULE
	LABELS bitmap modifier BitmapThreshold)







K3D_TEST(bitmap.modifier.CUDABitmapAdd 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.modifier.CUDABitmapAdd.py
	REQUIRES K3D_BUILD_BITMAP_MODULE K3D_BUILD_CUDA_MODULE
	LABELS bitmap modifier cuda)

K3D_TEST(bitmap.modifier.CUDABitmapMultiply 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.modifier.CUDABitmapMultiply.py
	REQUIRES K3D_BUILD_BITMAP_MODULE K3D_BUILD_CUDA_MODULE
	LABELS bitmap modifier cuda)

K3D_TEST(bitmap.modifier.CUDABitmapSubtract 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.modifier.CUDABitmapSubtract.py
	REQUIRES K3D_BUILD_BITMAP_MODULE K3D_BUILD_CUDA_MODULE
	LABELS bitmap modifier cuda)

K3D_TEST(bitmap.modifier.CUDABitmapColorMonochrome 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.modifier.CUDABitmapColorMonochrome.py
	REQUIRES K3D_BUILD_BITMAP_MODULE K3D_BUILD_CUDA_MODULE
	LABELS bitmap modifier cuda)

K3D_TEST(bitmap.modifier.CUDABitmapGamma 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.modifier.CUDABitmapGamma.py
	REQUIRES K3D_BUILD_BITMAP_MODULE K3D_BUILD_CUDA_MODULE
	LABELS bitmap modifier cuda)

K3D_TEST(bitmap.modifier.CUDABitmapInvert 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.modifier.CUDABitmapInvert.py
	REQUIRES K3D_BUILD_BITMAP_MODULE K3D_BUILD_CUDA_MODULE
	LABELS bitmap modifier cuda)

K3D_TEST(bitmap.modifier.CUDABitmapMatteColorDiff 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.modifier.CUDABitmapMatteColorDiff.py
	REQUIRES K3D_BUILD_BITMAP_MODULE K3D_BUILD_CUDA_MODULE
	LABELS bitmap modifier cuda)

K3D_TEST(bitmap.modifier.CUDABitmapMatteInvert 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.modifier.CUDABitmapMatteInvert.py
	REQUIRES K3D_BUILD_BITMAP_MODULE K3D_BUILD_CUDA_MODULE
	LABELS bitmap modifier cuda)

K3D_TEST(bitmap.modifier.CUDABitmapThreshold 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.modifier.CUDABitmapThreshold.py
	REQUIRES K3D_BUILD_BITMAP_MODULE K3D_BUILD_CUDA_MODULE
	LABELS bitmap modifier cuda)

K3D_TEST(bitmap.modifier.CUDABitmapAdd.benchmark 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.modifier.CUDABitmapAdd.benchmark.py
	REQUIRES K3D_BUILD_BITMAP_MODULE K3D_BUILD_CUDA_MODULE
	LABELS bitmap modifier cuda)

K3D_TEST(bitmap.modifier.CUDABitmapMultiply.benchmark 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.modifier.CUDABitmapMultiply.benchmark.py
	REQUIRES K3D_BUILD_BITMAP_MODULE K3D_BUILD_CUDA_MODULE
	LABELS bitmap modifier cuda)

K3D_TEST(bitmap.modifier.CUDABitmapSubtract.benchmark 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.modifier.CUDABitmapSubtract.benchmark.py
	REQUIRES K3D_BUILD_BITMAP_MODULE K3D_BUILD_CUDA_MODULE
	LABELS bitmap modifier cuda)

K3D_TEST(bitmap.modifier.CUDABitmapColorMonochrome.benchmark 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.modifier.CUDABitmapColorMonochrome.benchmark.py
	REQUIRES K3D_BUILD_BITMAP_MODULE K3D_BUILD_CUDA_MODULE
	LABELS bitmap modifier cuda)

K3D_TEST(bitmap.modifier.CUDABitmapGamma.benchmark 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.modifier.CUDABitmapGamma.benchmark.py
	REQUIRES K3D_BUILD_BITMAP_MODULE K3D_BUILD_CUDA_MODULE
	LABELS bitmap modifier cuda)

K3D_TEST(bitmap.modifier.CUDABitmapInvert.benchmark 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.modifier.CUDABitmapInvert.benchmark.py
	REQUIRES K3D_BUILD_BITMAP_MODULE K3D_BUILD_CUDA_MODULE
	LABELS bitmap modifier cuda)

K3D_TEST(bitmap.modifier.CUDABitmapMatteColorDiff.benchmark 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.modifier.CUDABitmapMatteColorDiff.benchmark.py
	REQUIRES K3D_BUILD_BITMAP_MODULE K3D_BUILD_CUDA_MODULE
	LABELS bitmap modifier cuda)

K3D_TEST(bitmap.modifier.CUDABitmapMatteInvert.benchmark 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.modifier.CUDABitmapInvert.benchmark.py
	REQUIRES K3D_BUILD_BITMAP_MODULE K3D_BUILD_CUDA_MODULE
	LABELS bitmap modifier cuda)

K3D_TEST(bitmap.modifier.CUDABitmapThreshold.benchmark 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.modifier.CUDABitmapThreshold.benchmark.py
	REQUIRES K3D_BUILD_BITMAP_MODULE K3D_BUILD_CUDA_MODULE
	LABELS bitmap modifier cuda)

K3D_TEST(bitmap.modifier.BenchmarkComparison
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.modifier.BenchmarkComparison.py
	REQUIRES K3D_BUILD_BITMAP_MODULE K3D_BUILD_CUDA_MODULE
	LABELS bitmap cuda)

