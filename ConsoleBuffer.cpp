#include "ConsoleBuffer.h"
#include <assert.h>
#include <stdio.h>
#include <malloc.h>

ConsoleBuffer::ConsoleBuffer(HANDLE stdoutHandle)
{
    m_stdoutHandle = stdoutHandle;
    m_width = 0;
    m_height = 0;
    m_buffer = 0;
}

void ConsoleBuffer::Write(int x, int y, WORD attributes, const char* format, ...)
{
    assert(m_buffer != 0);
    assert(m_width > 0);
    assert(m_height > 0);

    va_list args;
    va_start(args, format);
    int count = _vscprintf(format, args);
    char* buffer = (char*)_alloca(count + 1);
    vsprintf_s(buffer, count + 1, format, args);

    int offset = x + (y * m_width);
    for (int i = 0; i < count; i++, offset++)
    {
        m_buffer[offset].Char.AsciiChar = buffer[i];
        m_buffer[offset].Attributes = attributes;
    }

    va_end(args);
}

void ConsoleBuffer::FillLine(int y, char c, WORD attributes)
{
    assert(m_buffer != 0);
    assert(m_width > 0);
    assert(m_height > 0);

    int offset = y * m_width;
    for (int i = 0; i < m_width; i++, offset++)
    {
        m_buffer[offset].Char.AsciiChar = c;
        m_buffer[offset].Attributes = attributes;
    }
}

void ConsoleBuffer::FillRect(int x, int y, int width, int height, char c, WORD attributes)
{
    assert(m_buffer != 0);
    assert(m_width > 0);
    assert(m_height > 0);

    for (int j = y; j < y + height; j++)
    {
        for (int i = x; i < x + width; i++)
        {
            CHAR_INFO& charInfo = m_buffer[i + (j * m_width)];
            charInfo.Char.AsciiChar = c;
            charInfo.Attributes = attributes;
        }
    }
}

void ConsoleBuffer::Clear()
{
    assert(m_buffer != 0);
    assert(m_width > 0);
    assert(m_height > 0);

    for (int i = 0; i < m_width * m_height; i++)
        m_buffer[i].Char.AsciiChar = ' ';
}

void ConsoleBuffer::OnWindowResize(int width, int height)
{
    delete[] m_buffer;
    m_width = width;
    m_height = height;
    m_buffer = new CHAR_INFO[width * height];
}

void ConsoleBuffer::Flush()
{
    assert(m_stdoutHandle != 0);
    assert(m_buffer != 0);
    assert(m_width > 0);
    assert(m_height > 0);

    COORD bufferSize;
    bufferSize.X = m_width;
    bufferSize.Y = m_height;

    COORD bufferCoord;
    bufferCoord.X = 0;
    bufferCoord.Y = 0;

    SMALL_RECT rect;
    rect.Top = 0;
    rect.Left = 0;
    rect.Bottom = m_height - 1;
    rect.Right = m_width - 1;

    WriteConsoleOutput(m_stdoutHandle, m_buffer, bufferSize, bufferCoord, &rect);
}
