#include "pch.h"
#include "Algorithms.h"
#include <complex>
#include <math.h>
using namespace std;
BITMAPINFO* lpBitsInfo = NULL;


BOOL LoadBmpFile(LPCTSTR BmpFileName)
{

	FILE* fp;
	BITMAPFILEHEADER bf;
	BITMAPINFOHEADER bi;

	if (0 != (fopen_s(&fp, BmpFileName, "rb")))
		return false;

	fread(&bf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&bi, sizeof(BITMAPINFOHEADER), 1, fp);

	DWORD NumColors;	//调色板数组中的颜色个数
	if (bi.biClrUsed > 0)
		NumColors = bi.biClrUsed;
	else {
		switch (bi.biBitCount) {
		case 1:
			NumColors = 2;
			break;
		case 4:
			NumColors = 16;
			break;
		case 8:
			NumColors = 255;
			break;
		case 24:
			NumColors = 0;
			break;
		}
	}

	DWORD LineBytes = ((bi.biBitCount * bi.biWidth) + 31)/32 * 4;
	DWORD dataBytes = LineBytes * bi.biHeight;
	DWORD size = sizeof(BITMAPINFOHEADER) + NumColors * sizeof(RGBQUAD) + dataBytes;

	if (NULL == (lpBitsInfo = (BITMAPINFO*)malloc(size))) 
		return false;

	fseek(fp, sizeof(BITMAPFILEHEADER), SEEK_SET);
	fread(lpBitsInfo, size, 1, fp);
	lpBitsInfo->bmiHeader.biClrUsed = NumColors;
	
	fclose(fp);
	
	return true;
}

void gray()
{
	int w = lpBitsInfo->bmiHeader.biWidth;
	int h = lpBitsInfo->bmiHeader.biHeight;
	DWORD LineBytes = ((lpBitsInfo->bmiHeader.biBitCount * lpBitsInfo->bmiHeader.biWidth) + 31) / 32 * 4;
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];

	int LineBytes_Gray = (w * 8 + 31) / 32 * 4;
	LONG size = sizeof(BITMAPINFOHEADER) + 256*sizeof(RGBQUAD) + LineBytes_Gray * h;

	LPBITMAPINFO lpBitsInfo_Gray = (LPBITMAPINFO)malloc(size);
	memcpy(lpBitsInfo_Gray, lpBitsInfo, 40);
	lpBitsInfo_Gray->bmiHeader.biBitCount = 8;
	lpBitsInfo_Gray->bmiHeader.biClrUsed = 256;

	int i, j;
	for (i = 0; i < 256; ++i) {
		lpBitsInfo_Gray->bmiColors[i].rgbRed = i;
		lpBitsInfo_Gray->bmiColors[i].rgbGreen = i;
		lpBitsInfo_Gray->bmiColors[i].rgbBlue = i;
		lpBitsInfo_Gray->bmiColors[i].rgbReserved = 0;
	}
	BYTE* lpBits_Gray = (BYTE*)&lpBitsInfo_Gray->bmiColors[256];


	BYTE* R, * G, * B, avg, * pixel;
	
	for ( i = 0; i < h; ++i) {
		for (j = 0; j < w; ++j) {
			B = lpBits + LineBytes * (h-1-i) + j * 3;
			G = B + 1;
			R = G + 1;
			avg = (*R + *G + *B) / 3;
			
			pixel = lpBits_Gray + LineBytes_Gray * (h - i - 1) + j;
			*pixel = avg;
		}
	}
	free(lpBitsInfo);
	lpBitsInfo = lpBitsInfo_Gray;
}

bool IsGray()
{
	//灰度图像必定8位
	if (lpBitsInfo->bmiHeader.biBitCount != 8) return false;
	for (int i = 0; i < 256; i += 25) {
		if (lpBitsInfo->bmiColors[i].rgbRed == lpBitsInfo->bmiColors[i].rgbGreen
			&& lpBitsInfo->bmiColors[i].rgbRed == lpBitsInfo->bmiColors[i].rgbBlue)
			continue;
		else return false;
	}
	return true;
}

