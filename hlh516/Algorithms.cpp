#include "pch.h"
#include "Algorithms.h"
#include <complex>
#include <math.h>
using namespace std;
BITMAPINFO* lpBitsInfo = NULL;
BITMAPINFO* lpDIB_IFFT = NULL;

BOOL LoadBmpFile(LPCTSTR BmpFileName)
{

	FILE* fp;
	BITMAPFILEHEADER bf;
	BITMAPINFOHEADER bi;

	if (0 != (fopen_s(&fp, BmpFileName, "rb")))
		return false;

	fread(&bf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&bi, sizeof(BITMAPINFOHEADER), 1, fp);

	DWORD NumColors;	//��ɫ�������е���ɫ����
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
	//�Ҷ�ͼ��ض�8λ
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
			sprintf(str, "�Ҷ�ֵ:%d", *pixel);
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
			strcpy(str, "������");
		else
			strcpy(str, "ǰ����");
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
//ͳ�ƻҶ�ͼ��ĻҶ�ֱ��ͼ
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
//TDΪָ��ʱ�������ָ��
//FDΪָ��Ƶ�������ָ��
//mΪ��ĸ���
//һλ��ɢ����Ҷ���任
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
//һά��ɢ����Ҷ���任
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
BITMAPINFO* lpDIB_FFT = NULL;
void FFT(complex<double>* TD, complex<double>* FD, int r)
{
	// ���㸶��Ҷ�任����
	LONG count = 1 << r;
	// �����Ȩϵ��
	int i;
	double angle;
	complex<double>* W = new complex<double>[count / 2];
	for (i = 0; i < count / 2; i++)
	{
		angle = -i * PI * 2 / count;
		W[i] = complex<double>(cos(angle), sin(angle));
	}
	// ��ʱ���д��X1
	complex<double>* X1 = new complex<double>[count];
	memcpy(X1, TD, sizeof(complex<double>) * count);

	// ���õ����㷨���п��ٸ���Ҷ�任�����ΪƵ��ֵX2
	complex<double>* X2 = new complex<double>[count];

	int k, j, p, size;
	complex<double>* temp;
	for (k = 0; k < r; k++)
	{
		for (j = 0; j < 1 << k; j++)
		{
			size = 1 << (r - k);
			for (i = 0; i < size / 2; i++)
			{
				p = j * size;
				X2[i + p] = X1[i + p] + X1[i + p + size / 2];
				X2[i + p + size / 2] = (X1[i + p] - X1[i + p + size / 2]) * W[i * (1 << k)];
			}
		}
		temp = X1;
		X1 = X2;
		X2 = temp;
	}

	// ����������λ�������У�
	for (j = 0; j < count; j++)
	{
		p = 0;
		for (i = 0; i < r; i++)
		{
			if (j & (1 << i))
			{
				p += 1 << (r - i - 1);
			}
		}
		FD[j] = X1[p];
		FD[j] /= count;
	}

	// �ͷ��ڴ�
	delete W;
	delete X1;
	delete X2;
}
//IFFT���任
void IFFT(complex<double>* FD, complex<double>* TD, int r)
{
	// ����Ҷ�任����
	LONG	count;
	// ���㸶��Ҷ�任����
	count = 1 << r;

	// ������������洢��
	complex<double>* X = new complex<double>[count];
	// ��Ƶ���д��X
	memcpy(X, FD, sizeof(complex<double>) * count);

	// ����
	for (int i = 0; i < count; i++)
		X[i] = complex<double>(X[i].real(), -X[i].imag());

	// ���ÿ��ٸ���Ҷ�任
	FFT(X, TD, r);

	// ��ʱ���Ĺ���
	for (int i = 0; i < count; i++)
		TD[i] = complex<double>(TD[i].real() * count, -TD[i].imag() * count);

	// �ͷ��ڴ�
	delete X;
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
	//ת��
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
void FFourier()
{
	//ͼ��Ŀ�Ⱥ͸߶�
	int width = lpBitsInfo->bmiHeader.biWidth;
	int height = lpBitsInfo->bmiHeader.biHeight;
	int LineBytes = (width * lpBitsInfo->bmiHeader.biBitCount + 31) / 32 * 4;
	//ָ��ͼ������ָ��
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[256];

	// FFT��ȣ�����Ϊ2�������η���
	int FFT_w = 1;
	// FFT��ȵ�����������������
	int wp = 0;
	while (FFT_w * 2 <= width)
	{
		FFT_w *= 2;
		wp++;
	}

	// FFT�߶ȣ�����Ϊ2�������η���
	int FFT_h = 1;
	// FFT�߶ȵ�����������������
	int hp = 0;
	while (FFT_h * 2 <= height)
	{
		FFT_h *= 2;
		hp++;
	}

	// �����ڴ�
	complex<double>* TD = new complex<double>[FFT_w * FFT_h];
	complex<double>* FD = new complex<double>[FFT_w * FFT_h];

	int i, j;
	BYTE* pixel;

	for (i = 0; i < FFT_h; i++)  // ��
	{
		for (j = 0; j < FFT_w; j++)  // ��
		{
			// ָ��DIB��i�У���j�����ص�ָ��
			pixel = lpBits + LineBytes * (height - 1 - i) + j;

			// ��ʱ��ֵ
			TD[j + FFT_w * i] = complex<double>(*pixel * pow(-1, i + j), 0);
		}
	}

	for (i = 0; i < FFT_h; i++)
	{
		// ��y������п��ٸ���Ҷ�任
		FFT(&TD[FFT_w * i], &FD[FFT_w * i], wp);
	}

	// �����м�任���
	for (i = 0; i < FFT_h; i++)
	{
		for (j = 0; j < FFT_w; j++)
		{
			TD[i + FFT_h * j] = FD[j + FFT_w * i];
		}
	}

	for (i = 0; i < FFT_w; i++)
	{
		// ��x������п��ٸ���Ҷ�任
		FFT(&TD[i * FFT_h], &FD[i * FFT_h], hp);
	}

	// ɾ����ʱ����
	delete TD;

	//����Ƶ��ͼ��
	//ΪƵ��ͼ������ڴ�
	LONG size = 40 + 1024 + LineBytes * height;
	lpDIB_FFT = (LPBITMAPINFO)malloc(size);
	if (NULL == lpDIB_FFT)
		return;
	memcpy(lpDIB_FFT, lpBitsInfo, size);

	//ָ��Ƶ��ͼ������ָ��
	lpBits = (BYTE*)&lpDIB_FFT->bmiColors[256];

	double temp;
	for (i = 0; i < FFT_h; i++) // ��
	{
		for (j = 0; j < FFT_w; j++) // ��
		{
			// ����Ƶ�׷���
			temp = sqrt(FD[j * FFT_h + i].real() * FD[j * FFT_h + i].real() +
				FD[j * FFT_h + i].imag() * FD[j * FFT_h + i].imag()) * 2000;

			// �ж��Ƿ񳬹�255
			if (temp > 255)
			{
				// ���ڳ����ģ�ֱ������Ϊ255
				temp = 255;
			}

			pixel = lpBits + LineBytes * (height - 1 - i) + j;

			// ����Դͼ��
			*pixel = (BYTE)(temp);
		}
	}

	gFD = FD;

}


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
	//ת��
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
	//delete gFD;
	gFD = NULL;
	
	free(lpBitsInfo);
	lpBitsInfo = lpDIB_IFT;
}
void IFFourier()
{
	//ͼ��Ŀ�Ⱥ͸߶�
	int width = lpBitsInfo->bmiHeader.biWidth;
	int height = lpBitsInfo->bmiHeader.biHeight;
	int LineBytes = (width * lpBitsInfo->bmiHeader.biBitCount + 31) / 32 * 4;

	// FFT��ȣ�����Ϊ2�������η���
	int FFT_w = 1;
	// FFT��ȵ�����������������
	int wp = 0;
	while (FFT_w * 2 <= width)
	{
		FFT_w *= 2;
		wp++;
	}

	// FFT�߶ȣ�����Ϊ2�������η���
	int FFT_h = 1;
	// FFT�߶ȵ�����������������
	int hp = 0;
	while (FFT_h * 2 <= height)
	{
		FFT_h *= 2;
		hp++;
	}

	// �����ڴ�
	complex<double>* TD = new complex<double>[FFT_w * FFT_h];
	complex<double>* FD = new complex<double>[FFT_w * FFT_h];

	int i, j;
	for (i = 0; i < FFT_h; i++)  // ��
		for (j = 0; j < FFT_w; j++)  // ��
			FD[j + FFT_w * i] = gFD[i + FFT_h * j];

	// ��ˮƽ������п��ٸ���Ҷ�任
	for (i = 0; i < FFT_h; i++)
		IFFT(&FD[FFT_w * i], &TD[FFT_w * i], wp);

	// �����м�任���
	for (i = 0; i < FFT_h; i++)
		for (j = 0; j < FFT_w; j++)
			FD[i + FFT_h * j] = TD[j + FFT_w * i];

	// �ش�ֱ������п��ٸ���Ҷ�任
	for (i = 0; i < FFT_w; i++)
		IFFT(&FD[i * FFT_h], &TD[i * FFT_h], hp);

	//Ϊ���任ͼ������ڴ�
	LONG size = 40 + 1024 + LineBytes * height;

	lpDIB_IFFT = (LPBITMAPINFO)malloc(size);
	if (NULL == lpDIB_IFFT)
		return;
	memcpy(lpDIB_IFFT, lpBitsInfo, size);

	//ָ�򷴱任ͼ������ָ��
	BYTE* lpBits = (BYTE*)&lpDIB_IFFT->bmiColors[256];
	BYTE* pixel;
	double temp;
	for (i = 0; i < FFT_h; i++) // ��
	{
		for (j = 0; j < FFT_w; j++) // ��
		{
			pixel = lpBits + LineBytes * (height - 1 - i) + j;
			temp = (TD[j * FFT_h + i].real() / pow(-1, i + j));
			if (temp < 0)
				temp = 0;
			else if (temp > 255)
				temp = 255;
			*pixel = (BYTE)temp;
		}
	}

	// ɾ����ʱ����
	delete FD;
	delete TD;
	delete gFD;
}


BOOL gFD_isValid()
{
	return (gFD != NULL);
}
//��ֵ�˲�
//Array:����ģ�壬3*3���飬��һά�����ʾ
void Template(int* Array, float coef)
{
	//ͼ��Ŀ�Ⱥ͸߶�
	int w = lpBitsInfo->bmiHeader.biWidth;
	int h = lpBitsInfo->bmiHeader.biHeight;

	//ÿ�е��ֽ���
	int LineBytes = ((lpBitsInfo->bmiHeader.biBitCount * lpBitsInfo->bmiHeader.biWidth) + 31) / 32 * 4;
	//ָ��ԭͼ�����ݵ�ָ��
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];

	//Ϊ��ͼ������ڴ�
	BITMAPINFO* new_lpBitsInfo;
	LONG size = sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD) + h * LineBytes;
	if (NULL == (new_lpBitsInfo = (LPBITMAPINFO)malloc(size)))
		return;

	//����BMP
	memcpy(new_lpBitsInfo, lpBitsInfo, size);
	//��ͼ����ʼλ��
	BYTE* lpNewBIts = (BYTE*)&new_lpBitsInfo->bmiColors[new_lpBitsInfo->bmiHeader.biClrUsed];

	int i, j, k, l;
	BYTE* pixel, * new_pixel;
	float result;

	//��
	for (i = 1; i < h - 1; i++) {
		//��
		for (j = 1; j < w - 1; j++) {
			new_pixel = lpNewBIts + LineBytes * (h - 1 - i) * j;
			result = 0;

			//3*3ģ���ڵ����غ�
			for (k = 0; k < 3; k++) {
				for (l = 0; l < 3; l++) {
					pixel = lpBits + LineBytes * (h - 1 - k) + j - 1 + l;

					result += (*pixel) * Array[k * 3 + l];
				}
			}

			result *= coef;
			if (result < 0) *new_pixel = 0;
			else if (result > 254) *new_pixel = 254;
			else *new_pixel = (BYTE)(result + 0.5);
		}
	}
	free(lpBitsInfo);
	lpBitsInfo = new_lpBitsInfo;
}

