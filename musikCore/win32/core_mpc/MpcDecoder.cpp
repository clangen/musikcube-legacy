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
#include "MpcDecoder.h"

/////////////////////////////////////////////////////////////////////////////

// MpcDecoder

MpcDecoder::MpcDecoder(const wchar_t* filename)
{
    this->isValid = false;

    this->stream = _wfopen(filename, L"rb");
    mpc_reader_setup_file_reader(&this->readerFile, this->stream);

    // stream info
    mpc_streaminfo_init(&this->streamInfo);
    if (mpc_streaminfo_read(&this->streamInfo, &this->readerFile.reader) != ERROR_CODE_OK)
    {
        return;
    }

    // decoder
    mpc_decoder_setup(&this->decoder, &this->readerFile.reader);
    if (!mpc_decoder_initialize(&this->decoder, &this->streamInfo))
    {
        return;
    }

    this->durationMS = (unsigned int) (mpc_streaminfo_get_length(&this->streamInfo) * 1000);

    this->buffer = new Buffer(&this->decoder, 16, this->streamInfo.channels, this->readerFile.file_size);

    this->isValid = true;
}

MpcDecoder::~MpcDecoder()
{
    fclose(this->stream);
}

int MpcDecoder::GetNumChannels()
{
    return this->streamInfo.channels;
}

int MpcDecoder::GetSampleRate()
{
    return this->streamInfo.sample_freq;
}

bool MpcDecoder::CanPlay()
{
    return this->IsValid();
}

bool MpcDecoder::CanSeek()
{
    return true;
}

int MpcDecoder::GetDuration()
{
    return durationMS;
}

int MpcDecoder::GetTime()
{
    float multiplier = (float) this->decoder.DecodedFrames / (float) this->decoder.OverallFrames;
    return (int)((float)this->durationMS * multiplier);
}

bool MpcDecoder::Seek(int ms)
{
    return (mpc_decoder_seek_seconds(&this->decoder, ms / 1000) == 1);
}

unsigned int MpcDecoder::Decode(void* buffer, unsigned int length)
{
    return this->buffer->Read((short*) buffer, length);
}

/////////////////////////////////////////////////////////////////////////////

// Buffer

MpcDecoder::Buffer::Buffer(mpc_decoder* decoder, unsigned int bps, unsigned int numChannels, mpc_uint32_t streamLength)
{
    this->bufferLength = 0;
    this->bufferOffset = 0;
    this->streamLength = streamLength;
    this->decoder = decoder;
    this->bps = bps;
    this->numChannels = numChannels;
    this->floatScale = 1 << (this->bps - 1);
}

unsigned int MpcDecoder::Buffer::Read(short* target, unsigned int length)
{
    if (length % 2 != 0)
    {
        throw L"Requested read length not a multiple of 2";
    }

    unsigned int bytesRead = 0;
    unsigned int inputBufferPos = 0;

    while((this->decoder->WordsRead < this->streamLength) && (bytesRead < length))
    {
        unsigned int targetRemaining = length - bytesRead;
        unsigned int sourceAvailable = this->bufferLength - this->bufferOffset;

        if (sourceAvailable == 0)
        {
            BufferNext();       // read next sample
            sourceAvailable = this->bufferLength - this->bufferOffset;

            if (sourceAvailable == 0) // stream finished
            {
                return 0;
            }
        }

        unsigned int valuesToRead = 0;
        //
        if (sourceAvailable > (targetRemaining / 2))
        {
            valuesToRead = targetRemaining / 2;
        }
        else
        {
            valuesToRead = sourceAvailable;
        }

        unsigned int firstValue = this->bufferOffset;
        unsigned int lastValue = this->bufferOffset + valuesToRead;

        for (unsigned int i = firstValue; i < lastValue; i++)
        {
            target[inputBufferPos] = (short)((float)this->buffer[i] * (float)this->floatScale);

            inputBufferPos++;
            this->bufferOffset++;
            bytesRead += sizeof(short);
        }
    }

    if (bytesRead > length)
    {
        throw L"Bytes read exceeded stream length";
    }

    return bytesRead;
}
 
void MpcDecoder::Buffer::BufferNext()
{
    mpc_uint32_t size = mpc_decoder_decode(this->decoder, this->buffer, 0, 0);
    this->bufferOffset = 0;
    this->bufferLength = size * this->numChannels;
}