void Histogram();
void pixel(int i, int j, char* str)
{
	if (NULL == lpBitsInfo)
		return;

	int w = lpBitsInfo->bmiHeader.biWidth;
	int h = lpBitsInfo->bmiHeader.biHeight;
	DWORD LineBytes = ((lpBitsInfo->bmiHeader.biBitCount * lpBitsInfo->bmiHeader.biWidth) + 31) / 32 * 4;
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];

	if (i > h || j > w)
		return;

	BYTE* pixel, bv;
	int r, g, b;

	switch (lpBitsInfo->bmiHeader.biBitCount) {
	case 8:
		pixel = lpBits + LineBytes * (h - i - 1) + j;
		if (IsGray()) {
			sprintf(str, "灰度值:%d", *pixel);
			Histogram();
		}
		else {
			r = lpBitsInfo->bmiColors[*pixel].rgbRed;
			g = lpBitsInfo->bmiColors[*pixel].rgbGreen;
			b = lpBitsInfo->bmiColors[*pixel].rgbBlue;
			sprintf(str, "RGB(%d,%d,%d)", r, g, b);
		}
		break;
	case 24:
		pixel = lpBits + LineBytes * (h - i - 1) + j * 3;
		b = *pixel;
		g = *(pixel + 1);
		r = *(pixel + 2);
		sprintf(str, "RGB(%d,%d,%d)", r, g, b);
		break;
	case 1:
		bv = *(lpBits + LineBytes * (h - i - 1) + j /8) & (1<<(7-j%8));
		if (0 == bv)
			strcpy(str, "背景点");
		else
			strcpy(str, "前景点");
		break;
	case 4:
		pixel = lpBits + LineBytes * (h - i - 1) + j / 2;
		if (j % 2 == 0)
			*pixel = *pixel >> 4;
		else
			*pixel = *pixel & 15;

		r = lpBitsInfo->bmiColors[*pixel].rgbRed;
		g = lpBitsInfo->bmiColors[*pixel].rgbGreen;
		b = lpBitsInfo->bmiColors[*pixel].rgbBlue;
		sprintf(str, "RGB(%d,%d,%d)", r, g, b);
		break;
	}

}

extern DWORD H[256];
//统计灰度图像的灰度直方图
void Histogram()
{
	DWORD LineBytes = ((lpBitsInfo->bmiHeader.biBitCount * lpBitsInfo->bmiHeader.biWidth) + 31) / 32 * 4;
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];
	BYTE* pixel;
	int w = lpBitsInfo->bmiHeader.biWidth;
	int h = lpBitsInfo->bmiHeader.biHeight;

	for (int i = 0; i < 256; ++i) H[i] = 0;

	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			pixel = lpBits + LineBytes * (h - i - 1) + j;
			H[lpBitsInfo->bmiColors[*pixel].rgbRed] += 1;
		}
	}
}

void LinearTran(float a, float b)
{
	DWORD LineBytes = ((lpBitsInfo->bmiHeader.biBitCount * lpBitsInfo->bmiHeader.biWidth) + 31) / 32 * 4;
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];
	BYTE* pixel;
	int w = lpBitsInfo->bmiHeader.biWidth;
	int h = lpBitsInfo->bmiHeader.biHeight;

	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			pixel = lpBits + LineBytes * (h - i - 1) + j;
			float tmp = a * (*pixel) + b;
			if (tmp > 254) tmp = 254;
			if (tmp < 0) tmp = 0;
			*pixel = tmp;
		}
	}
}

