#include <iostream>
#include <fstream>

struct BITMAPFILEHEADER
{
    unsigned short bfType;
    unsigned long bfSize;
    unsigned short  bfReserved1;
    unsigned short  bfReserved2;
    unsigned long bfOffBits;
};

struct BITMAPINFOHEADER 
{
    unsigned long biSize;
    unsigned long biWidth;
    unsigned long biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned long biCompression;
    unsigned long biSizeImage;
    unsigned long biXPelsPerMeter;
    unsigned long biYPelsPerMeter;
    unsigned long biClrUsed;
    unsigned long biClrImportant;
};

class Bitmap
{
    struct BITMAPFILEHEADER fileHeader;
    struct BITMAPINFOHEADER infoHeader;

    int bytesPerRow;
    unsigned char pallette[1024];
    unsigned char* pixels;
    bool created;
public:    
    Bitmap(int width, int height);
    Bitmap(const char* filename);
    ~Bitmap();
    int GetWidth();
    int GetHeight();
    unsigned char GetR(int x, int y);
    unsigned char GetG(int x, int y);
    unsigned char GetB(int x, int y);

    void SetR(int x, int y, unsigned char r);
    void SetG(int x, int y, unsigned char g);
    void SetB(int x, int y, unsigned char b);

    void Save(const char* filename);
};

int main(int argc, char* argv[])
{
    if (argc > 2)
    {
        Bitmap srcBmp(argv[1]);
        Bitmap dstBmp(srcBmp.GetWidth(), srcBmp.GetHeight());

        for (int i = 0; i < srcBmp.GetWidth(); i++)
        {
            for (int j = 0; j < srcBmp.GetHeight(); j++)
            {
                dstBmp.SetB(i, j, srcBmp.GetB(i, j));
                dstBmp.SetG(i, j, srcBmp.GetB(i, j));
                dstBmp.SetR(i, j, srcBmp.GetR(i, j));
            }
        }

        dstBmp.Save(argv[2]);
    }
    return 0;
}

Bitmap::Bitmap(int width, int height)
{
    created = true;
    
    bytesPerRow = width * 3;
    while (bytesPerRow % 4)    
        bytesPerRow++;

    pixels = new unsigned char[bytesPerRow * height];
    
    infoHeader.biSize = 40;
    infoHeader.biHeight = height;
    infoHeader.biWidth = width;
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = 24;
    infoHeader.biClrUsed = 0;
    infoHeader.biClrImportant = 0;
    infoHeader.biSizeImage = bytesPerRow * infoHeader.biHeight;
    infoHeader.biXPelsPerMeter = 0x256C;
    infoHeader.biYPelsPerMeter = 0x256C;
    infoHeader.biCompression = 0;
    
    fileHeader.bfType = 19778;
    fileHeader.bfReserved2 = 0;
    fileHeader.bfReserved1 = 0;
    fileHeader.bfOffBits = 0x36;
    fileHeader.bfSize = 54 + bytesPerRow * infoHeader.biHeight;
}

