///////////////////////////////////////////////////
//
// License Agreement:
//
// The following are Copyright © 2002-2006, Casey Langen
//
// Sources and Binaries of: musikCore musikCube musikBox
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright 
//      notice, this list of conditions and the following disclaimer in the 
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the author nor the names of other contributors may 
//      be used to endorse or promote products derived from this software 
//      without specific prior written permission. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE. 
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "WmaDecoder.h"

WmaDecoder::WmaDecoder(const wchar_t* filename)
{
    this->refCount = 1;
    this->streamPos = 0;
    this->streamDuration = 0;
    this->isValid = false;
    this->canSeek = false;
    this->wmReader = NULL;
    this->metadataEditor = NULL;

    ::CoInitialize(NULL);

    if (::WMCreateSyncReader(NULL, WMT_RIGHT_PLAYBACK, &this->wmReader) == S_OK)
    {
        if (this->wmReader->Open(filename) == S_OK)
        {
            this->metadataEditor = new WmaMetadataEditor(filename);

            if(! metadataEditor->IsValid())
            {
                return;
            }

            if (this->QueryAudioProperties())
            {
                this->buffer = new Buffer(this, this->wmReader, this->outputStream, this->streamLengthBytes);
                this->isValid = true;
            }
        }
    }
}

WmaDecoder::~WmaDecoder()
{
    delete this->metadataEditor;
    delete this->buffer;
    this->wmReader->Release();

    ::CoFreeUnusedLibraries();
}

int WmaDecoder::GetChannels()
{
    return this->waveFormat.nChannels;
}

int WmaDecoder::GetSampleRate()
{
    return this->waveFormat.nSamplesPerSec;
}

bool WmaDecoder::CanPlay()
{
    return this->isValid;
}

bool WmaDecoder::CanSeek()
{
    return (this->canSeek != 0);
}

int WmaDecoder::GetDuration()
{
    return (int)(this->streamDuration / 10000);   // 100-nanoseconds to microseconds
}

int WmaDecoder::GetTime()
{
    float percentComplete = (float)this->streamPos / (float)this->streamLengthBytes;
    return (int)((float)GetDuration() * percentComplete);
}

bool WmaDecoder::Seek(int ms)
{
    QWORD offset = (QWORD) ms * 10000;

    HRESULT hr = this->wmReader->SetRange(offset, 0);

    if (hr == S_OK)
    {
        this->streamPos = this->SampleTimeToBytePosition(offset);
        return true;
    }

    return false;
}

unsigned int WmaDecoder::Decode(void* buffer, unsigned int length)
{
    unsigned int bytesRead = this->buffer->Read(buffer, length);

    this->streamPos =
        this->SampleTimeToBytePosition(this->buffer->GetPositionSamples());

    return bytesRead;
}

bool WmaDecoder::QueryAudioProperties()
{
    return (this->QueryDurationAndSeekable() && this->QueryWaveFormat());
}

bool WmaDecoder::QueryDurationAndSeekable()
{
    this->streamDuration = this->metadataEditor->GetQWORDAttribute(g_wszWMDuration);
    this->canSeek = this->metadataEditor->GetBOOLAttribute(g_wszWMSeekable);

    return (this->streamDuration != 0);
}

bool WmaDecoder::QueryWaveFormat()
{
    bool success = false;
    if (this->wmReader)
    {
        DWORD outputCount = 0;
        if (this->wmReader->GetOutputCount(&outputCount) == S_OK)
        {
            for (DWORD i = 0; i < outputCount; i++)
            {
                IWMOutputMediaProps* wmMediaProps;
                if (this->wmReader->GetOutputProps(i, &wmMediaProps) == S_OK)
                {
                    GUID mediaType;
                    wmMediaProps->GetType(&mediaType);

                    if (mediaType == WMMEDIATYPE_Audio)
                    {
                        if (this->wmReader->GetStreamNumberForOutput(i, &this->outputStream) != S_OK)
                        {
                            break;
                        }

                        DWORD mediaTypeSize;
                        wmMediaProps->GetMediaType(NULL, &mediaTypeSize);

                        WM_MEDIA_TYPE* mediaTypeStruct = (WM_MEDIA_TYPE*) malloc(mediaTypeSize);
                        wmMediaProps->GetMediaType(mediaTypeStruct, &mediaTypeSize);

                        if (mediaTypeStruct->formattype == WMFORMAT_WaveFormatEx)
                        {
                            memcpy((void*)&this->waveFormat, mediaTypeStruct->pbFormat, sizeof(WAVEFORMATEX));

                            this->streamLengthBytes = SampleTimeToBytePosition(this->streamDuration);

                            success = true;
                        }

                        free(mediaTypeStruct);

                        if (success)
                        {
                            break;
                        }
                    }
                }

                wmMediaProps->Release();
            }
        }
    }

    return success;
}