BYTE WINAPI GetMedianNum(BYTE* Array)
{
	int i, j;
	BYTE tmp;
	//Ҳ���ڵõ�һ�����������ͽ�������
	for (i = 0; i < 8; i++) {
		for (j = i + 1; i < 9; j++) {
			if (Array[i] > Array[j]) {
				tmp = Array[i];
				Array[i] = Array[j];
				Array[j] = tmp;
			}
		}
	}
	return Array[4];
}
//��ֵ�˲�
void MedianFilter()
{
	//ͼ��Ŀ�Ⱥ͸߶�
	int w = lpBitsInfo->bmiHeader.biWidth;
	int h = lpBitsInfo->bmiHeader.biHeight;

	//ÿ�е��ֽ���
	int LineBytes = ((lpBitsInfo->bmiHeader.biBitCount * lpBitsInfo->bmiHeader.biWidth) + 31) / 32 * 4;
	//ָ��ԭͼ�����ݵ�ָ��
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];

	//Ϊ��ͼ������ڴ�
	BITMAPINFO* new_lpBitsInfo;
	LONG size = sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD) + h * LineBytes;
	if (NULL == (new_lpBitsInfo = (LPBITMAPINFO)malloc(size)))
		return;

	//����BMP
	memcpy(new_lpBitsInfo, lpBitsInfo, size);
	//��ͼ����ʼλ��
	BYTE* lpNewBIts = (BYTE*)&new_lpBitsInfo->bmiColors[new_lpBitsInfo->bmiHeader.biClrUsed];

	int i, j, k, l;
	BYTE* pixel, * new_pixel;
	BYTE Value[9];	//3*3ģ��

	//��
	for (i = 1; i < h - 1; i++) {
		//��
		for (j = 1; j < w - 1; j++) {
			new_pixel = lpNewBIts + LineBytes * (h - 1 - i) * j;

			//3*3ģ���ڵ����غ�
			for (k = 0; k < 3; k++) {
				for (l = 0; l < 3; l++) {
					pixel = lpBits + LineBytes * (h - 1 - k) + j - 1 + l;

					Value[k * 3 + l] = *pixel;
				}
			}

			*new_pixel = GetMedianNum(Value);
		}
	}
	free(lpBitsInfo);
	lpBitsInfo = new_lpBitsInfo;
}