Bitmap::Bitmap(const char* filename)
{
    created = false;
    std::ifstream in(filename, std::ios_base::binary);
    if (in)
    {      
        in.read((char*)&fileHeader.bfType, 2);
        
        in.read((char*)&fileHeader.bfSize, 4);
        in.read((char*)&fileHeader.bfReserved1, 2);
        in.read((char*)&fileHeader.bfReserved2, 2);
        in.read((char*)&fileHeader.bfOffBits, 4);

        in.read((char*)&infoHeader.biSize, 4);
        in.read((char*)&infoHeader.biWidth, 4);
        in.read((char*)&infoHeader.biHeight, 4);
        in.read((char*)&infoHeader.biPlanes, 2);
        in.read((char*)&infoHeader.biBitCount, 2);
        in.read((char*)&infoHeader.biCompression, 4);
        in.read((char*)&infoHeader.biSizeImage, 4);
        in.read((char*)&infoHeader.biXPelsPerMeter, 4);
        in.read((char*)&infoHeader.biYPelsPerMeter, 4);
        in.read((char*)&infoHeader.biClrUsed, 4);
        in.read((char*)&infoHeader.biClrImportant, 4);

        if (infoHeader.biBitCount == 24)
        {
            bytesPerRow = infoHeader.biWidth * 3;
            while (bytesPerRow % 4)
                bytesPerRow++;

            pixels = new unsigned char[infoHeader.biHeight * bytesPerRow];
            in.read((char*)pixels, infoHeader.biHeight * bytesPerRow);
        }
        else if (infoHeader.biBitCount == 8)
        {
            if (!infoHeader.biClrUsed)
                infoHeader.biClrUsed = 256;

            in.read((char*)pallette, infoHeader.biClrUsed * 4);
            
            bytesPerRow = infoHeader.biWidth;
            while (bytesPerRow % 4)
                bytesPerRow++;

            pixels = new unsigned char[infoHeader.biHeight * bytesPerRow];
            in.read((char*)pixels, infoHeader.biHeight * bytesPerRow);
        }
        in.close();
    }
}

Bitmap::~Bitmap()
{
    if (pixels)
        delete pixels;
}

int Bitmap::GetWidth()
{
    return infoHeader.biWidth;
}

int Bitmap::GetHeight()
{
    return infoHeader.biHeight;
}

unsigned char Bitmap::GetR(int x, int y)
{
    if (infoHeader.biBitCount == 24)
        return pixels[bytesPerRow * y + x * 3 + 2];
    else 
    {
        int index = pixels[bytesPerRow * y + x];
        return pallette[4 * index + 2];
    }
}

unsigned char Bitmap::GetG(int x, int y)
{
    if (infoHeader.biBitCount == 24)
        return pixels[bytesPerRow * y + x * 3 + 1];
    else
    {
        int index = pixels[bytesPerRow * y + x];
        return pallette[4 * index + 1];
    }
}

unsigned char Bitmap::GetB(int x, int y)
{
    if (infoHeader.biBitCount == 24)
        return pixels[bytesPerRow * y + x*3];
    else
    {
        int index = pixels[bytesPerRow * y + x];
        return pallette[4 * index];
    }
}

void Bitmap::SetR(int x, int y, unsigned char r)
{
    pixels[bytesPerRow * y + x * 3 + 2] = r;    
}

void Bitmap::SetG(int x, int y, unsigned char g)
{
    pixels[bytesPerRow * y + x * 3 + 1] = g;
}

void Bitmap::SetB(int x, int y, unsigned char b)
{
    pixels[bytesPerRow * y + x * 3] = b;
}

void Bitmap::Save(const char* filename)
{
    std::ofstream out(filename, std::ios_base::binary);

    out.write((char*)&fileHeader.bfType, 2);
    out.write((char*)&fileHeader.bfSize, 4);
    out.write((char*)&fileHeader.bfReserved1, 2);
    out.write((char*)&fileHeader.bfReserved2, 2);
    out.write((char*)&fileHeader.bfOffBits, 4);

    out.write((char*)&infoHeader.biSize, 4);
    out.write((char*)&infoHeader.biWidth, 4);
    out.write((char*)&infoHeader.biHeight, 4);
    out.write((char*)&infoHeader.biPlanes, 2);
    out.write((char*)&infoHeader.biBitCount, 2);
    out.write((char*)&infoHeader.biCompression, 4);
    out.write((char*)&infoHeader.biSizeImage, 4);
    out.write((char*)&infoHeader.biXPelsPerMeter, 4);
    out.write((char*)&infoHeader.biYPelsPerMeter, 4);
    out.write((char*)&infoHeader.biClrUsed, 4);
    out.write((char*)&infoHeader.biClrImportant, 4);

    out.write((char*)pixels, bytesPerRow * infoHeader.biHeight);

    out.close();
}