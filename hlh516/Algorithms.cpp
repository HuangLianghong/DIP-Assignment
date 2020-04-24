#include "pch.h"
#include "Algorithms.h"
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

	for (int i = 0; i < w; ++i) {
		for (int j = 0; j < h; ++j) {
			pixel = lpBits + LineBytes * (h - i - 1) + j;
			H[lpBitsInfo->bmiColors[*pixel].rgbRed] += 1;
		}
	}
}