//�ݶ��񻯺���
void GradientSharp()
{
	//ͼ��Ŀ�Ⱥ͸߶�
	int w = lpBitsInfo->bmiHeader.biWidth;
	int h = lpBitsInfo->bmiHeader.biHeight;

	//ÿ�е��ֽ���
	int LineBytes = ((lpBitsInfo->bmiHeader.biBitCount * lpBitsInfo->bmiHeader.biWidth) + 31) / 32 * 4;
	//ָ��ԭͼ�����ݵ�ָ��
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];

	BYTE* lpSrc, * lpSrc1, * lpSrc2;

	int i, j;
	BYTE temp;

	//��
	for (i = 0; i < h - 1; i++) {
		//��
		for (j = 0; j < w - 1; j++) {
			//��i�У���j��
			lpSrc = lpBits + LineBytes * (h - 1 - i) + j;
			//��i+1�У���j��
			lpSrc1 = lpBits + LineBytes * (h - 2 - i) + j;
			//��i�У���j+1��
			lpSrc2 = lpBits + LineBytes * (h - 1 - i) + j + 1;
			//�ݶ�����
			temp = abs((*lpSrc) - (*lpSrc1)) + abs((*lpSrc) - (*lpSrc2));

			if (temp > 254)
				*lpSrc = 254;
			else
				*lpSrc = temp;
		}
	}
}