void Equalize()
{
	DWORD LineBytes = ((lpBitsInfo->bmiHeader.biBitCount * lpBitsInfo->bmiHeader.biWidth) + 31) / 32 * 4;
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];
	int w = lpBitsInfo->bmiHeader.biWidth;
	int h = lpBitsInfo->bmiHeader.biHeight;

	BYTE* pixel;
	int i, j;
	BYTE Map[256];

	Histogram();
	DWORD tmp;
	for (i = 0; i < 255; i++) {
		tmp = 0;
		for (j = 0; j < i; ++j) {
			tmp += H[j];
		}
		Map[i] =(BYTE) (tmp * 254.0/(w * h)+0.5);
	}
	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			pixel = lpBits + LineBytes * (h - i - 1) + j;
			*pixel = Map[*pixel];
		}
	}
}
#define PI  3.1415926535
//TD为指向时域数组的指针
//FD为指向频域数组的指针
//m为点的个数
//一位离散傅里叶正变换
void FT(complex<double>* TD,complex<double>* FD,int m)	
{
	int x, u;
	double angle;
	for (u = 0; u < m;u++) {
		FD[u] = 0;
		for (x = 0; x < m;x++) {
			angle = -2 * PI * u * x / m;
			FD[u] += TD[x] * complex<double>(cos(angle), sin(angle));
		}
		FD[u] /= m;
	}
}
//一维离散傅里叶反变换
void IFT(complex<double>* FD, complex<double>* TD, int m)
{
	int x, u;
	double angle;
	for (x = 0; x < m; x++) {
		TD[x] = 0;
		for (u = 0; u < m; u++) {
			angle = 2 * PI * u * x / m;
			TD[x] += FD[u] * complex<double>(cos(angle), sin(angle));
		}
	}
}
complex<double>* gFD = NULL;
void Fourier()
{
	DWORD LineBytes = ((lpBitsInfo->bmiHeader.biBitCount * lpBitsInfo->bmiHeader.biWidth) + 31) / 32 * 4;
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];
	int w = lpBitsInfo->bmiHeader.biWidth;
	int h = lpBitsInfo->bmiHeader.biHeight;

	BYTE* pixel;
	complex<double>* TD = new complex<double>[w * h];
	complex<double>* FD = new complex<double>[w * h];

	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			pixel = lpBits + LineBytes * (h - i - 1) + j;
			TD[w * i + j] = complex<double>(*pixel * pow(-1, i + j), 0);
		}
	}

	for (int i = 0; i < h; ++i) {
		FT(&TD[w * i], &FD[i*w], w);
	}
	//转置
	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			TD[h * j + i] = FD[w * i + j];
		}
	}
	for (int j = 0; j < w; ++j) {
		FT(&TD[h * j], &FD[j*h], h);
	}

	LONG size = 40 + 1024 + LineBytes * h;
	LPBITMAPINFO lpDIB_FT = (LPBITMAPINFO)malloc(size);
	memcpy(lpDIB_FT, lpBitsInfo, size);

	lpBits = (BYTE*)&lpDIB_FT->bmiColors[256];

	double temp;
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; ++j) {
			pixel = lpBits + LineBytes * (h - 1 - i) + j;
			temp = sqrt(FD[j * h + i].real() * FD[j * h + i].real()+FD[j * h + i].imag() * FD[j * h + i].imag()) * 2000;
			if (temp > 254)
				temp = 254;
			*pixel = (BYTE)(temp);
		}
	}
	delete TD;
	//delete FD;
	gFD = FD;
	free(lpBitsInfo);
	lpBitsInfo = lpDIB_FT;
};

void IFourier()
{
	DWORD LineBytes = ((lpBitsInfo->bmiHeader.biBitCount * lpBitsInfo->bmiHeader.biWidth) + 31) / 32 * 4;
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];
	int w = lpBitsInfo->bmiHeader.biWidth;
	int h = lpBitsInfo->bmiHeader.biHeight;

	BYTE* pixel;
	complex<double>* TD = new complex<double>[w * h];
	complex<double>* FD = new complex<double>[w * h];

	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			FD[w * i + j] = gFD[h * j + i];
		}
	}
	for (int i = 0; i < h; ++i) {
		IFT(&FD[w * i], &TD[i * w], w);
	}
	//转置
	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			FD[h * j + i] = TD[w * i + j];
		}
	}
	for (int j = 0; j < w; ++j) {
		IFT(&FD[h * j], &TD[j * h], h);
	}


	LONG size = 40 + 1024 + LineBytes * h;
	LPBITMAPINFO lpDIB_IFT = (LPBITMAPINFO)malloc(size);
	memcpy(lpDIB_IFT, lpBitsInfo, size);

	lpBits = (BYTE*)&lpDIB_IFT->bmiColors[256];

	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; ++j) {
			pixel = lpBits + LineBytes * (h - 1 - i) + j;
			*pixel = (BYTE)(TD[j * h + i].real() / pow(-1, i + j));
		}
	}
	delete TD;
	delete FD;
	delete gFD;
	gFD = NULL;
	
	free(lpBitsInfo);
	lpBitsInfo = lpDIB_IFT;
}
BOOL gFD_isValid()
{
	return (gFD != NULL);
}