QWORD WmaDecoder::SampleTimeToBytePosition(QWORD sampleTime)
{
    QWORD bytePosition = sampleTime * (DWORD)this->waveFormat.nAvgBytesPerSec / 10000000;
    bytePosition -= (bytePosition % (DWORD)this->waveFormat.nBlockAlign);
    return bytePosition;
}

QWORD WmaDecoder::BytePositionToSampleTime(QWORD bytePosition)
{
    QWORD sampleTime = bytePosition - (bytePosition % (QWORD)this->waveFormat.nBlockAlign);
    return (QWORD)sampleTime * 10000000 / (QWORD)this->waveFormat.nAvgBytesPerSec;
}

////////////////////////////////////////////////////////////////////////////////

WmaDecoder::Buffer::Buffer(WmaDecoder* parent, IWMSyncReader* syncReader, WORD outputStream, QWORD streamLength)
{
    this->parent = parent;
    this->syncReader = syncReader;
    this->outputStream = outputStream;
    this->streamLength = streamLength;

    this->sampleTime = 0;
    this->sampleDuration = 0;
    this->inssBuffer = NULL;
    this->buffer = NULL;
    this->bufferLength = 0;
    this->bufferOffset = 0;
}

WmaDecoder::Buffer::~Buffer()
{
    if (inssBuffer)
    {
        inssBuffer->Release();
    }
}

unsigned int WmaDecoder::Buffer::Read(void* target, unsigned int length)
{
    unsigned int bytesRead = 0;

    while((this->GetPositionBytes() < this->streamLength) && (bytesRead < length))
    {
        unsigned int bytesRemaining = length - bytesRead;
        unsigned int bufferAvailable = this->bufferLength - this->bufferOffset;

        if (bufferAvailable == 0)
        {
            try
            {
                BufferNext();       // read next sample
            }
            catch(...)
            {
                return 0;           // error reading stream
            }

            bufferAvailable = this->bufferLength - this->bufferOffset;
        }

        unsigned int bytesToRead = (bufferAvailable > bytesRemaining) ? bytesRemaining : bufferAvailable;

        memcpy(
            (void*)((char*)target + bytesRead),
            (void*)((char*)this->buffer + this->bufferOffset), 
            bytesToRead);

        this->bufferOffset += bytesToRead;
        bytesRead += bytesToRead;
    }

    return bytesRead;
}

void WmaDecoder::Buffer::BufferNext()
{
    DWORD flags, outputNum;
    WORD streamNum;

    if (this->inssBuffer)
    {
        this->inssBuffer->Release();
        this->inssBuffer = NULL;
    }

    HRESULT hr = this->syncReader->GetNextSample(
        this->outputStream,
        &this->inssBuffer,
        &this->sampleTime,
        &this->sampleDuration,
        &flags,
        &outputNum,
        &streamNum);

    if (hr == S_OK)
    {
        this->inssBuffer->GetBufferAndLength((BYTE**)&this->buffer, &this->bufferLength);
        this->bufferOffset = 0;
    }
    else
    {
        throw 1;
    }
}

QWORD WmaDecoder::Buffer::GetPositionSamples()
{
    float percentSampleUsed = (float)this->bufferOffset / (float)this->bufferLength;
    QWORD usedOffset = (QWORD)((float)this->sampleDuration * percentSampleUsed);
    return this->sampleTime + usedOffset;
}

QWORD WmaDecoder::Buffer::GetPositionBytes()
{
    QWORD samplePositionBytes = parent->SampleTimeToBytePosition(this->sampleTime);
    return samplePositionBytes += this->bufferOffset;
}