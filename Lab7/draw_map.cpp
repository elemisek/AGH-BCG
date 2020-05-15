#include <wx/wx.h>
#include "GUIMyFrame1.h"
// UWAGA: TO JEST JEDYNY PLIK, KTORY NALEZY EDYTOWAC **************************

double Shepard(int N, float d[100][3], float x, float y)
{
    double v = 0.;
	double w = 0.;
	double wk = 0.;

    for (int i = 0; i < N; i++)
	{
        double wk = 1. / std::abs((x - d[i][0]) * (x - d[i][0]) + (y - d[i][1]) * (y - d[i][1]));
        v += wk * d[i][2];
		w += wk;
    }
	return (v / w);
}

void GUIMyFrame1::DrawMap(int N, float d[100][3], bool Contour, int MappingType, int NoLevels, bool ShowPoints)
{
    wxMemoryDC memDC;
    memDC.SelectObject(MemoryBitmap);
    memDC.SetBackground(*wxWHITE_BRUSH);
    memDC.Clear();

	const int size = 500;
	double** shepard = new double* [size];
	for (int i = 0; i < size; i++)
		shepard[i] = new double[size];
	double dmin = d[0][2];
	double dmax = d[0][2];
	unsigned char* data = new unsigned char[size * size * 3];
	

	for (int i = 0; i < N; i++)
	{
		if (d[i][2] < dmin)
		{
			dmin = d[i][2];
		}
		if (d[i][2] > dmax)
		{
			dmax = d[i][2];
		}
	}

	for (int x = 0; x < size; x++)
	{
		for (int y = 0; y < size; y++)
		{
			shepard[y][x] = Shepard(N, d, y / 100. - 2.5, - x / 100. + 2.5);
		}
	}

	if (MappingType)
	{
		for (int x = 0; x < size; x++)
		{
			for (int y = 0; y < size; y++)
			{
				const int it = y * size * 3 + x * 3;
				if (MappingType == 1)
				{
					const float color = ((shepard[x][y] - dmin) / (dmax - dmin) - 0.5) * 2;
					data[it + 0] = 127 - 127 * color;
					data[it + 1] = 0;
					data[it + 2] = 127 + 127 * color;
				}
				else if (2 == MappingType)
				{
					const float color = ((shepard[x][y] - dmin) / (dmax - dmin) - 0.5) * 2;
					data[it + 0] = color < 0 ? 255 * std::abs(color) : 0;
					data[it + 1] = 255 - 255 * std::abs(color);
					data[it + 2] = color > 0 ? 255 * std::abs(color) : 0;
				}
				else if (MappingType == 3)
				{
					const float color = ((shepard[x][y] - dmin) / (dmax - dmin) - 0.5) * 2;
					const unsigned char value = 127 + 127 * color;
					data[it + 0] = value;
					data[it + 1] = value;
					data[it + 2] = value;
				}
			}
		}

		wxImage tmp(size, size, data);
		wxBitmap tmp2(tmp);
		memDC.DrawBitmap(tmp2, 0, 0);
	}

	if (ShowPoints)
	{
		memDC.SetPen(*wxBLACK);
		memDC.SetBrush(*wxTRANSPARENT_BRUSH);
		for (int i = 0; i < N; i++)
		{
			const int x = 100 * (d[i][0] + 2.5);
			const int y = 100 * (2.5 - d[i][1]);
			memDC.DrawLine(x, y + 4, x, y - 3);
			memDC.DrawLine(x - 3, y, x + 4, y);
		}
	}

	if (Contour)
	{
		memDC.SetPen(*wxBLACK);
		memDC.SetBrush(*wxTRANSPARENT_BRUSH);

		for (int level = 0; level < NoLevels; level++)
		{
			double threshold = dmin + (double(level + 1) * (dmax - dmin) / (double(NoLevels + 1)));
			for (int x = 0; x < size; x++)
				for (int y = 0; y < size; y++)
					if (shepard[y][x] > threshold)
						for (int i = -1; i <= 1; i++)
							for (int j = -1; j <= 1; j++)
								if (i && j && (y + i >= 0) && (y + i < size) && (y + j >= 0) && (x + j < size) && (shepard[y + i][x + j] < threshold))
									memDC.DrawPoint(y, x);
		}
	}

	for (int i = 0; i < size; i++)
		delete[] shepard[i];
	delete[] shepard;
}
